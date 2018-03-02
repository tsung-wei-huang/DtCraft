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

#ifndef DTC_EXIT_HPP_
#define DTC_EXIT_HPP_

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>

namespace dtc {

constexpr int EXIT_BROKEN_CONNECTION = 100;
constexpr int EXIT_CRITICAL_STREAM = 101;
constexpr int EXIT_CONTAINER_EXEC_FAILED = 102;
constexpr int EXIT_VERTEX_EXEC_FAILED = 103;

std::string status_to_string(int);

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif

