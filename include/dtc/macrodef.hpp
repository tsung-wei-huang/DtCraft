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

#ifndef DTC_MACRODEF_HPP_
#define DTC_MACRODEF_HPP_

#include <exception>
#include <iostream>
#include <sstream>

// Constants
#define DTC_LOCALHOST "127.0.0.1"

// Code generator - concatenation.
#define DTC_CONCAT2_(a, b) a##b
#define DTC_CONCAT2(a, b) DTC_CONCAT2_(a, b)

#define DTC_CONCAT3_(a, b, c) a##b##c
#define DTC_CONCAT3(a, b, c) DTC_CONCAT3_(a, b, c)

#define DTC_CONCAT4_(a, b, c, d) a##b##c##d
#define DTC_CONCAT4(a, b, c, d) DTC_CONCAT4_(a, b, c, d)


// Code generator - class accessor.
//
// DTC_ACCESSOR(obj)
// ->
// constexpr const auto& obj() const { return _obj; }  // immutable accessor.
// inline auto& obj() { return _obj; }  // mutable accessor.
//
#define DTC_ACCESSOR(object) \
  inline const auto& object() const { return this->_##object; }

// Code generator - class mutator.
//
// DTC_MUTATOR(obj)
// ->
// template <typename T>
// inline void set_obj(T&& val) { _obj = std::forward<T>(val); }
//
#define DTC_MUTATOR(object) \
  template <typename T> \
  inline void set_##object(T&& t) { this->_##object = std::forward<T>(t); }

// Code generator - class object size query.
// 
// DTC_SIZE_QUERY(foo, thing) 
// ->
// constexpr size_t num_things() const { return foo().size() }
//
#define DTC_SIZE_QUERY(container, item) \
  inline size_t num_##item##s() const { return this->_##container.size(); }

// Code generator - disable class copy
//
// DTC_DISABLE_COPY
//
// object(const object&) = delete;
// object& operator = (const object&) = delete; 
//
#define DTC_DISABLE_COPY(object) \
  object(const object&) = delete;\
  object& operator = (const object&) = delete;

// Code generator - disable class move
//
// DTC_DISABLE_MOVE
//
// object(object&&) = delete;
// object& operator = (object&&) = delete; 
//
#define DTC_DISABLE_MOVE(object) \
  object(object&&) = delete;\
  object& operator = (object&&) = delete;

// Macro for generating critical section.
//
// Usage: 
//
//   DTC_LOCK_GUARD(m) {
//     ...
//   }
//
#define DTC_LOCK_GUARD_VAR DTC_CONCAT4(__lgvar, __COUNTER__, __, __LINE__)
#define DTC_LOCK_GUARD_LABEL DTC_CONCAT3(__lglabel, __, __LINE__)
#define DTC_LOCK_GUARD(m)                               \
  if(auto DTC_LOCK_GUARD_VAR = mt::make_lock_guard(&m)) { \
    abort();                                                 \
  } else                                                     \

// Macro for wrapping a static class variable.
//
// Example:
//
// inline static int& object() {
//   static int item;
//   return item;
// }
//
template <typename T> struct dtc_static_wrapper_helper;
template <typename T, typename U> struct dtc_static_wrapper_helper<T(U)> { using type = U; };

#define DTC_STATIC_WRAPPER(Type, Func)                                \
  inline static dtc_static_wrapper_helper<void(Type)>::type & Func(){ \
    static dtc_static_wrapper_helper<void(Type)>::type item;          \
    return item;                                                      \
  }

#endif

// Exception:
#define THROW(...) _throw(__FILE__, __LINE__, #__VA_ARGS__);

inline void _throw(const char* fname, const size_t line, auto&&... args) {
  std::ostringstream oss;
  oss << "[" << fname << ":" << line << "] ";
  (oss << ... << args);
  throw std::runtime_error(oss.str());
}

