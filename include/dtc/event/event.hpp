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

#ifndef DTC_EVENT_EVENT_HPP_
#define DTC_EVENT_EVENT_HPP_

#include <dtc/policy.hpp>
#include <dtc/utility/utility.hpp>
#include <dtc/device.hpp>

namespace dtc {

//------------------------------------------------------------------------------------------------

// Forward declaration.
class Reactor;
class Select;
class Epoll;
class TimeoutEventHeap;

// Class: Event
// The basic event class from which every customized event should inherit. Event is the basic unit
// of operation that supports exclusive types: (1) TIMEOUT, (2) PERIODIC, (3) READ, (4) WRITE, and
// (5) ASYNC. The event has protected constructor. Users are not allowed to create an event via
// this Event class. Instead, use provided base classes (scroll down) to customize events using
// class inheritance.
class Event : public std::enable_shared_from_this <Event> {

  friend class Reactor;
  friend class Select;
  friend class Epoll;
  friend class TimeoutEventHeap;
  
  public:

  enum Signal {
    REMOVE,
    DEFAULT
  };

  enum Type {
    TIMEOUT = 0,
    PERIODIC,
    READ,
    WRITE
  };

  struct Timer {
    int satellite;
    std::chrono::steady_clock::time_point::duration duration;
    std::chrono::steady_clock::time_point timeout;
  };

    const Type type;
    
    template <typename C>
    constexpr static auto make_on(C&&);

  private:

    Reactor* _reactor {nullptr};

    const std::function<Signal(Event&)> _on;

    std::variant<std::shared_ptr<Device>, Timer> _handle;
    
    inline Timer& _timer();

  protected:
    
    template <typename C>
    inline Event(const Type, std::shared_ptr<Device>, C&&);

    template <typename D, typename C>
    inline Event(const Type, D&&, const bool, C&&);
    
  public:
    
    Event() = delete;

    Event(const Event&) = delete;
    Event(Event&&) = delete;

    Event& operator()(const Event&) = delete;
    Event& operator()(Event&&) = delete;
    
    virtual ~Event() = default;
    
    inline Reactor* reactor() const;

    inline const Timer& timer() const;
    inline std::shared_ptr<Device> device();
}; 

// Event callback wrapper.
template <typename C>
constexpr auto Event::make_on(C&& c) {
  if constexpr(std::is_same_v<std::invoke_result_t<C, Event&>, Event::Signal>) {
    return std::forward<C>(c);
  }
  else {
    return [c=std::forward<C>(c)] (Event& e) mutable {
      c(e);
      return Event::DEFAULT;
    };
  }
}

// IO event constructor.
template <typename C>
inline Event::Event(const Type t, std::shared_ptr<Device> dev, C&& c) : 
  type {t},
  _on {make_on(std::forward<C>(c))},
  _handle {std::move(dev)} {
}

// Timeout event constructor.
template <typename D, typename C>
inline Event::Event(const Type t, D&& d, const bool from_now, C&& c) : 
  type {t},
  _on {make_on(std::forward<C>(c))},
  _handle { Timer{-1, d, from_now ? now() : now() + d} } {
}

// Function: reactor
// Return the non-owned pointer to the reactor
inline Reactor* Event::reactor() const {
  return _reactor;
}

// Function: timer
// Return the timer handle of the event.
inline const Event::Timer& Event::timer() const {
  return std::get<Timer>(_handle);
}

// Function: _timer
// Return the modifiable timer handle of the event.
inline Event::Timer& Event::_timer() {
  return std::get<Timer>(_handle);
}

// Function: device
inline std::shared_ptr<Device> Event::device() {
  return std::get<std::shared_ptr<Device>>(_handle);
}

//-------------------------------------------------------------------------------------------------

// class: TimeoutEvent
// Base class which every customized timeout event should inherit. Any derived classes must provide
// a timeout duration in the constructor. The timeout duration should be std::chrono duration types
// (std::chrono::seconds, std::chrono::milliseconds, etc.).
class TimeoutEvent : public Event {
  
  public:

    template <typename D, typename C>
    TimeoutEvent(D&& d, C&& c) : 
      Event(TIMEOUT, std::forward<D>(d), false, std::forward<C>(c)) {
    }

};

// Class: PeriodicEvent
// Base class which every customized periodic event should inherit. Any derived classes must provide
// a timeout duration in the constructor. The timeout duration should be std::chrono duration types
// (std::chrono::seconds, std::chrono::milliseconds, etc.).
class PeriodicEvent : public Event {

  public:
  
    template <typename D, typename C>
    PeriodicEvent(D&& d, const bool from_now, C&& c) : 
      Event(PERIODIC, std::forward<D>(d), from_now, std::forward<C>(c)) {
    }

};

// Class: ReadEvent
// Base class which every read event should inherit. The constructor should take a valid
// file descriptor that is opened in non-blocking read mode.
class ReadEvent : public Event {
  
  public:
    
    template <typename C>
    ReadEvent(std::shared_ptr<Device> dev, C&& c) : Event(READ, std::move(dev), std::forward<C>(c)) {
    }
};

// class: WriteEvent
// Base class which every write event should inherit. The constructor should take a valid
// file descriptor that is opened in non-blocking write mode.
class WriteEvent : public Event {
  
  public:
    
    template <typename C>
    WriteEvent(std::shared_ptr<Device> dev, C&& c) : Event(WRITE, std::move(dev), std::forward<C>(c)) {
    }
};

template <typename T> struct is_event {
  static constexpr bool value = 
    std::is_base_of_v<Event, T>         ||
    std::is_base_of_v<TimeoutEvent, T>  || 
    std::is_base_of_v<PeriodicEvent, T> ||
    std::is_base_of_v<ReadEvent, T>     ||
    std::is_base_of_v<WriteEvent, T>;
};

template <typename T> constexpr bool is_event_v = is_event<T>::value;

//-------------------------------------------------------------------------------------------------

// Class: TimeoutEventHeap
// Priority queue of the event. The priority queue is typically keyed on the timeout.
class TimeoutEventHeap {

  private:

    std::vector<Event*> _array;

  public:

    void clear();
    void remove(Event*);
    void insert(Event*);

    bool empty() const;
    
    const size_t size() const;

    Event* pop();
    Event* top() const;

  private:
    
    bool _less(Event*, Event*) const;
    void _bubble_up(size_t, Event*);
    void _bubble_down(size_t, Event*);
};




};  // End of namespace dtc -----------------------------------------------------------------------







#endif



