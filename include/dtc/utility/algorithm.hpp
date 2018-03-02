/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_ALGORITHM_HPP_
#define DTC_UTILITY_ALGORITHM_HPP_

#include <utility>
#include <algorithm>

namespace dtc {

// Function: transform_if
template <typename I, typename O, typename P, typename T>
auto transform_if(I first, I last, O res, P&& pred, T&& tran) {
  while(first != last) {
    if(pred(*first)) {
      *res++ = tran(*first);
    }
    ++first;
  }
  return res;
}

// Function: transform_if
template <typename From, typename To, typename P, typename T>
auto transform_if(From&& from, To&& to, P&& pred, T&& tran) {
  return transform_if(
    std::begin(from), std::end(from), std::inserter(to, std::end(to)), 
    std::forward<P>(pred),
    std::forward<T>(tran)
  );
}


};  // End of dtc namespace. ----------------------------------------------------------------------


#endif
