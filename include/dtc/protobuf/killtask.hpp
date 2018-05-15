/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin and Martin D. F. Wong,   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_PROTOBUF_KILLTASK_HPP_
#define DTC_PROTOBUF_KILLTASK_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/common.hpp>

namespace dtc::pb {

// KillTask.
struct KillTask {
  
  TaskID task_id;
  
  KillTask(const TaskID& tid) : task_id {tid} {}
  KillTask() = default;
  KillTask(const KillTask&) = default;
  KillTask(KillTask&&) = default;
  ~KillTask() = default;

  KillTask& operator = (const KillTask&) = default;
  KillTask& operator = (KillTask&&) = default;
  
  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(task_id);
  }
};

};  // End of namespace dtc::pb. ------------------------------------------------------------

#endif




