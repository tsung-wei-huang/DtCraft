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

#ifndef DTC_UTILITY_SCOPE_GUARD_HPP_
#define DTC_UTILITY_SCOPE_GUARD_HPP_

namespace dtc {

template <typename F>
class ScopeGuard {
  
  private:
    
    F _f;
    bool _active;

  public:

    ScopeGuard(F&& f) : _f {std::forward<F>(f)}, _active {true} {
    }

    ~ScopeGuard() {
      if(_active) _f();
    }

    void dismiss() noexcept {
      _active = false;
    }

    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator = (const ScopeGuard&) = delete;
    ScopeGuard& operator = (ScopeGuard&&) = delete;

    ScopeGuard(ScopeGuard&& rhs) : _f{std::move(rhs._f)}, _active {rhs._active} {
      rhs.dismiss();
    }
};

template <typename F> ScopeGuard(F&&) -> ScopeGuard<F>;

template <typename F>
auto make_scope_guard(F&& f) {
  return ScopeGuard<F>(std::forward<F>(f));
}


};  // end of namespace dtc. ----------------------------------------------------------------------



#endif


