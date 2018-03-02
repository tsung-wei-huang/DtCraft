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

#ifndef DTC_EVENT_REACTOR_HPP_
#define DTC_EVENT_REACTOR_HPP_

#include <dtc/event/event.hpp>
#include <dtc/event/select.hpp>
#include <dtc/event/epoll.hpp>
#include <dtc/concurrent/mutex.hpp>
#include <dtc/concurrent/queue.hpp>
#include <dtc/concurrent/threadpool.hpp>
#include <dtc/utility/lambda.hpp>
#include <dtc/static/logger.hpp>

// TODO:
// 1. Measure the performance/throughput between using threadpool and thread
// 2. Use better data structure or STL container to hold acync events.

namespace dtc {

// Class: Reactor
// Reactor class provides a set of higher-level programming abstractions that simplify the 
// design and implementation of event-driven distributed applications. 
class Reactor {

  public:
  
    // Threadpool
    Threadpool _threadpool;
    
    // Notify event.
    std::atomic<bool> _notified {false};
    std::unique_ptr<Event> _notifier;

    // Reactor internal.
    bool _break_loop {false};
    std::chrono::steady_clock::time_point _sync_time_point {now()};
    ConcurrentQueue<std::function<void()>> _promises;

    // Event container.
    std::unordered_set<std::shared_ptr<Event>> _eventset;
    TimeoutEventHeap _timeoutpq;
    Select _demux;
    
    // Customized stopping criteria
    size_t _threshold {0};
    std::function<bool(Reactor&)> _break_loop_on;

  public:
    
    const std::thread::id owner {std::this_thread::get_id()};

    Reactor(unsigned = std::thread::hardware_concurrency());
    Reactor(const Reactor&) = delete;
    Reactor(Reactor&&) = delete;

    virtual ~Reactor(); 

    Reactor& operator = (const Reactor&) = delete;
    Reactor& operator = (Reactor&&) = delete;
    
    void dispatch();                                        

    template <typename C>
    void break_loop_on(C&&);

    inline void threshold(size_t);

    template <typename T, typename... ArgsT>
    auto insert(ArgsT&&...);

    std::future<bool> break_loop();

    inline size_t num_events() const;
    inline size_t num_workers() const;

    template <typename C>
    auto promise(C&&);

    template <typename C>
    auto async(C&&);

    inline bool is_owner() const;
    
    bool notify();

    auto remove(auto&&... events);
    auto freeze(auto&&... events);
    auto thaw(auto&&... events);

    void clear();

  private:
    
    void _carry_on_promises();
    void _poll_timeout_events();
    void _poll_io_events();
    void _activate_event(Event*);

    bool _remove(std::shared_ptr<Event>);
    bool _freeze(std::shared_ptr<Event>);
    bool _thaw(std::shared_ptr<Event>);
};

// Function: num_events
inline size_t Reactor::num_events() const {
  return _eventset.size();
}

// Function: num_workers
inline size_t Reactor::num_workers() const {
  return _threadpool.num_workers();
}

// Function: is_owner
// Query whether the caller is the owner of the reactor
inline bool Reactor::is_owner() const {
  return owner == std::this_thread::get_id();
}

// Function: threshold
inline void Reactor::threshold(size_t v) {
  _threshold = v;
}

// Procedure: break_loop_on
template <typename C>
void Reactor::break_loop_on(C&& c) {
  _break_loop_on = std::forward<C>(c);
}

// Function: async
// Assign a task
template <typename C>
auto Reactor::async(C&& c) {
  return _threadpool.async(std::forward<C>(c));
}

// Function: promise
// Let the main thread (reactor thread) to run the task of a callable type.
template<typename C>
auto Reactor::promise(C&& c) {
  
  using R = std::invoke_result_t<C>;
  
  std::promise<R> p;
  auto fu = p.get_future();
  
  // TODO: use packaged_task to avoid master thread call on fail?
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
 
// Function: remove
auto Reactor::remove(auto&&... events) {
  return promise([this, events...] () mutable {
    return std::make_tuple(_remove(std::move(events))...);
  });
}

// Function: freeze
auto Reactor::freeze(auto&&... events) {
  return promise([this, events...] () mutable {
    return std::make_tuple(_freeze(std::move(events))...);
  });
}

// Function: thaw
auto Reactor::thaw(auto&&... events) {
  return promise([this, events...] () mutable {
    return std::make_tuple(_thaw(std::move(events))...);
  });
}

// Function: insert
// The function creates and inserts an event and returns a future object wrapping the event. 
// The construction of the event happens at the caller's thread.
template <typename T, typename... ArgsT>
auto Reactor::insert(ArgsT&&... args) {

  return promise([this, event=std::make_shared<T>(std::forward<ArgsT>(args)...)] {

    _eventset.insert(event);                                                                     
    event->_reactor = this;  

    if constexpr(std::is_base_of_v<TimeoutEvent, T> || std::is_base_of_v<PeriodicEvent, T>) {    
      _timeoutpq.insert(event.get());                                                            
    }
    else if constexpr(std::is_base_of_v<ReadEvent, T>) {
      _demux._insert(event.get());
    }
    else if constexpr(std::is_base_of_v<WriteEvent, T>) {
    }
    else static_assert(dependent_false_v<T>);
     
    return event;
  });
}


};  // End of namespace dtc::reactor. -------------------------------------------------------------



#endif




