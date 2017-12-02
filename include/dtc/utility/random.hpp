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

#ifndef DTC_UTILITY_RANDOM_HPP_
#define DTC_UTILITY_RANDOM_HPP_

#include <string>
#include <random>
#include <algorithm>
#include <thread>

namespace dtc {

// Function: random
// Randomly generate a floating value in the given range.
template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, T> random(
  const T from = -1.0, 
  const T to = 1.0
) {
  thread_local std::mt19937 G {std::random_device{}()};
  return std::uniform_real_distribution<T>(from, to)(G);
}

// Function: random
// Randomly generate an integer value.
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T> random(
  const T from = std::numeric_limits<T>::lowest(), 
  const T to = std::numeric_limits<T>::max()
) {
  thread_local std::mt19937 G {std::random_device{}()};
  return std::uniform_int_distribution<T>(from, to)(G);
} 

// Function: random
// Randomly generate a string.
template <typename T>
std::enable_if_t<std::is_same<T, std::string>::value, T> random(
  const std::string::value_type from = ' ',
  const std::string::value_type to = '~',
  const std::string::size_type len = 16
) {
  std::string str(len, ' ');
  for(auto& c : str) {
    c = random<std::string::value_type>(from, to);
  }
  return str;
}

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif




