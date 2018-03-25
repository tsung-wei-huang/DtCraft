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

#include <dtc/protobuf/topology.hpp>

namespace dtc::pb {

//-------------------------------------------------------------------------------------------------
    
// Constructor
Topology::Container::Container(key_type in_key) : key {in_key} {
} 

// Procedure: host
void Topology::Container::host(std::string h) {
  configs["host"] = std::move(h);
}

// Procedure: preferred_host
void Topology::Container::preferred_host(std::string h) {
  configs["preferred_hosts"] += (std::move(h) + ' ');
}

// Function: host
std::string Topology::Container::host() const {
  if(auto itr = configs.find("host"); itr == configs.end()) {
    return ""; 
  }
  else return itr->second;
}

// Function: preferred_host
std::vector<std::string> Topology::Container::preferred_hosts() const {

  std::vector<std::string> hosts;

  if(auto itr = configs.find("preferred_hosts"); itr != configs.end()) {
    
    const static std::regex ws_re("\\s+|\\n+|\\t+"); 
    
    auto beg = std::sregex_token_iterator(itr->second.begin(), itr->second.end(), ws_re, -1);
    auto end = std::sregex_token_iterator();
    
    while(beg != end) {
      hosts.push_back(beg->str());
      beg++;
    }
  }

  return hosts;
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
Topology::Topology(key_type in_graph, key_type in_topology) : 
  graph {in_graph}, topology {in_topology} {
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
    return has_vertex(itr->second.tail) && has_vertex(itr->second.head);
  }
  return false;
}

// Function: has_inter_stream
bool Topology::has_inter_stream(key_type key) const {
  return has_inter_stream(key, std::ios_base::in) || has_inter_stream(key, std::ios_base::out);
}

// Function: has_inter_stream
bool Topology::has_inter_stream(key_type key, std::ios_base::openmode m) const {

  if(auto itr = streams.find(key); itr != streams.end()) {
    switch(m) {
      case std::ios_base::in:
        return has_vertex(itr->second.head) && !has_vertex(itr->second.tail);
      break;

      case std::ios_base::out:
        return has_vertex(itr->second.tail) && !has_vertex(itr->second.head);
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
      return has_inter_stream(kvp.first); 
    }
  );
}

// Function: num_intra_streams
size_t Topology::num_intra_streams() const {
  return std::count_if(
    streams.begin(), streams.end(), [&] (const auto& kvp) { 
      return has_intra_stream(kvp.first); 
    }
  );
}

// Function: max_container_key
key_type Topology::max_container_key() const {
  key_type mkey = std::numeric_limits<key_type>::min();
  for(const auto& kvp : containers) {
    mkey = std::max(mkey, kvp.first);
  }
  return mkey;
}

// Function: min_container_key
key_type Topology::min_container_key() const {
  key_type mkey = std::numeric_limits<key_type>::max();
  for(const auto& kvp : containers) {
    mkey = std::min(mkey, kvp.first);
  }
  return mkey;
}

// Function: extract
// Extract a topology from container id 'topology'.
Topology Topology::extract(key_type topology) const {

  Topology tpg(graph, topology);
    
  tpg.runtime = runtime;
	
  // Copy the container.
  tpg.containers[topology] = containers.at(topology);

	// Copy those vertices belonging to this container.
	for(const auto& kvp : vertices) {
		if(kvp.second.container == topology) {
      tpg.vertices.emplace(kvp);
		}
	}
  
  // Copy streams.
  for(const auto& [skey, stream] : streams) {

    const auto tc = vertices.at(stream.tail).container;
    const auto hc = vertices.at(stream.head).container;

    if(tc == topology || hc == topology) {
      tpg.streams[skey] = stream;
    }
  }
  
  return tpg;
}

// Function: to_string
std::string Topology::to_string(size_t verb) const {
  
  std::ostringstream oss;

  oss << "@" << runtime.this_host() << " "
      << "[vertex:" << vertices.size() 
      << "|stream:" << streams.size()
      << "|container:" << containers.size() << "]";

  return oss.str();
}

// Function: ostream << 
std::ostream& operator<<(std::ostream& os , const Topology& rhs) {

  std::ostringstream oss;
  
  oss << "[Topology " << rhs.topology << "]\n";

  oss << "UUID = " << rhs.graph << "\n";

  oss << "# vertices = " << rhs.vertices.size() << "\n";
  for(const auto& v : rhs.vertices) {
    oss << "vertex " << v.second.key << " [container=" << v.second.container << "]\n";
  }

  oss << "# streams = " << rhs.streams.size() << "\n";
  
  oss << "# containers = " << rhs.containers.size() << "\n";
  for(const auto& c : rhs.containers) {
    oss << "container " << c.first << "\n";
  }

  os << oss.str();

  return os;
}

};  // End of namespace dtc::pb::topology. --------------------------------------------------


