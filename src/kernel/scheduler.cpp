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

#include <dtc/kernel/scheduler.hpp>

namespace dtc {

// Function: best_fit_bin_packing
std::vector<Partition> best_fit_bin_packing(const pb::Topology& graph, std::vector<Bin>&& bins) {

  std::vector<Partition> result;

  std::unordered_map<key_type, size_t> c2p;   // Container to partition
  std::unordered_map<size_t, size_t> p2b;     // Partition to bin

  for(const auto& [ckey, container] : graph.containers) {

    size_t dst {bins.size()};

    // Find the best bin.
    for( size_t i = 0 ; i < bins.size() ; ++ i ){
      if( bins[i].resource >= container.resource ){
        if( dst == bins.size() || bins[dst].resource > bins[i].resource ){
          dst = i;
        }
      }
    }

    // If a valid bin exists
    if( dst != bins.size() ){

      auto &p = result.emplace_back( 
        Partition{ bins[dst].key, pb::Topology{graph.graph, static_cast<key_type>(result.size())} } 
      ).topology;

      p.file = graph.file;
      p.argv = graph.argv;
      p.envp = graph.envp;

			// emplace returns a pair with (iterator, true/false)
      p.containers.emplace(std::make_pair(ckey, container));

			// Copy those vertices belonging to this container.
			for(const auto & vkvp : graph.vertices) {
				if(vkvp.second.container == ckey) {
					p.vertices.emplace(vkvp);
				}
			}

      c2p[ckey] = p.id;
			p2b[p.id] = dst;
      // Update the resource usage 
      bins[dst].resource -= container.resource;
    }
    // If no valid bin exists 
    else{
      result.clear();
      goto done;
    }
  }

  // Copy those streams to the corresponding container.
  for(const auto &[skey, stream] : graph.streams){
    
    // Dig out the partition id of both sides.
    auto tpid = c2p[graph.vertices.at(stream.tail).container];
    auto hpid = c2p[graph.vertices.at(stream.head).container];
    
    // Tail-side partition.
    result[tpid].topology.streams[skey] = stream;
    result[tpid].topology.streams[skey].tail_host = bins[p2b[tpid]].resource.host;
    result[tpid].topology.streams[skey].head_host = bins[p2b[hpid]].resource.host;
    result[tpid].topology.streams[skey].tail_topology = tpid;
    result[tpid].topology.streams[skey].head_topology = hpid;
    
    // Skip if the tail and head are sitting at the same partition.
    if(hpid == tpid) continue;

    // Head-side partition.
    result[hpid].topology.streams[skey] = stream;
    result[hpid].topology.streams[skey].tail_host = bins[p2b[tpid]].resource.host;
    result[hpid].topology.streams[skey].head_host = bins[p2b[hpid]].resource.host;
    result[hpid].topology.streams[skey].tail_topology = tpid;
    result[hpid].topology.streams[skey].head_topology = hpid;
  }

done:
  return result;
}

// ------------------------------------------------------------------------------------------------


};  // End of namespace dtc. ----------------------------------------------------------------------
