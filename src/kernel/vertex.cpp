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

#include <dtc/kernel/vertex.hpp>
#include <dtc/kernel/executor.hpp>

namespace dtc {

//-------------------------------------------------------------------------------------------------
// Vertex
//-------------------------------------------------------------------------------------------------

// Constructor
Vertex::Vertex(key_type k) : key {k} {
}

// Function: _ostream
std::shared_ptr<OutputStream> Vertex::ostream(key_type k) const {
  auto itr = _ostreams.find(k);
  if(itr == _ostreams.end()) {
    DTC_THROW("Failed to get ostream ", k, " from vertex ", key, " (invalid key)");
  }
  return itr->second->ostream();
}

// Function: _istream
std::shared_ptr<InputStream> Vertex::istream(key_type k) const {
  auto itr = _istreams.find(k);
  if(itr == _istreams.end()) {
    DTC_THROW("Failed to get istream ", k, " from vertex ", key, " (invalid key)");
  }
  return itr->second->istream();
}

// Operator
Vertex& Vertex::operator()() {
  if(_on) {
    std::call_once(_once_flag, _on, *this);
  }
  return *this;
}

// Procedure: remove_istream
void Vertex::remove_istream(key_type ikey) const {
  if(auto itr = _istreams.find(ikey); itr == _istreams.end()) {
    DTC_THROW("Failed to remove ostream ", ikey, " from vertex ", key, " (invalid key)");
  }
  _executor->remove_istream(ikey);
}

// Procedure: remove_ostream
void Vertex::remove_ostream(key_type okey) const {
  if(auto itr = _ostreams.find(okey); itr == _ostreams.end()) {
    DTC_THROW("Failed to remove ostream ", okey, " from vertex ", key, " (invalid key)");
  }
  _executor->remove_ostream(okey);
}

// Function: program
bool Vertex::program() const {
  return !_runtime.program().empty();
}

// Procedure: _prespawn
Vertex::Program Vertex::_prespawn() {
  
  // Replace the argv/file and clear unnecessary fields
  auto cmd = _runtime.program();

  assert(!cmd.empty());

  // Assign the bridges [key|tag]:fd
  std::vector<std::shared_ptr<Device>> B;
  std::ostringstream oss;

  for(const auto& [k, s] : _istreams) {
    B.push_back(s->extract_ibridge());
    oss << (s->tag().empty() ? std::to_string(k) : s->tag()) << ":" << B.back()->fd() << ' ';
  }

  for(const auto& [k, s] : _ostreams) { 
    B.push_back(s->extract_obridge());
    oss << (s->tag().empty() ? std::to_string(k) : s->tag()) << ":" << B.back()->fd() << ' ';
  }

  // Update the runtime 
  _runtime.merge(environment_variables())
          .submit_file(cmd.substr(0, cmd.find_first_of(' ')))
          .submit_argv(cmd)
          .remove_vertex_hosts()
          .remove_frontiers()
          .remove_topology_fd()
          .bridges(oss.str());

  return Program{key, _runtime.c_file(), _runtime.c_argv(), _runtime.c_envp(), std::move(B)};
}

// Procedure: _extract_bridges

// ------------------------------------------------------------------------------------------------

// Constructor
Prober::Prober(key_type v, Vertex* ptr) : key {v}, _vertex {ptr} {
}

// Operator
Event::Signal Prober::operator()() const {
  if(_on) return _on((*_vertex)());
  else return Event::REMOVE;
}


};  // end of namespace dtc. ----------------------------------------------------------------------







