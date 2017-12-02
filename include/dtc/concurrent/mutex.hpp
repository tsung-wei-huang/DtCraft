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

#ifndef DTC_CONCURRENT_MUTEX_HPP_
#define DTC_CONCURRENT_MUTEX_HPP_

#include <dtc/headerdef.hpp>

namespace dtc {

// Class: SpinLock
class SpinLock {

  public:
    
    inline bool try_lock() { return !_m.test_and_set(std::memory_order_acquire); }
    inline void lock()     { while(_m.test_and_set(std::memory_order_acquire));  }
    inline void unlock()   { _m.clear(std::memory_order_release);                }
  
  private:
  
    std::atomic_flag _m {ATOMIC_FLAG_INIT};
};

//-------------------------------------------------------------------------------------------------

// Class: LockGuard
template <typename T>
class LockGuard {

  using acquire_t = void (*) (T*);
  using release_t = void (*) (T*);

  public:
    
    // Explicit constructor.
    explicit LockGuard(T* m, void (*acquire)(T*), void (*release)(T*)) :
      _m(m), _release(release) {
      acquire(m);
    }
    
    // Destructor.
    ~LockGuard() {
      _release(_m);
    }

    // Type conversion to bool.
    explicit operator bool () const { return false; }

  private:

    T* _m;
    release_t _release;
};

// Function: make_lock_guard - generic template
template <typename T>
LockGuard<T> make_lock_guard(T* );

// Function: make_lock_guard - partial specialization for std::mutex
template <>
inline LockGuard<std::mutex> make_lock_guard(std::mutex* m) {
  return LockGuard<std::mutex> (
    m,
    [](std::mutex* m) { m->lock(); },
    [](std::mutex* m) { m->unlock(); }
  );
}

// Function: make_lock_guard - partial specialization for ::atomic_flag
// Atomic flags are boolean atomic objects that support two operations: test-and-set and clear.
// Atomic flags are lock-free (this is the only type guaranteed to be lock-free on all library 
// atomic implementations).
template <>
inline LockGuard<std::atomic_flag> make_lock_guard(std::atomic_flag* flag) {
  return LockGuard<std::atomic_flag> (
    flag,
    [](std::atomic_flag* flag) {
      while (flag->test_and_set(std::memory_order_acquire));
    },
    [](std::atomic_flag* flag) {
      flag->clear(std::memory_order_release);
    }
  );
}

// Function: make_lock_guard - partial specialization for pthread_mutex_t
// Mutex variables must be declared with type pthread_mutex_t, and must be initialized before 
// they can be used. There are two ways to initialize a mutex variable:
// 
// 1. Statically, when it is declared. For example: 
//    - pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
// 2. Dynamically, with the pthread_mutex_init() routine. This method permits setting mutex object 
//    attributes, attr.
//
// Notice that on creation, the mutex is initially unlocked.
//
template <>
inline LockGuard<pthread_mutex_t> make_lock_guard(pthread_mutex_t* m) {
  return LockGuard<pthread_mutex_t>(
    m,
    [](pthread_mutex_t* m) { pthread_mutex_lock(m); },
    [](pthread_mutex_t* m) { pthread_mutex_unlock(m); }
  );
}


};  // End of namespace dtc::concurrent. ----------------------------------------------------------

#endif









