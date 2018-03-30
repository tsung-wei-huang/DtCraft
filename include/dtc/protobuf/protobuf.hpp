/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin and Martin D. F. Wong,   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_PROTOBUF_PROTOBUF_HPP_
#define DTC_PROTOBUF_PROTOBUF_HPP_

#include <dtc/protobuf/resource.hpp>
#include <dtc/protobuf/killtask.hpp>
#include <dtc/protobuf/topology.hpp>
#include <dtc/protobuf/frontier.hpp>
#include <dtc/protobuf/taskinfo.hpp>
#include <dtc/protobuf/solution.hpp>
#include <dtc/protobuf/brokenio.hpp>
#include <dtc/protobuf/loadinfo.hpp>

namespace dtc::pb { 

// Protobuf.
using Protobuf = std::variant<
  KillTask, 
  BrokenIO,
  Topology, 
  Resource, 
  LoadInfo,
  TaskInfo, 
  Solution
>;


};  // End of namespace dtc. --------------------------------------------------------------


#endif



