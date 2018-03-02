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

#ifndef DTC_CONCURRENT_SHARED_GUARD_HPP
#define DTC_CONCURRENT_SHARED_GUARD_HPP

#include <memory>
#include <mutex>
#include <shared_mutex>

namespace dtc {

template <typename T, typename M = std::shared_mutex>
class SharedGuard {
   
  class UniqueDeleter {

    public:
     
      UniqueDeleter(std::unique_lock<M>&&);

      void operator()(T*);

    private:

		  std::unique_lock<M> _lock;
  };

  class SharedDeleter {

    public:
     
      SharedDeleter(std::shared_lock<M>&&);

      void operator()(const T*);

    private:

		  std::shared_lock<M> _lock;
  };

  using unique_handle = std::unique_ptr<T, UniqueDeleter>;
  using shared_handle = std::unique_ptr<const T, SharedDeleter>;

  public:
    
    template <typename... ArgsT>
    SharedGuard(ArgsT&&...);

    auto get();
    auto try_get();
    auto share() const;
    auto try_share() const;

  private:

    T _obj;
    mutable M _mutex;

};

// Deleter
template <typename T, typename M>
SharedGuard<T, M>::UniqueDeleter::UniqueDeleter(std::unique_lock<M>&& rhs) : 
  _lock {std::move(rhs)} {
}

// Operator
template <typename T, typename M>
void SharedGuard<T, M>::UniqueDeleter::operator()(T* ptr) {
  if(ptr) {
    _lock.unlock();
  }
}

// Deleter
template <typename T, typename M>
SharedGuard<T, M>::SharedDeleter::SharedDeleter(std::shared_lock<M>&& rhs) : 
  _lock {std::move(rhs)} {
}

// Operator
template <typename T, typename M>
void SharedGuard<T, M>::SharedDeleter::operator()(const T* ptr) {
  if(ptr) {
    _lock.unlock();
  }
}

// Constructor
template <typename T, typename M>
template <typename... ArgsT>
SharedGuard<T, M>::SharedGuard(ArgsT&&... args) : _obj {std::forward<ArgsT>(args)...} {
}

// Function: get
template <typename T, typename M>
auto SharedGuard<T, M>::get() {
  std::unique_lock l(_mutex);
  return unique_handle(&_obj, UniqueDeleter(std::move(l)));
}

// Function: try_get
template <typename T, typename M>
auto SharedGuard<T, M>::try_get() {
  std::unique_lock l(_mutex, std::try_to_lock);
	if(l) {
    return unique_handle(&_obj, UniqueDeleter(std::move(l)));
  }
  else {
    return unique_handle(nullptr, UniqueDeleter(std::move(l)));
  }
}

// Function: shared
template <typename T, typename M>
auto SharedGuard<T, M>::share() const {
  std::shared_lock l(_mutex);
  return shared_handle(&_obj, SharedDeleter(std::move(l)));
}

// Function: try_shared
template <typename T, typename M>
auto SharedGuard<T, M>::try_share() const {
  std::shared_lock l(_mutex, std::try_to_lock);
  if(l) {
    return shared_handle(&_obj, SharedDeleter(std::move(l)));
  }
  return shared_handle(nullptr, SharedDeleter(std::move(l)));
}

};  // End of namespace dtc. ----------------------------------------------------------------------



#endif



