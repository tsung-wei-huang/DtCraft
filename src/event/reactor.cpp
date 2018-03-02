/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
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
Reactor::Reactor(unsigned num_workers) {

  // Ignore the signal sigpipe
  ::signal(SIGPIPE, SIG_IGN);

  // Enable the thread pool. To ensure correct functionality, each reactor must have 
  // at least one working thread.
  _threadpool.spawn(num_workers);
  
  // Initiate the notify event.
  if(auto fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK); fd == -1) {
    throw std::system_error(make_posix_error_code(errno), "Reactor failed to create notifier");
  }
  else {
    _notifier = std::make_unique<ReadEvent>(std::make_shared<Device>(fd), [](Event&){});
    _demux._insert(_notifier.get());
  }
}

// Destructor
Reactor::~Reactor() {

  // Fetch all events from the eventset to break all possible dependencies among event 
  // destructors that are defined by users.
  auto fetch = std::move(_eventset);

  // Disable the thread pool. Make sure all threads are dead before reactor is destroyed.
  _threadpool.shutdown();
}

// Procedure: clear
void Reactor::clear() {
  _timeoutpq.clear();
  _demux._clear();
  auto fetch = std::move(_eventset);
}

//// Function: _expired
//bool Reactor::_expired(const std::shared_ptr<Event>& event) const {
//  assert(is_owner());
//  return _eventset.find(event) == _eventset.end();
//}

//// Function: expired
//std::future<bool> Reactor::expired(const std::shared_ptr<Event>& event) {
//  return promise( [&, event] () { return _expired(event); } );
//}

// Function: _freeze
bool Reactor::_freeze(std::shared_ptr<Event> event) {

  if(_eventset.find(event) == _eventset.end()) return false;

  switch(event->type) {
    case Event::TIMEOUT:
    case Event::PERIODIC:
      _timeoutpq.remove(event.get());
    break;

    case Event::READ:
    case Event::WRITE:
      _demux._remove(event.get());
    break;

    default:
    break;
  };
  
  return true;
}

// Function: _thaw
bool Reactor::_thaw(std::shared_ptr<Event> event) {

  if(_eventset.find(event) == _eventset.end()) return false;

  switch(event->type) {
    case Event::TIMEOUT:
    case Event::PERIODIC:
      _timeoutpq.insert(event.get());
    break;

    case Event::READ:
    case Event::WRITE:
      _demux._insert(event.get());
    break;

    default:
    break;
  };

  return true;
}

// Function: _remove
// The public wrapper of the function to remove a list of events.
bool Reactor::_remove(std::shared_ptr<Event> event) {

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
    _threadpool.async([event=std::move(event)](){});

    return true;
  }

  return false;
}
    
// Function: notify
// Notify the reactor to wake up by the eventfd technique.
bool Reactor::notify() {
  if(_notified || is_owner()) return true;
  _notified = true;
  uint64_t c = 1;
  auto r = ::write(_notifier->device()->fd(), &c, sizeof(c));
  return (r >= 0 || errno == EAGAIN);
}

// Procedure: break_loop
std::future<bool> Reactor::break_loop() {
  return promise( [this] { 
    if(_break_loop == false) {
      return (_break_loop = true);
    }
    return false;
  });
}

// Procedure: dispatch 
// Dispatch the reactor into an event loop. Event loop is protected by the lock which enables
// safe operations applied to the reactor. The lock is released when the reactor calls IO 
// demux and sleep during the communication with kernel. 
void Reactor::dispatch() {

  if(!is_owner()) {
    throw std::runtime_error("Only the reactor owner can dispatch event(s)");
  }
  
  // Update the owner per dispatch call. 
  _break_loop = false;

  while(1) {

    // Carry on the promises
    _carry_on_promises();

    if(_break_loop || _eventset.size() <= _threshold || (_break_loop_on && _break_loop_on(*this))) {
      break;
    }

    // Activate the io events.
    _poll_io_events();

    // Synchronize the time point.
    _sync_time_point = now();

    // Activate timeout events.
    _poll_timeout_events();
  }
  
  _break_loop = true;
}

// Procedure: _carry_on_promises
void Reactor::_carry_on_promises() {
  assert(is_owner());
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
  if(e == _notifier.get()) {
    uint64_t c;
    while(::read(_notifier->device()->fd(), &c, sizeof(c)) > 0);
    _notified = false;
    return;
  }

  
  // Create a new ownership for worker thread.
  auto event = e->shared_from_this();

  switch(event->type) {
    
    // Case 1-1: Read event.
    case Event::READ:
      _demux._remove(event.get());
      _threadpool.async(
        [this, event=std::move(event)] () mutable {

          switch(auto s = event->_on(*event); s) {

            case Event::REMOVE:
              remove(std::move(event));
            break;

            default:
              promise([this, event=std::move(event)] {
                if(_eventset.find(event) != _eventset.end()) {
                  _demux._insert(event.get());
                }
              });
            break;
          }
        }
      );
    break;

    // Case 1-2: Write event.
    case Event::WRITE:
      _demux._remove(event.get());
      _threadpool.async(
        [this, event=std::move(event)] () mutable {
          switch(auto s = event->_on(*event); s) {
            case Event::REMOVE:
              remove(std::move(event));
            break;
            
            default:
            break;
          }
        }
      );
    break;
    
    // Case 2: Timeout event
    case Event::TIMEOUT:
      //_eventset.erase(event);
      _threadpool.async(
        [this, event=std::move(event)] () mutable {
          event->_on(*event);
          remove(std::move(event));
        }
      );
    break;

    // Case 3: Periodic event.
    case Event::PERIODIC:
      _threadpool.async(
        [this, event=std::move(event)] () mutable {
          // Adjust the timeout.
          switch(auto s = event->_on(*event); s) {

            case Event::REMOVE:
              remove(std::move(event));
            break;

            default:
              event->_timer().timeout = _sync_time_point + event->_timer().duration;
              promise([this, event=std::move(event)]() { 
                if(_eventset.find(event) != _eventset.end()) { 
                  _timeoutpq.insert(event.get()); 
                } 
              });
            break;
          }
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
    if(_timeoutpq.top()->_timer().timeout > _sync_time_point) break;

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
    if(event->_timer().timeout > _sync_time_point) {
      _demux._poll(event->_timer().timeout - _sync_time_point, [this] (Event* e) { _activate_event(e); });
    }
    // else to process the timeout first.
  }
  else {
    _demux._poll(std::chrono::milliseconds::max(), [this] (Event* e) { _activate_event(e); });
  }
}


};  // End of namespace dtc. ---------------------------------------------------------------



