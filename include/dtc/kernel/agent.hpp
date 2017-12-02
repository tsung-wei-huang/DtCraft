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

#ifndef DTC_KERNEL_AGENT_HPP_
#define DTC_KERNEL_AGENT_HPP_

#include <dtc/kernel/manager.hpp>
#include <dtc/kernel/container.hpp>

namespace dtc {

// Class: Agent
class Agent : public KernelBase {
  
  // ---- Internal data structure ---------------

  struct Topology {

    std::list<pb::Frontier> frontiers;

    std::optional<pb::Topology> topology;
    std::optional<size_t> num_frontiers;

    std::optional<int> stdout;
    std::optional<int> stderr;

    bool ready() const;
  };
  
  // ---- Actor ---------------------------------
  
  struct Master : ActorBase {

  }; 


  struct Executor : ActorBase {

    const TaskID key;

    Container container;

    Executor(const TaskID& k) : key{k} {}
  };

  private:

    Master _master;

    std::unordered_map<TaskID, Topology> _topologies;
    std::unordered_map<TaskID, Executor> _executors;
    
    bool _remove_executor(const TaskID&, bool, std::error_code);

    void _make_master();
    void _make_frontier_listener();
    void _schedule(pb::Frontier&);
    void _schedule(pb::Topology&, int, int);
    void _deploy(const TaskID&);

  public:

    Agent();
    ~Agent() = default;
    
    std::future<void> schedule(pb::Topology&&);
    std::future<void> schedule(pb::Frontier&&);

    std::future<bool> remove_executor(const TaskID&, bool);
};

};  // End of namespace dtc. --------------------------------------------------------------



#endif



