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

#ifndef DTC_PROTOBUF_TASKINFO_HPP_
#define DTC_PROTOBUF_TASKINFO_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/common.hpp>

namespace dtc::pb {

// Struct: TaskInfo
struct TaskInfo {
  
  TaskID task_id;
  std::error_code errc;
  int status {0};

  TaskInfo(const TaskID&);
  TaskInfo(const TaskID&, const std::error_code&);
  TaskInfo(const TaskID&, const std::error_code&, int);
  TaskInfo() = default;
  TaskInfo(TaskInfo&&) = default;
  TaskInfo(const TaskInfo&) = default;

  TaskInfo& operator = (const TaskInfo&) = default;
  TaskInfo& operator = (TaskInfo&&) = default;

  bool has_error() const;
  bool exited() const;
  bool signaled() const;

  std::string status_to_string() const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(task_id, errc, status); 
  }

};

// Outputstream
std::ostream& operator<<(std::ostream&, const TaskInfo&);

};  // End of namespace dtc::pb. ------------------------------------------------------------


#endif




