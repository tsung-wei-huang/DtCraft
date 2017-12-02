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

#include <dtc/protobuf/solution.hpp>

namespace dtc::pb {

// Constructor
Solution::Solution(key_type k) : graph {k} {
}

// Function: num_errors
size_t Solution::num_errors() const {
  return (errc ? 1 : 0) + std::count_if(
    taskinfos.begin(), taskinfos.end(), [] (const auto& t) { return t.has_error(); }
  );
}

// Function: to_string
std::string Solution::to_string() const {
  
  std::stringstream os;
  TableBuilder table;

  os << "[Graph " << graph << "]\n";

  table.add("Topology", "Status", "Message").end_of_row();

  for(const auto& t : taskinfos) {
    table.add(
      t.task_id.to_string(), 
      t.status_to_string(),
      t.errc.message()
    ).end_of_row();
  }
  
  os << table 
     << "Graph finished with " << num_errors() << " error(s)";
  
  if(errc) os << ": " << errc.message();

  return os.str();
}

// Operator: <<
std::ostream& operator << (std::ostream& os, const Solution& sol) {
  
  os << sol.to_string();
  
  return os;
}




};  // End of namespace dtc::pb. ------------------------------------------------------------------
