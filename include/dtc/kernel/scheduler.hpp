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

#ifndef DTC_KERNEL_SCHEDULER_HPP_
#define DTC_KERNEL_SCHEDULER_HPP_

#include <dtc/protobuf/topology.hpp>

namespace dtc {

struct Bin {
  key_type key;
  pb::Resource resource;
};

struct Partition {
  key_type key;
  pb::Topology topology;
};

struct Deployment {
  std::vector<Partition> partitions;
};

// TODO (tsung-wei). Scheduler returns a deployment of type optional. 
// Null optional means the scheduler fails to find a deployment
// Otherwise, the scheduler can find a deployment (now or future).

std::vector<Partition> best_fit_bin_packing(const pb::Topology&, std::vector<Bin>&&);



};  // end of namespace scheduler. ----------------------------------------------------------------

#endif







