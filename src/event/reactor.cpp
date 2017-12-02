/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/event/reactor.hpp>

namespace dtc {

// Constructor
Reactor::Reactor(unsigned num_workers) :
  _demux { [&](Event* event) { _activate_event(event); } } {

  // Enable the thread pool. To ensure correct functionality, each reactor must have 
  // at least one working thread.
  _threadpool.spawn(std::clamp(num_workers, 1u, std::thread::hardware_concurrency()));
  
  // Initiate the notify event.
  if(_notify_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK); _notify_fd == -1) {
    throw std::system_error(make_posix_error_code(errno), "Reactor failed");
  }
  _notify_event = std::make_unique<ReadEvent>(_notify_fd, [] (Event& e) { });
  _demux._insert(_notify_event.get());
}

// Destructor
Reactor::~Reactor() {

  // Fetch all events from the eventset to break all possible dependencies among event 
  // destructors that are defined by users.
  auto fetch = std::move(_eventset);

  // Close the event fd.
  ::close(_notify_fd);

  // Disable the thread pool. Make sure all threads are dead before reactor is destroyed.
  _threadpool.shutdown();
}

// Function: _expired
bool Reactor::_expired(const std::shared_ptr<Event>& event) const {
  assert(is_owner());
  return _eventset.find(event) == _eventset.end();
}

// Function: expired
std::future<bool> Reactor::expired(const std::shared_ptr<Event>& event) {
  return promise( [&, event] () { return _expired(event); } );
}

// Function: _freeze_impl
size_t Reactor::_freeze_impl(const std::shared_ptr<Event>& event) {
  
  assert(is_owner());

  if(_expired(event)) return 0;

  switch(event->type) {
    case Event::TIMEOUT:
    case Event::PERIODIC:
      _timeoutpq.remove(event.get());
    break;

    case Event::READ:
    case Event::WRITE:
      _demux._freeze(event.get());
    break;

    default:
    break;
  };

  return 1;
}


// Function: _thaw_impl
size_t Reactor::_thaw_impl(const std::shared_ptr<Event>& event) {
  
  assert(is_owner());
      
  if(_expired(event)) return 0;

  switch(event->type) {
    case Event::TIMEOUT:
    case Event::PERIODIC:
      _timeoutpq.insert(event.get());
    break;

    case Event::READ:
    case Event::WRITE:
      _demux._thaw(event.get());
    break;

    default:
    break;
  };

  return 1;
}

// Function: remove
// The public wrapper of the function to remove a list of events.
std::future<bool> Reactor::remove(std::shared_ptr<Event>&& event) {
  return promise([&, event=std::move(event)] () mutable { return _remove(std::move(event)); });
}
    
// Function: _remove
// Remove a given event.
bool Reactor::_remove(std::shared_ptr<Event>&& event) {
  
  assert(is_owner());

  if(auto itr = _eventset.find(event); itr != _eventset.end()) {
  
    switch(event->type) {
      
      // Timeout-related event.
      case Event::TIMEOUT:
      case Event::PERIODIC:
        _timeoutpq.remove(event.get());
      break;

      // Non-blocking IO event.
      case Event::READ:
      case Event::WRITE:
        _demux._remove(event.get());
      break;

      default:
        assert(false);
      break;
    };
    
    // Remove the event from the reactor
    _eventset.erase(itr);
    event->_reactor = nullptr;

    // Let the worker thread to perform the destructor.
    _threadpool.push_back([event=std::move(event)](){});

    return true;
  }
  else {
    return false;
  }
}

// Function: notify
// Notify the reactor to wake up by the eventfd technique.
bool Reactor::notify() {

  if(_notified) return true;

  _notified = true;
  uint64_t c = 1;
  auto r = ::write(_notify_fd, &c, sizeof(c));
  return (r >= 0 || errno == EAGAIN);
}

// Procedure: _shutdown
void Reactor::_shutdown() {
  assert(is_owner());
  _loopbreak = true;
}

// Procedure: shutdown
std::future<void> Reactor::shutdown() {
  return promise([&](){ _shutdown(); });
}

// Procedure: dispatch 
// Dispatch the reactor into an event loop. Event loop is protected by the lock which enables
// safe operations applied to the reactor. The lock is released when the reactor calls IO 
// demux and sleep during the communication with kernel. 
void Reactor::dispatch() {
  
  // Update the owner per dispatch call. 
  _owner = std::this_thread::get_id();
  _loopbreak = false;

  while(1) {

    // Carry on the promises
    _carry_on_promises();

    if(_loopbreak || _eventset.empty()) {
      break;
    }

    // Activate the io events.
    _poll_io_events();

    // Synchronize the time point.
    _sync_time_point = now();

    // Activate timeout events.
    _poll_timeout_events();

  }
}

// Procedure: _carry_on_promises
void Reactor::_carry_on_promises() {
  std::function<void()> c;
  while(_promises.try_dequeue(c)) {
    c(); 
  }
}

// Procedure: _activate_event
// On event activation, the procedure pushes an active event into the task queue.
// The procedure can only be called by the main thread (under reactor lock) sequentially.
//
// reactor ---- (create additional event owner) ----> worker threads
//
void Reactor::_activate_event(Event* e) {

  // Clear the notify event.
  if(e == _notify_event.get()) {
    uint64_t c;
    while(::read(_notify_fd, &c, sizeof(c)) > 0);
    _notified = false;
    return;
  }

  
  // Create a new ownership for worker thread.
  auto event = e->shared_from_this();

  switch(event->type) {
    
    // Case 1-1: Read event.
    case Event::READ:
      _demux._freeze(event.get());
      _threadpool.push_back(
        [&, event=std::move(event)] () mutable {
          event->_on(*event);
          promise([&, event=std::move(event)]() { 
            if(!_expired(event)) { 
              _demux._thaw(event.get());
            } 
          });
        }
      );
    break;

    // Case 1-2: Write event.
    case Event::WRITE:
      _demux._freeze(event.get());
      _threadpool.push_back(
        [&, event=std::move(event)] () mutable {
          event->_on(*event);
        }
      );
    break;
    
    // Case 2: Timeout event
    case Event::TIMEOUT:
      _eventset.erase(event);
      _threadpool.push_back(
        [&, event=std::move(event)] () {
          event->_on(*event);
        }
      );
    break;

    // Case 3: Periodic event.
    case Event::PERIODIC:
      _threadpool.push_back(
        [&, event=std::move(event)] () mutable {
          // Adjust the timeout.
          event->_timeout = _sync_time_point + event->_duration;
          event->_on(*event);
          promise([&, event=std::move(event)]() { 
            if(!_expired(event)) { 
              _timeoutpq.insert(event.get()); 
            } 
          });
        }
      );
    break;

    default:
    break;
  };
}

// Procedure: _poll_timeout_events 
// The procedure removes activate timeout events (those events with timeout value passing the
// last synchronization point) from the priority queue.
void Reactor::_poll_timeout_events() {

  while(!_timeoutpq.empty()) {

    // Stop if the minimum timeout is not yet reached.
    if(_timeoutpq.top()->_timeout > _sync_time_point) break;

    // Remove the timeout event.
    auto event = _timeoutpq.pop();
  
    // Insert the event into the active list.
    _activate_event(event);
  }
}

// Procedure: _poll_io_events
// The procedure extracts the activate events from the demux and insert them into the active event 
// list through the procedure on_activate_event.
void Reactor::_poll_io_events() {

  // Obtain the waiting time for this round.
  auto event = _timeoutpq.top();

  if(event != nullptr) {
    if(event->_timeout > _sync_time_point) {
      _demux._poll(event->_timeout - _sync_time_point);
    }
  }
  else {
    _demux._poll(std::chrono::milliseconds::max());
  }
}


};  // End of namespace dtc. ---------------------------------------------------------------



