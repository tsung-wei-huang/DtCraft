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

#include "dtc/protobuf/taskinfo.hpp"

namespace dtc::pb {

//// Constructor
//TaskInfo::TaskInfo(const TaskID& tid) : 
//  task_id {tid} {
//}
//
//// Constructor
//TaskInfo::TaskInfo(const TaskID& tid, const std::error_code& e) : 
//  task_id {tid}, errc {e} {
//}
//
//// Constructor
//TaskInfo::TaskInfo(const TaskID& tid, const std::error_code& e, int s) : 
//  task_id {tid}, errc {e}, status {s} {
//}

// Constructor  
TaskInfo::TaskInfo(const TaskID& k, std::string_view h, int s) :
  task_id {k}, agent {h}, status {s} {
}

// Function: has_error
bool TaskInfo::has_error() const {
  return status == -1 ||
         (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) || 
         WIFSIGNALED(status);
}

//// Function: exited
//bool TaskInfo::exited() const {
//  return WIFEXITED(status);
//}
//
//// Function: signaled
//bool TaskInfo::signaled() const {
//  return WIFSIGNALED(status);
//}

// Function: to_string
std::string TaskInfo::to_string() const {
  return "Task "s + task_id.to_string() + ' ' + status_to_string(status) + " @" + agent;
}

// Operator: <<
std::ostream& operator<<(std::ostream& os, const TaskInfo& rhs) {
  os << rhs.to_string();
  return os;
}

};  // End of namespace dtc::pb. ------------------------------------------------------------





