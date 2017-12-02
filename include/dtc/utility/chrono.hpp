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

#ifndef DTC_UTILITY_CHRONO_HPP_
#define DTC_UTILITY_CHRONO_HPP_

#include <chrono>
#include <sys/time.h>

namespace dtc {

// Function: now
// Return the current clock tick (steady_clock).
inline static std::chrono::steady_clock::time_point now() {
  return std::chrono::steady_clock::now();
}

// Function: duration_cast
// chrono -> timeval
template <typename T, typename Rep, typename Period>
auto duration_cast (const std::chrono::duration<Rep, Period>& d) 
-> std::enable_if_t<std::is_same<T, struct timeval>::value, struct timeval> {

  struct timeval tv;
  std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);
  tv.tv_sec  = sec.count();
  tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
	
  return tv;
}

// Function: duration_cast
// timeval -> chrono
template <typename D>
auto duration_cast(const struct timeval& tv) {
  return std::chrono::duration_cast<D> (
    std::chrono::seconds(tv.tv_sec) + std::chrono::microseconds(tv.tv_usec)
  );
}



};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
