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

#ifndef DTC_EVENT_REACTOR_HPP_
#define DTC_EVENT_REACTOR_HPP_

#include <dtc/event/event.hpp>
#include <dtc/event/select.hpp>
#include <dtc/concurrent/queue.hpp>
#include <dtc/concurrent/threadpool.hpp>

// TODO:
// 1. Measure the performance/throughput between using threadpool and thread.detach
// 2. Use better data structure or STL container to hold acync events.

namespace dtc {

// Class: Reactor
// Reactor class provides a set of higher-level programming abstractions that simplify the 
// design and implementation of event-driven distributed applications. 
class Reactor {

  protected:

    Threadpool _threadpool;

  private:

    // Notify event.
    int _notify_fd {-1};
    std::atomic<bool> _notified {false};
    std::unique_ptr<Event> _notify_event;

    std::thread::id _owner {std::this_thread::get_id()};
    
    bool _loopbreak {false};
    std::chrono::steady_clock::time_point _sync_time_point {now()};
    ConcurrentQueue<std::function<void()>> _promises;

    // Event container data structure.
    std::unordered_set<std::shared_ptr<Event>> _eventset;
    TimeoutEventHeap _timeoutpq;
    Select _demux;

  public:

    Reactor(unsigned = std::thread::hardware_concurrency());
    Reactor(const Reactor&) = delete;
    Reactor(Reactor&&) = delete;

    virtual ~Reactor(); 

    Reactor& operator = (const Reactor&) = delete;
    Reactor& operator = (Reactor&&) = delete;
    
    void dispatch();                                        

    template <typename T, typename... ArgsT>
    std::future<std::shared_ptr<T>> insert(ArgsT&&...);

    std::future<bool> remove(std::shared_ptr<Event>&&);
    std::future<bool> expired(const std::shared_ptr<Event>&);
    
    template <typename... T>
    std::future<size_t> freeze(T&&...);
    
    template <typename... Ts>
    std::future<size_t> thaw(Ts&&...);

    inline size_t num_events() const;
    
    std::future<void> shutdown();
    
    template <typename C>
    std::future<std::result_of_t<C()>> promise(C&&);

    inline const std::thread::id& owner() const;
    inline bool is_owner() const;

    bool notify();

  private:
    
    void _carry_on_promises();
    void _poll_timeout_events();
    void _poll_io_events();
    void _activate_event(Event*);

    template <typename T>
    void _insert(const std::shared_ptr<T>&);

    size_t _freeze_impl(const std::shared_ptr<Event>&);
    size_t _thaw_impl(const std::shared_ptr<Event>&);

  protected:

    template <typename... Ts>
    size_t _freeze(Ts&&...);

    template <typename... Ts>
    size_t _thaw(Ts&&...);

    template <typename C>
    std::result_of_t<C()> _promise(C&&);

    template <typename T, typename... ArgsT>
    std::shared_ptr<T> _insert(ArgsT&&...);

    bool _remove(std::shared_ptr<Event>&&);
    bool _expired(const std::shared_ptr<Event>&) const;
    
    void _shutdown();
};

// Function: num_events
inline size_t Reactor::num_events() const {
  return _eventset.size();
}

// Function: freeze
// The public wrapper of the function to freeze a list of events
template <typename... Ts>
std::future<size_t> Reactor::freeze(Ts&&... events) {
  return promise([&, events...]() mutable { return _freeze(events...); });
}

// Function: _freeze
// Freeze a list of events.
template <typename... Ts>
size_t Reactor::_freeze(Ts&&... events) {
  assert(is_owner());
  size_t N {0};
  ((N += _freeze_impl(events)), ...);
  return N;
}

// Function: thaw
// The public wrapper of the function to thaw a list of events
template <typename... Ts>
std::future<size_t> Reactor::thaw(Ts&&... events) {
  return promise([&, events...]() mutable { return _thaw(events...); });
}

// Function: _thaw
// Thaw a list of events. Thawing an event means inserting the event back to the IO-demux or 
// the timeout priority queue. The reactor still holds an ownership to this event.
template <typename... Ts>
size_t Reactor::_thaw(Ts&&... events) {
  assert(is_owner());
  size_t N {0};
  ((N += _thaw_impl(events)), ...);
  return N;
}

// Function: owner
// Query the owner of the reactor (the dispatcher).
inline const std::thread::id& Reactor::owner() const {
  return _owner;
}

// Function: is_owner
// Query whether the caller is the owner of the reactor
inline bool Reactor::is_owner() const {
  return _owner == std::this_thread::get_id();
}

// Function: promise
// Let the main thread (reactor thread) to run the task of a callable type.
template<typename C>
std::future<std::result_of_t<C()>> Reactor::promise(C&& c) {
  
  using R = std::result_of_t<C()>;
  
  std::promise<R> p;
  auto fu = p.get_future();
  
  // Case 1: caller is the owner
  if(is_owner()) {
    if constexpr(std::is_same_v<void, R>) {
      c();
      p.set_value();
    }
    else {
      p.set_value(c());
    }
  } 
  // Case 2: caller is not the owner
  else {
    _promises.enqueue(make_strong_movable_closure(std::move(p), std::forward<C>(c)));
    notify();
  }

  //-------------------------------------------------------
  
  // Package the task and get the future of the task.
  //std::packaged_task<R()> task (std::forward<C>(c));
  //auto fu = task.get_future();

  //// Immediately invoke the task if we are the owner
  //if(is_owner()) {
  //  task();
  //}
  //// Calls from other threads should be deferred to promises queue.
  //else {
  //  _promises.enqueue(make_strong_movable_closure(std::move(task)));
  //}

  return fu;
}

// Function: _insert
template <typename T>
void Reactor::_insert(const std::shared_ptr<T>& event) {

  _eventset.insert(event);                                                                     
  event->_reactor = this;                                                                      

  if constexpr(std::is_base_of_v<TimeoutEvent, T> || std::is_base_of_v<PeriodicEvent, T>) {    
    _timeoutpq.insert(event.get());                                                            
  }                                                                                            
  else if constexpr(std::is_base_of_v<ReadEvent, T> || std::is_base_of_v<WriteEvent, T>) {
    _demux._insert(event.get());                                                                
    if(std::is_base_of_v<WriteEvent, T>) {                                                     
      _demux._freeze(event.get());                                                              
    }                                                                                          
  }
  else static_assert(dependent_false_v<T>);
}

// Function: insert
// The function creates and inserts an event and returns a future object wrapping the event. 
// The construction of the event happens at the caller's thread.
template <typename T, typename... ArgsT>
std::future<std::shared_ptr<T>> Reactor::insert(ArgsT&&... args) {
  return promise(
    [&, event=std::make_shared<T>(std::forward<ArgsT>(args)...)] () {
      _insert(event);
      return event;
    }
  );
}

// Function: _insert
// The protected version of the function 'insert'. It is the caller's responsibility to ensure 
// only the owner thread of the reactor can invoke this function call.
template <typename T, typename... ArgsT>
std::shared_ptr<T> Reactor::_insert(ArgsT&&... args) {
  assert(is_owner());
  auto event = std::make_shared<T>(std::forward<ArgsT>(args)...);
  _insert(event);
  return event;
}


};  // End of namespace dtc::reactor. -------------------------------------------------------------



#endif




