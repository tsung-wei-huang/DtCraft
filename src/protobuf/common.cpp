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

#include <dtc/protobuf/common.hpp>

namespace dtc {
  
// Function: to_string
std::string TaskID::to_string() const {
  return std::to_string(graph) + "-" + std::to_string(topology);
}

// Operator: ==  
bool TaskID::operator == (const TaskID& rhs) const {
  return graph == rhs.graph && topology == rhs.topology;
}

// Outputstream 
std::ostream& operator<<(std::ostream& os, const TaskID& rhs) {
  os << rhs.to_string();
  return os;
}

};  // End of namespace dtc. ----------------------------------------------------------------------


