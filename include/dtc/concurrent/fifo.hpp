/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_CONCURRENT_FIFO_HPP_
#define DTC_CONCURRENT_FIFO_HPP_

#include <dtc/concurrent/mutex.hpp>

namespace dtc {

template <typename T, size_t C>
class ConcurrentFIFO {

  static_assert(C > 1u, "FIFO capacity should be at least 2");

  public:

    inline bool empty() const;
    
    template <typename I>
    bool push(I&&);
    
    bool pop(T&);

    inline size_t size() const;
    inline size_t capacity() const;

  private:

    inline size_t _forward(const size_t idx) const;

    std::atomic<size_t> _tail {0};
    std::atomic<size_t> _head {0};

    std::array<T, C> _array;
};

// Function: _forward
template <typename T, size_t C>
inline size_t ConcurrentFIFO<T, C>::_forward(const size_t idx) const {
  return (idx + 1) % C;
}

// Function: empty
template <typename T, size_t C>
inline bool ConcurrentFIFO<T, C>::empty() const {
  return _tail.load() == _head.load();
}

// Funcion: size
template <typename T, size_t C>
inline size_t ConcurrentFIFO<T, C>::size() const {
  return (_tail.load() - _head.load() + C) % C;
}

// Function: capacity
template <typename T, size_t C>
inline size_t ConcurrentFIFO<T, C>::capacity() const {
  return (_head.load() - _tail.load() + C - 1) % C;
}

// Function: push
template <typename T, size_t C>
template <typename I>
bool ConcurrentFIFO<T, C>::push(I&& item) {

	static_assert(std::is_same_v<std::decay_t<I>, T>);

  const auto curr = _tail.load(std::memory_order_relaxed);
  auto next = _forward(curr);
  if(next != _head.load(std::memory_order_acquire)) {
    _array[curr] = std::forward<I>(item);
    _tail.store(next, std::memory_order_release);
    return true;
  }
  return false;
}

// Function: pop
template <typename T, size_t C>
bool ConcurrentFIFO<T, C>::pop(T& item) {

  const auto curr = _head.load(std::memory_order_relaxed);

  if(curr == _tail.load(std::memory_order_acquire)) {
    return false;
  }

  if constexpr (std::is_move_assignable_v<T>) {
    item = std::move(_array[curr]);
  }
  else {
    item = _array[curr];
  }

  _head.store(_forward(curr), std::memory_order_release);

  return true;
}

};  // End of namespace dtc -----------------------------------------------------------------------


#endif




