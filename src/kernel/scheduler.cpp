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

#include <dtc/kernel/scheduler.hpp>
#include <dtc/kernel/master.hpp>

namespace dtc {

// Function: _try_enqueue
bool Master::_try_enqueue(Graph& graph) {
  
  assert(is_owner());
  
  std::unordered_map<key_type, key_type> c2a;

  for(const auto& [ckey, container] : graph.topology->containers) {

    key_type best {-1};

    // Find the largest bin to accommondate the container
    for(const auto& [akey, agent] : _agents) {
      if(agent.resource && *agent.resource >= container.resource) {
        if(best == -1 || *agent.resource < *_agents.at(best).resource) {
          best = akey;
        }
      }
    }

    // Failed to schedule.
    if(best == -1) {
      break;
    }
    // Assign container to bin
    else {
      c2a[ckey] = best;
      *_agents.at(best).resource -= container.resource; 
    }
  }

  // restore the resource
  for(const auto& [c, a] : c2a) {
    *_agents.at(a).resource += graph.topology->containers.at(c).resource;
  }
  
  // Failed to schedule the graph.
  if(c2a.size() != graph.topology->containers.size()) {
    return false;
  }

  return true;
}

// Function: _try_dequeue
bool Master::_try_dequeue(Graph& graph) {

  assert(is_owner());
  
  std::unordered_map<key_type, key_type> c2a;

  for(const auto& [ckey, container] : graph.topology->containers) {

    key_type best {-1};

    // Find the largest bin to accommondate the container
    for(const auto& [akey, agent] : _agents) {
      if(agent.released && *agent.released >= container.resource) {
        if(best == -1 || *agent.released < *_agents.at(best).released) {
          best = akey;
        }
      }
    }

    // Failed to schedule.
    if(best == -1) {
      break;
    }
    // Assign container to bin
    else {
      c2a[ckey] = best;
      *_agents.at(best).released -= container.resource; 
    }
  }

  // Restore the released
  if(c2a.size() != graph.topology->containers.size()) {
    for(const auto& [c, a] : c2a) {
      *_agents.at(a).released += graph.topology->containers.at(c).resource;
    }
    return false;
  }
  
  // Spawn thread and send the topology to the corresponding agent.
  std::vector<std::future<std::tuple<key_type, pb::Topology>>> futures;

  for(const auto& [c, a] : c2a) {

    futures.emplace_back(async([c, a, &graph, &agents=_agents, &c2a] () {

      auto tpg = graph.topology->extract(c);

      std::unordered_map<key_type, std::string> vhosts;

      for(const auto& kvp : tpg.streams) {
        const auto hc = graph.topology->vertices.at(kvp.second.head).container;
        const auto tc = graph.topology->vertices.at(kvp.second.tail).container;
        vhosts.try_emplace(kvp.second.head, agents.at(c2a.at(hc)).released->host);
        vhosts.try_emplace(kvp.second.tail, agents.at(c2a.at(tc)).released->host);
      }

      std::ostringstream oss;
      for(const auto& [vertex, host] : vhosts) {
        oss << vertex << ":" << host << " ";
      }
      tpg.runtime.vertex_hosts(oss.str());

      (*agents.at(a).ostream)(pb::Protobuf{tpg});

      return std::make_tuple(a, tpg);
    }));
  }
  
  // Synchronize all tasks.
  for(auto& fu : futures) {

    // Retrieve the topology.
    auto [akey, tpg] = fu.get();
    const auto task_id = tpg.task_id(); 

    // Set up the graph data structure.
    graph.taskmeta.try_emplace(task_id, akey);
    
    // Set up the agent data structure.
    auto& agent = _agents.at(akey);
    agent.taskmeta.try_emplace(task_id, std::move(tpg));
  }

  return true;
}


};  // End of namespace dtc. ----------------------------------------------------------------------
