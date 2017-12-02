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

#include <dtc/protobuf/topology.hpp>

namespace dtc::pb {

//-------------------------------------------------------------------------------------------------
    
// Constructor
Topology::Container::Container(key_type in_key) : key {in_key} {
} 

//-------------------------------------------------------------------------------------------------

// Constructor
Topology::Vertex::Vertex(key_type k) : key {k} {
}

//-------------------------------------------------------------------------------------------------

// Constructor
Topology::Stream::Stream(key_type k, key_type t, key_type h) : 
  key{k}, tail{t}, head{h} {
}

//-------------------------------------------------------------------------------------------------

// Constructor  
Topology::Topology(key_type in_graph, key_type in_id) : 
  graph {in_graph}, id {in_id} {
};

// Function: resource
Resource Topology::resource() const {

  Resource ret;
  
  // Sum up all the container resource. 
  for(const auto& kvp : containers) {
    ret += kvp.second.resource;
  }

  return ret;
}

// Function: match
bool Topology::match(const Frontier& ftr) const {
  return ftr.graph == graph && ftr.topology == id;
  //return ftr.graph == graph && has_inter_stream(ftr.stream, ftr.mode);
}

// Function: has_vertex
bool Topology::has_vertex(key_type key) const {
  return vertices.find(key) != vertices.end();
}

// Function: has_stream
bool Topology::has_stream(key_type key) const {
  return streams.find(key) != streams.end();
}

// Function: has_container
bool Topology::has_container(key_type key) const {
  return containers.find(key) != containers.end();
}

// Function: has_intra_stream
bool Topology::has_intra_stream(key_type key) const {
  if(auto itr = streams.find(key); itr != streams.end()) {
    return itr->second.head_topology == itr->second.tail_topology;
  }
  return false;
}

// Function: has_inter_stream
bool Topology::has_inter_stream(key_type key) const {
  if(auto itr = streams.find(key); itr != streams.end()) {
    return itr->second.head_topology != itr->second.tail_topology;
  }
  return false;
}

// Function: has_inter_stream
bool Topology::has_inter_stream(key_type key, std::ios_base::openmode m) const {

  if(auto itr = streams.find(key); itr != streams.end()) {

    if(itr->second.head_topology == itr->second.tail_topology) {
      return false;
    }

    switch(m) {
      case std::ios_base::in:
        return itr->second.head_topology == id;
      break;

      case std::ios_base::out:
        return itr->second.tail_topology == id;
      break;

      default:
        return false;
      break;
    }
  }
  
  return false;
}

// Function: num_inter_streams
size_t Topology::num_inter_streams() const {
  return std::count_if(
    streams.begin(), streams.end(), [&] (const auto& kvp) { 
      //return has_inter_stream(kvp.first); 
      return kvp.second.tail_topology != kvp.second.head_topology;
    }
  );
}

// Function: num_intra_streams
size_t Topology::num_intra_streams() const {
  return std::count_if(
    streams.begin(), streams.end(), [&] (const auto& kvp) { 
      //return has_intra_stream(kvp.first); 
      return kvp.second.tail_topology == kvp.second.head_topology;
    }
  );
}

//// Function: cgroup_mount
//std::filesystem::path Topology::cgroup_mount() const {
//  return Policy::get().CGROUP_MOUNT() / (std::to_string(graph) + '_' + std::to_string(id));
//}

// Function: to_string
std::string Topology::to_string(size_t verb) const {
  
  std::ostringstream oss;

  oss << "@" << envp.at("DTC_THIS_HOST") << " "
      << "[vertex:" << vertices.size() 
      << "|stream:" << streams.size()
      << "|container:" << containers.size() << "]";

  return oss.str();
}

// Function: ostream << 
std::ostream& operator<<(std::ostream& os , const Topology& rhs) {

  std::ostringstream oss;
  
  oss << "[Topology " << rhs.id << "]\n";

  oss << "UUID = " << rhs.graph << "\n";
  oss << "file = " << rhs.file << "\n";

  oss << "argv =";
  for(const auto& s : rhs.argv) {
    oss << " " << s;
  }
  oss << "\n";

  //oss << "envp:\n";
  //for(const auto& kvp : rhs.envp) {
  //  oss << kvp.first << "=" << kvp.second << "\n";
  //}

  oss << "# vertices = " << rhs.vertices.size() << "\n";
  for(const auto& v : rhs.vertices) {
    oss << "vertex " << v.second.key << " [container=" << v.second.container << "]\n";
  }

  oss << "# streams = " << rhs.streams.size() << "\n";
  for(const auto& e : rhs.streams) {
    oss << "stream " << e.second.key << " : " 
        << e.second.tail << "@" << e.second.tail_host << " -> " 
        << e.second.head << "@" << e.second.head_host << "\n";
  }
  
  oss << "# containers = " << rhs.containers.size() << "\n";
  for(const auto& c : rhs.containers) {
    oss << "container " << c.first << "\n";
  }

  os << oss.str();

  return os;
}

};  // End of namespace dtc::pb::topology. --------------------------------------------------


