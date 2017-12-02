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

#include "dtc/protobuf/taskinfo.hpp"

namespace dtc::pb {

// Constructor
TaskInfo::TaskInfo(const TaskID& tid) : 
  task_id {tid} {
}

// Constructor
TaskInfo::TaskInfo(const TaskID& tid, const std::error_code& e) : 
  task_id {tid}, errc {e} {
}

// Constructor
TaskInfo::TaskInfo(const TaskID& tid, const std::error_code& e, int s) : 
  task_id {tid}, errc {e}, status {s} {
}

// Function: has_error
bool TaskInfo::has_error() const {
  return (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) || WIFSIGNALED(status);
}

// Function: exited
bool TaskInfo::exited() const {
  return WIFEXITED(status);
}

// Function: signaled
bool TaskInfo::signaled() const {
  return WIFSIGNALED(status);
}

// Function: status_to_string
std::string TaskInfo::status_to_string() const {
  if(WIFEXITED(status)) {
    return std::string("Exit ") + std::to_string(WEXITSTATUS(status));
  }
  else if(WIFSIGNALED(status)) {
    return std::string("Signal " + std::to_string(WTERMSIG(status)));
  }
  else {
    return "N/A";
  }
}

// Operator: <<
std::ostream& operator<<(std::ostream& os, const TaskInfo& rhs) {

  std::ostringstream oss;
  oss << "Task " << rhs.task_id.to_string() << "\n";
  oss << "errc " << rhs.errc.message() << "\n";
  oss << "status " << rhs.status << "\n";
  os << oss.str();

  return os;
}

};  // End of namespace dtc::pb. ------------------------------------------------------------





