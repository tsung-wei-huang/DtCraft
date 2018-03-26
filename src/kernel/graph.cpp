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

#include <dtc/kernel/graph.hpp>

namespace dtc {

//-------------------------------------------------------------------------------------------------
// VertexBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
VertexBuilder::VertexBuilder(Graph* g, key_type k) : 
  _graph {g}, key {k} {
}

VertexBuilder::VertexBuilder(const VertexBuilder& rhs) :
  _graph {rhs._graph}, key {rhs.key} {
}

// Function: tag
VertexBuilder& VertexBuilder::tag(std::string s) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, s=std::move(s)] (pb::Topology* tpg) mutable {
      // Local/distributed mode
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        G->_vertices.at(key)._tag = std::move(s);
      }
    }
  );
  return *this;
}

// Function: program
VertexBuilder& VertexBuilder::program(std::string cmd) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, cmd=std::move(cmd)] (pb::Topology* tpg) mutable {
      // Local/distributed mode
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        G->_vertices.at(key)._runtime.program(std::move(cmd));
      }
    }
  );
  return *this;
}

//-------------------------------------------------------------------------------------------------
// StreamBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
StreamBuilder::StreamBuilder(Graph* g, key_type k, std::optional<key_type> t, std::optional<key_type> h) :
  _graph {g}, key {k}, tail {t}, head {h} {
}

// Copy constructor
StreamBuilder::StreamBuilder(const StreamBuilder& rhs) : 
  _graph {rhs._graph}, key {rhs.key}, tail {rhs.tail}, head {rhs.head} {
}

// Function: critical
StreamBuilder& StreamBuilder::critical(bool flag) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, flag] (pb::Topology* tpg) {
      // Local/distributed mode
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_stream(key))) {
        G->_streams.at(key)._critical = flag;
      }
    }
  ); 
  return *this;
}

// Function: tag
StreamBuilder& StreamBuilder::tag(std::string s) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, s=std::move(s)] (pb::Topology* tpg) mutable {
      // Local/distributed mode
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_stream(key))) {
        G->_streams.at(key)._tag = std::move(s);
      }
    }
  ); 
  return *this;
}

//-------------------------------------------------------------------------------------------------
// ContainerBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
ContainerBuilder::ContainerBuilder(Graph* g, key_type k) : 
  _graph {g}, key {k} {
}

// Copy constructor
ContainerBuilder::ContainerBuilder(const ContainerBuilder& rhs) : 
  _graph {rhs._graph}, key {rhs.key} {
}

// Function: add
ContainerBuilder& ContainerBuilder::add(key_type v) {
  _graph->_tasks.emplace_back(
    [v=v, c=key] (pb::Topology* tpg) {
      // Insert the vertex to the container.
      if(tpg && tpg->topology == -1) {
        tpg->vertices.at(v).container = c;
      }
    }
  );
  return *this;
}

// Function: memory_limit_in_bytes
ContainerBuilder& ContainerBuilder::memory_limit_in_bytes(uintmax_t l) {
  _graph->_tasks.emplace_back(
    [l, c=key] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode.
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).resource.memory_limit_in_bytes = l;
      }
    }
  );
  return *this;
}

// Function: memory
ContainerBuilder& ContainerBuilder::memory(uintmax_t l) {
  return memory_limit_in_bytes(l);
}

// Function: space
ContainerBuilder& ContainerBuilder::space(uintmax_t l) {
  _graph->_tasks.emplace_back(
    [l, c=key] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode.
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).resource.space_limit_in_bytes = l;
      }
    }
  );
  return *this;
}

// Function: num_cpus
ContainerBuilder& ContainerBuilder::cpu(uintmax_t n) {
  _graph->_tasks.emplace_back(
    [n, c=key] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode.
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).resource.num_cpus = n;
      }
    }
  );
  return *this;
}

// Function: host
ContainerBuilder& ContainerBuilder::host(std::string host) {
  _graph->_tasks.emplace_back(
    [host=std::move(host), c=key] (pb::Topology* tpg) mutable {
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).host(std::move(host));
      }
    }
  );
  return *this;
}

//-------------------------------------------------------------------------------------------------
// ProberBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
ProberBuilder::ProberBuilder(Graph* g, key_type v) : _graph{g}, vertex{v} {
}

// Copy constructor
ProberBuilder::ProberBuilder(const ProberBuilder& rhs) : _graph{rhs._graph}, vertex{rhs.vertex} {
}

// Function: on
ProberBuilder& ProberBuilder::tag(std::string t) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=vertex, t=std::move(t)] (pb::Topology* tpg) mutable {
      // Case 1: vertex needs to be initialized (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        G->_probers.at(key)._tag = std::move(t);
      }
      // Case 2: no need to handle submit mode.
    }
  );
  return *this;
}

//-------------------------------------------------------------------------------------------------
// Graph
//-------------------------------------------------------------------------------------------------

// Function: vertex
VertexBuilder Graph::vertex() {

  auto k = _generate_key();
  
  _tasks.emplace_back(
    [G=this, k] (pb::Topology* tpg) {
      // Case 1: vertex needs to be initiated (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(k))) {
        //LOGI("creating a vertex: ", k);
        G->_vertices.try_emplace(k, k);
      }
      // Case 2: topology needs to be modified (submit mode)
      else if(tpg->topology == -1) {
        tpg->vertices.try_emplace(k, k);
      }
    }
  ); 
  
  return VertexBuilder(this, k);
}

// Function: _emplace_stream
void Graph::_emplace_stream(key_type key, key_type tail, key_type head) {
  _tasks.emplace_back(
    [G=this, k=key, t=tail, h=head] (pb::Topology* tpg) {
      // Case 1: vertex needs to be initiated (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_stream(k))) {
        auto tptr = G->_vertex(t);
        auto hptr = G->_vertex(h);
        auto sitr = G->_streams.try_emplace(k, k, tptr, hptr).first;
        if(tptr) tptr->_ostreams[k] = &(sitr->second);
        if(hptr) hptr->_istreams[k] = &(sitr->second);
      }
      // Case 2: topology needs to be modified (submit mode)
      else if(tpg->topology == -1) {
        tpg->streams.try_emplace(k, k, t, h);
      }
    }
  );
}

// Function: stream
StreamBuilder Graph::stream(PlaceHolder& s, key_type head) {
	if(!s.tail || s.head) {
    DTC_THROW("Failed to create stream to ", head, " through placeholder");
  }
  auto k = _generate_key();
  _emplace_stream(k, *s.tail, head);
  s._keys.push_back(k);
  return StreamBuilder(this, k, *s.tail, head);
}

// Function: stream
StreamBuilder Graph::stream(key_type tail, PlaceHolder& s) {
  if(s.tail || !s.head) {
    DTC_THROW("Failed to create stream from ", tail, " through placeholder");
  }
  auto k = _generate_key();
  _emplace_stream(k, tail, *s.head);
  s._keys.push_back(k);
  return StreamBuilder(this, k, tail, *s.head);
}

// Function: stream
StreamBuilder Graph::stream(key_type tail, key_type head) {
  auto k = _generate_key();
  _emplace_stream(k, tail, head);
  return StreamBuilder(this, k, tail, head);
}

// Function: container
ContainerBuilder Graph::container() {
  auto k = _generate_key();
  _tasks.emplace_back(
    [k] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode
      if(tpg && tpg->topology == -1) {
        tpg->containers.try_emplace(k, k);
      }
    }
  );
  return ContainerBuilder(this, k);
}

// Function: _vertex
Vertex* Graph::_vertex(key_type key) {
  if(auto itr = _vertices.find(key); itr != _vertices.end()) {
    return &(itr->second);
  }
  else return nullptr;
}

// Function: _stream
Stream* Graph::_stream(key_type key) {
  if(auto itr = _streams.find(key); itr != _streams.end()) {
    return &(itr->second);
  }
  else return nullptr;
}

// Function: _prober
Prober* Graph::_prober(key_type key) {
  if(auto itr = _probers.find(key); itr != _probers.end()) {
    return &(itr->second);
  }
  else return nullptr;
}

// Function: generate_key
// Generate a unique key on each call for graph components, i.e., vertex, stream, and container.
key_type Graph::_generate_key() const {
  static key_type k(0);
  return k++;
}

// Function: prober
ProberBuilder Graph::prober(key_type vkey) {

  _tasks.emplace_back(
    [G=this, vkey] (pb::Topology* tpg) {
      // Case 1: vertex needs to be initiated (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(vkey))) {
        //LOGI("creating a vertex: ", k);
        if(auto ptr = G->_vertex(vkey); ptr == nullptr) {
          DTC_THROW("Failed to create a prober on ", vkey);
        }
        else G->_probers.try_emplace(vkey, vkey, ptr);
      }
      // Case 2: topology needs to be modified (submit mode)
    }
  ); 
  
  return ProberBuilder(this, vkey);
}

// Procedure: _make
// Initialize the graph from a given topology. There are three difference cases:
// 1. The given topology is nullptr: This is the local mode where we have to initialize the graph
//    and launch it on the local host.
// 2. The given topology's id is -1: This is the submit mode where we don't initialize the graph
//    but only extract the key information for each vertex and stream to the given topology.
// 3. The given topology's id is not -1: This is the distributed mode, where we have to initialize
//    The graph from the given topology.
void Graph::_make(pb::Topology* tpg) {

  if(tpg) {
    _task_id = tpg->task_id();
  }

  for(auto& task : _tasks) {
    task(tpg);
  }

  _tasks.clear();
}

// Function: _topologize
// This procedure is used for submit mode where we create a topology and modify the topology to
// the graph defined by the user. Notice that we only extract key information for vertex, stream,
// and container. The callback and the actual initialization of the graph are not performed.
pb::Topology Graph::_topologize() {
  
  // Initialize the topology
  pb::Topology tpg;

  // Store the environment variables for this topology.
	tpg.runtime = environment_variables();

  // Initialize the topology from the graph.
  _make(&tpg);

  // --------------------
  // Sanity check       |
  // --------------------

  // Create a default container for unsigned vertices.
  auto itr = std::find_if(
    tpg.vertices.begin(), tpg.vertices.end(),
    [] (const auto& tpg) {
      return tpg.second.container == -1;
    }
  );

  if(itr != tpg.vertices.end()) {
    std::ostringstream oss;
    oss << "Add vertices";
    auto ckey = _generate_key();
    tpg.containers.try_emplace(ckey, ckey);
    for(auto& [k, v] : tpg.vertices) {
      if(v.container == -1) {
        v.container = ckey;
        oss << ' ' << k;
      }
    }
    oss << " to container " << ckey;
    LOGI(oss.str());
  }
  
  // Container must have resource assigned.
  for(auto& c : tpg.containers) {
    // CPU field.
    if(c.second.resource.num_cpus == 0) {
      c.second.resource.num_cpus = 1;
    }
    // Memory field
    if(c.second.resource.memory_limit_in_bytes == 0) {
      c.second.resource.memory_limit_in_bytes = 1_GB;
    }
    // Disk field
    if(c.second.resource.space_limit_in_bytes == 0) {
      c.second.resource.space_limit_in_bytes = 1_GB;
    }
  }

  return tpg;

}

};  // End of namespace dtc::graph. ----------------------------------------------------









