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

#ifndef DTC_UTILITY_LITERALS_HPP_
#define DTC_UTILITY_LITERALS_HPP_

namespace dtc {

inline namespace literals {

// Literal: KB
constexpr uintmax_t operator"" _KB (unsigned long long v) {
  return v*1024;
}

// Literal: MB
constexpr uintmax_t operator"" _MB (unsigned long long v) {
  return v*1024*1024;
}

// Literal: GB
constexpr uintmax_t operator"" _GB (unsigned long long v) {
  return v*1024*1024*1024;
}



};

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
