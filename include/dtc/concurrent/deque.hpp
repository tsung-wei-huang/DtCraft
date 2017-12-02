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

#ifndef DTC_CONCURRENT_DEQUE_HPP_
#define DTC_CONCURRENT_DEQUE_HPP_

#include <deque>
#include <dtc/concurrent/mutex.hpp>

namespace dtc::concurrent {

// Class: Deque
// Thread-safe deque under lock.
template <typename T, typename A = std::allocator<T>>
class Deque {
  
  private:

    SpinLock _lock;
    std::deque<T, A> _deque;

  public:
    
    using value_type = typename std::deque<T, A>::value_type;
    using allocator_type = typename std::deque<T, A>::allocator_type;
    using size_type = typename std::deque<T, A>::size_type;
    using reference = typename std::deque<T, A>::reference;
    using const_reference = typename std::deque<T, A>::const_reference;

    Deque() = default;
    ~Deque() = default;

    template <typename... ArgsT>
    inline void emplace_back(ArgsT&&...);

    template <typename... ArgsT>
    inline void emplace_front(ArgsT&&...);
    
    inline void push_back(T&&);

    inline void pop_front();
    inline void pop_back();
    inline void clear();

    inline std::deque<T, A> fetch();

    inline size_type size() const;

  private:

};

template <typename T, typename A>
template <typename... ArgsT>
inline void Deque<T, A>::emplace_back(ArgsT&&... args) {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.emplace_back(std::forward<ArgsT>(args)...);
}

template <typename T, typename A>
template <typename... ArgsT>
inline void Deque<T, A>::emplace_front(ArgsT&&... args) {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.emplace_front(std::forward<ArgsT>(args)...);
}

template <typename T, typename A>
inline void Deque<T, A>::pop_front() {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.pop_front();
}

template <typename T, typename A>
inline void Deque<T, A>::push_back(T&& v) {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.push_back(std::forward<T>(v));
}

template <typename T, typename A>
inline void Deque<T, A>::pop_back() {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.pop_back();
}

template <typename T, typename A>
inline std::deque<T, A> Deque<T, A>::fetch() {
  std::lock_guard<decltype(_lock)> lock(_lock);
  auto snapshot = std::move(_deque);
  return snapshot;
}

template <typename T, typename A>
inline typename Deque<T, A>::size_type Deque<T, A>::size() const {
  return _deque.size();
}

template <typename T, typename A>
inline void Deque<T, A>::clear() {
  std::lock_guard<decltype(_lock)> lock(_lock);
  _deque.clear();
}


};  // end of namespace dtc::concurrent. ----------------------------------------------------------


#endif


