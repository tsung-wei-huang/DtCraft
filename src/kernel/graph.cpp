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

#include <dtc/kernel/graph.hpp>

namespace dtc {

//-------------------------------------------------------------------------------------------------
// Vertex
//-------------------------------------------------------------------------------------------------

// Constructor
Vertex::Vertex(key_type k) : key {k} {
}

//// Function: ostream
//std::shared_ptr<OutputStream> Vertex::ostream(key_type k) const {
//  if(auto itr = _ostreams.find(k); itr == _ostreams.end()) return nullptr;
//  else return itr->second->_ostream.lock();
//}

// Function: istream
//std::shared_ptr<InputStream> Vertex::istream(key_type k) const {
//  if(auto itr = _streams.find(k); itr == _streams.end()) return nullptr;
//  else return itr->second->_istream.lock();
//}

// Operator
Vertex& Vertex::operator()() {
  std::call_once(_once_flag, [&] () { _on(*this); });
  return *this;
}

// Function: ostream
//std::shared_ptr<OutputStream> Vertex::ostream(const StreamBuilder& s) const {
//  if(auto itr = _streams.find(s.key); itr == _streams.end()) return nullptr;
//  else return itr->second->ostream();
//}
//
//// Function: istream
//std::shared_ptr<InputStream> Vertex::istream(const StreamBuilder& s) const {
//  if(auto itr = _streams.find(s.key); itr == _streams.end()) return nullptr;
//  else return itr->second->istream();
//}

//-------------------------------------------------------------------------------------------------
// Stream
//-------------------------------------------------------------------------------------------------

// Constructor
Stream::Stream(key_type k, Vertex* t, Vertex* h) :
  key {k}, _tail {t}, _head {h} {
}

// Function: ostream
//std::shared_ptr<OutputStream> Stream::ostream() {
//  return _ostream.lock();
//}
//
//// Function: istream
//std::shared_ptr<InputStream> Stream::istream() {
//  return _istream.lock();
//}

// Function: is_intra_stream
//bool Stream::is_intra_stream() const {
//  return _tail && _head;
//}
//
//// Function: is_inter_stream
//bool Stream::is_inter_stream() const {
//  return (_tail && !_head) || (!_tail && _head);
//}

// Function: is_inter_stream    
bool Stream::is_inter_stream(std::ios_base::openmode m) {
  switch(m) {
    case std::ios_base::out:
      return _tail && !_head;
    break;

    case std::ios_base::in:
      return !_tail && _head;
    break;

    default:
      return false;
    break;
  }
}

Stream::Signal Stream::operator()(Vertex& v, InputStream& is) {
  return _on_istream(v(), is);
}

Stream::Signal Stream::operator()(Vertex& v, OutputStream& os) {
  return _on_ostream(v(), os);
}

//-------------------------------------------------------------------------------------------------
// VertexBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
VertexBuilder::VertexBuilder(Graph* g, key_type k) : 
  _graph {g}, key {k} {
}

//-------------------------------------------------------------------------------------------------
// StreamBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
StreamBuilder::StreamBuilder(Graph* g, key_type k) :
  _graph {g}, key {k} {
}

//-------------------------------------------------------------------------------------------------
// ContainerBuilder
//-------------------------------------------------------------------------------------------------

// Constructor
ContainerBuilder::ContainerBuilder(Graph* g, key_type k) : 
  _graph {g}, key {k} {
}

// Function: add
ContainerBuilder& ContainerBuilder::add(key_type v) {
  _graph->_tasks.emplace_back(
    [v=v, c=key] (pb::Topology* tpg) {
      // Insert the vertex to the container.
      if(tpg && tpg->id == -1) {
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
      if(tpg && tpg->id == -1) {
        tpg->containers.at(c).resource.memory_limit_in_bytes = l;
      }
    }
  );
  return *this;
}

// Function: num_cpus
ContainerBuilder& ContainerBuilder::num_cpus(uintmax_t n) {
  _graph->_tasks.emplace_back(
    [n, c=key] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode.
      if(tpg && tpg->id == -1) {
        tpg->containers.at(c).resource.num_cpus = n;
      }
    }
  );
  return *this;
}

// Function: rootfs
ContainerBuilder& ContainerBuilder::rootfs(const std::filesystem::path& path) {
  _graph->_tasks.emplace_back(
    [path, c=key] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode.
      if(tpg && tpg->id == -1) {
        tpg->containers.at(c).configs["rootfs"] = path;
      }
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
      if(tpg == nullptr || (tpg->id != -1 && tpg->has_vertex(k))) {
        //LOGI("creating a vertex: ", k);
        G->_vertices.try_emplace(k, k);
      }
      // Case 2: topology needs to be modified (submit mode)
      else if(tpg->id == -1) {
        tpg->vertices.try_emplace(k, k);
      }
    }
  ); 
  
  return VertexBuilder(this, k);
}

// Function: stream
StreamBuilder Graph::stream(key_type tail, key_type head) {
  
  auto k = _generate_key();
  
  _tasks.emplace_back(
    [G=this, k, t=tail, h=head] (pb::Topology* tpg) {
      // Case 1: vertex needs to be initiated (local/distributed mode)
      if(tpg == nullptr || (tpg->id != -1 && tpg->has_stream(k))) {
        auto tptr = G->_vertices.find(t) == G->_vertices.end() ? nullptr : &(G->_vertices.at(t));
        auto hptr = G->_vertices.find(h) == G->_vertices.end() ? nullptr : &(G->_vertices.at(h));
        auto sitr = G->_streams.try_emplace(k, k, tptr, hptr).first;
        if(tptr) tptr->_ostreams[k] = &(sitr->second);
        if(hptr) hptr->_istreams[k] = &(sitr->second);
      }
      // Case 2: topology needs to be modified (submit mode)
      else if(tpg->id == -1) {
        tpg->streams.try_emplace(k, k, t, h);
      }
    }
  ); 
  
  return StreamBuilder(this, k);
}

// Function: container
ContainerBuilder Graph::container() {
  
  auto k = _generate_key();

  _tasks.emplace_back(
    [k] (pb::Topology* tpg) {
      // We only initiate the resource container for submit mode
      if(tpg && tpg->id == -1) {
        tpg->containers.try_emplace(k, k);
      }
    }
  );

  return ContainerBuilder(this, k);
}

// Function: generate_key
// Generate a unique key on each call for graph components, i.e., vertex, stream, and container.
key_type Graph::_generate_key() const {
  static key_type k(0);
  return k++;
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

  // Store the file path for this topology.
  tpg.file = Policy::get().SUBMIT_FILE();

  // Parse the argv to this topology.
  const static std::regex ws_re("\\s+|\\n+|\\t+"); 
  auto argv = Policy::get().SUBMIT_ARGV();
	tpg.argv.insert(
		std::end(tpg.argv),
    std::sregex_token_iterator(argv.begin(), argv.end(), ws_re, -1),
    std::sregex_token_iterator()
  );

  // Store the environment variables for this topology.
	tpg.envp = environment_variables();

  // Initialize the topology from the graph.
  _make(&tpg);

  // --------------------
  // Post senity check  |
  // --------------------

  // Container must have resource assigned.
  for(auto& c : tpg.containers) {
    // CPU field.
    if(c.second.resource.num_cpus == 0) {
      c.second.resource.num_cpus = 1;
    }
    // Memory field
    if(c.second.resource.memory_limit_in_bytes == 0) {
      c.second.resource.memory_limit_in_bytes = 24_MB;
    }
    // Disk field
    if(c.second.resource.space_limit_in_bytes == 0) {
      c.second.resource.space_limit_in_bytes = 1_GB;
    }
  }

  // Create a default container for unsigned vertices.
  auto cnt = std::count_if(
    tpg.vertices.begin(), tpg.vertices.end(), 
    [] (const auto& v) { 
      return v.second.container == -1; 
    }
  );

  if(cnt > 0) {
    auto ckey = _generate_key();
    tpg.containers.try_emplace(ckey, ckey);
    for(auto& v : tpg.vertices) {
      if(v.second.container == -1) {
        v.second.container = ckey;
      }
    }
  }

  return tpg;

}

};  // End of namespace dtc::graph. ----------------------------------------------------









