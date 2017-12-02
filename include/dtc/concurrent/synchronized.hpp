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

#ifndef DTC_CONCURRENT_SYNCHRONIZED_HPP_
#define DTC_CONCURRENT_SYNCHRONIZED_HPP_

#include <shared_mutex>

namespace dtc {

template <typename T>
class Synchronized {
  
  public:
    
    template <typename... ArgsT>
    inline Synchronized(ArgsT&&... args) : _t{std::forward<ArgsT>(args)...} {}

    template <typename F>
    inline auto operator()(F&& f) {
      return (*this)(0, std::forward<F>(f));
    }

  private:
    
    T _t;
    mutable std::shared_mutex _m;

    template <typename F>
    inline auto operator()(int, F&& f) 
    -> decltype(std::forward<F>(f)(const_cast<const T&>(_t))) const {
      std::shared_lock<std::shared_mutex> slock(_m);
      return std::forward<F>(f)(_t);
    }

    template <typename F>
    inline auto operator()(char, F&& f) {
      std::unique_lock<std::shared_mutex> ulock(_m);
      return std::forward<F>(f)(_t);
    }

};


};  // End of namespace dtc. ----------------------------------------------------------------------

#endif



