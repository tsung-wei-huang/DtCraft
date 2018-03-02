/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang                                        *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_CONCURRENT_UNIQUE_GUARD_HPP
#define DTC_CONCURRENT_UNIQUE_GUARD_HPP

#include <memory>
#include <mutex>

namespace dtc {

template <typename T, typename M = std::mutex>
class UniqueGuard {

  class Deleter {

    public:
     
      Deleter(std::unique_lock<M>&&);

      void operator()(T*);

    private:

		  std::unique_lock<M> _lock;
  };
  
  using handle = std::unique_ptr<T, Deleter>;

  public:

    template <typename... ArgsT>
    UniqueGuard(ArgsT&&...);

    auto get();
    auto try_get();

  private:

    T _obj;
    M _mutex;
};

// Deleter
template <typename T, typename M>
UniqueGuard<T, M>::Deleter::Deleter(std::unique_lock<M>&& rhs) : _lock {std::move(rhs)} {
}

// Operator
template <typename T, typename M>
void UniqueGuard<T, M>::Deleter::operator()(T* ptr) {
  if(ptr) {
    _lock.unlock();
  }
}

// Constructor
template <typename T, typename M>
template <typename... ArgsT>
UniqueGuard<T, M>::UniqueGuard(ArgsT&&... args) : _obj {std::forward<ArgsT>(args)...} {
}

// Function: get
template <typename T, typename M>
auto UniqueGuard<T, M>::get() {
  std::unique_lock l(_mutex);
  return handle(&_obj, Deleter(std::move(l)));
}

// Function: try_get
template <typename T, typename M>
auto UniqueGuard<T, M>::try_get() {
  std::unique_lock l(_mutex, std::try_to_lock);
	if(l.owns_lock()) {
    return handle(&_obj, Deleter(std::move(l)));
  }
  else {
    return handle(nullptr, Deleter(std::move(l)));
  }
}


};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
