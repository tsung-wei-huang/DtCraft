/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_PROTOBUF_SOLUTION_HPP_
#define DTC_PROTOBUF_SOLUTION_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/taskinfo.hpp>

namespace dtc::pb {

// Struct: Solution
struct Solution {

  key_type graph {-1};
  std::error_code errc;
  std::list<TaskInfo> taskinfos;

  Solution() = default;
  Solution(const Solution&) = default;
  Solution(Solution&&) = default;
  Solution(key_type);
  
  Solution& operator = (const Solution&) = default;
  Solution& operator = (Solution&&) = default;

  size_t num_errors() const;

  std::string to_string() const;
  
  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(graph, errc, taskinfos); 
  }

};

// Outputstream
std::ostream& operator<<(std::ostream&, const Solution&);

};  // End of namespace dtc::pb. ------------------------------------------------------------


#endif




