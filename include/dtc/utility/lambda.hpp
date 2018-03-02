/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_LAMBDA_HPP_
#define DTC_UTILITY_LAMBDA_HPP_

#include <future>

namespace dtc {

// Struct: StrongMovable
// A strong movable object which replace copy constructor with move constructor.
template <typename T>
struct StrongMovable {

  StrongMovable(T&& object) : object(std::move(object)) {}
  StrongMovable(const StrongMovable& other) : object(std::move(other.object)) {}
  StrongMovable(StrongMovable&& other) : object(std::move(other.object)) {}
  
  StrongMovable& operator=(StrongMovable&& rhs) { object = std::move(rhs); } 
  StrongMovable& operator=(const StrongMovable& rhs) { object = std::move(rhs); }

  T& get() { return object; }
  const T& get() const { return object; }

  mutable T object; 
};


// Function: make_strong_movable
// Return a copy-constructed StrongMovable object with automatic type deduction.
template <typename T>
StrongMovable<T> make_strong_movable(T&& object) {
  return StrongMovable<T>(std::forward<T>(object));
}

// Function: make_strong_movable_closure
// Create a closure for a given callable object. The input callable object must be
// movable only.
template <typename C>
auto make_strong_movable_closure(C&& callable) {
  return [item=make_strong_movable(std::forward<C>(callable))] () { item.object(); };
}

template <typename C, typename R = std::result_of_t<C()>>
auto make_strong_movable_closure(std::promise<R>&& p, C&& c) {
  return [p=StrongMovable(std::move(p)), c=StrongMovable(std::forward<C>(c))] () mutable {
    if constexpr(std::is_same_v<void, R>) {
      c.get()();
      p.get().set_value();
    }
    else {
      p.get().set_value(c.get()());
    }
  };
}

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
