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

#ifndef DTC_UTILITY_HASH_HPP_
#define DTC_UTILITY_HASH_HPP_

namespace dtc {

template <typename T>
void hash_combine(size_t& seed, const T& key) {
  seed ^= std::hash<T>()(key) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
