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

#ifndef DTC_EVENT_EVENT_HPP_
#define DTC_EVENT_EVENT_HPP_

#include <dtc/policy.hpp>
#include <dtc/utility.hpp>

namespace dtc {

//------------------------------------------------------------------------------------------------

// Forward declaration.
class Reactor;
class TimeoutEventHeap;

// Class: Event
// The basic event class from which every customized event should inherit. Event is the basic unit
// of operation that supports exclusive types: (1) TIMEOUT, (2) PERIODIC, (3) READ, (4) WRITE, and
// (5) ASYNC. The event has protected constructor. Users are not allowed to create an event via
// this Event class. Instead, use provided base classes (scroll down) to customize events using
// class inheritance.
class Event : public std::enable_shared_from_this <Event> {

  friend class Reactor;
  friend class TimeoutEventHeap;
  
  public:

  enum Type {
    TIMEOUT = 0,
    PERIODIC,
    READ,
    WRITE
  };
    
    const Type type;

  private:

    Reactor* _reactor {nullptr};

    const std::function<void(Event&)> _on;

    int _descriptor {-1};

    std::chrono::steady_clock::time_point::duration _duration {0};
    std::chrono::steady_clock::time_point _timeout {now()};

  protected:
    
    template <typename C>
    inline Event(const Type, const int, C&&);

    template <typename D, typename C>
    inline Event(const Type, D&&, const bool, C&&);
    
  public:
    
    Event() = delete;

    Event(const Event&) = delete;
    Event(Event&&) = delete;

    Event& operator()(const Event&) = delete;
    Event& operator()(Event&&) = delete;
    
    virtual ~Event() = default;

    inline int descriptor() const;
    inline Reactor* reactor() const;
}; 

// IO event constructor.
template <typename C>
inline Event::Event(const Type t, const int des, C&& c) : 
  type {t},
  _on {std::forward<C>(c)},
  _descriptor {des} {
}

// Timeout event constructor.
template <typename D, typename C>
inline Event::Event(const Type t, D&& d, const bool from_now, C&& c) : 
  type {t},
  _on {std::forward<C>(c)},
  _duration {std::forward<D>(d)},
  _timeout {from_now ? now() : now() + _duration} {
}

// Function: descriptor
// Return the descriptor which could be either the file descriptor or the satellite for timeout
// heap.
inline int Event::descriptor() const {
  return _descriptor;
}

// Function: reactor
// Return the non-owned pointer to the reactor
inline Reactor* Event::reactor() const {
  return _reactor;
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
    ReadEvent(const int fd, C&& c) : Event(READ, fd, std::forward<C>(c)) {
    }
};

// class: WriteEvent
// Base class which every write event should inherit. The constructor should take a valid
// file descriptor that is opened in non-blocking write mode.
class WriteEvent : public Event {
  
  public:
    
    template <typename C>
    WriteEvent(const int fd, C&& c) : Event(WRITE, fd, std::forward<C>(c)) {
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
    const Event* top() const;

  private:
    
    bool _less(Event*, Event*) const;
    void _bubble_up(size_t, Event*);
    void _bubble_down(size_t, Event*);
};




};  // End of namespace dtc -----------------------------------------------------------------------







#endif



