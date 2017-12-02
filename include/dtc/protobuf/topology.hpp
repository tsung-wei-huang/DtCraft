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

#ifndef DTC_PROTOBUF_TOPOLOGY_HPP_
#define DTC_PROTOBUF_TOPOLOGY_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/common.hpp>
#include <dtc/protobuf/frontier.hpp>
#include <dtc/protobuf/resource.hpp>

namespace dtc::pb {

// Forward declaration.
struct Resource;
struct Frontier;

//-------------------------------------------------------------------------------------------------

// Struct: Topology
struct Topology {

  //-----------------------------------------------------------------------------------------------
  struct Container {

    key_type key {-1}; 

    Resource resource;

    std::unordered_map<std::string, std::string> configs;
    
    Container() = default;
    Container(const Container&) = default;
    Container(Container&&) = default;
    Container(key_type);

    Container& operator = (Container&&) = default;
    Container& operator = (const Container&) = default;
    
    template <typename ArchiverT>
    std::streamsize archive(ArchiverT& ar) {
      return ar(key, resource, configs);
    }
  };

  //-----------------------------------------------------------------------------------------------

  // Struct: Vertex
  struct Vertex {

    key_type key {-1};
    key_type container {-1};
      
    Vertex(key_type);

    Vertex() = default;
    Vertex(const Vertex&) = default;
    Vertex(Vertex&&) = default;

    Vertex& operator = (Vertex&&) = default;
    Vertex& operator = (const Vertex&) = default;

    template <typename ArchiverT>
    std::streamsize archive(ArchiverT& ar) {
      return ar(key, container);
    }
  };

  //-----------------------------------------------------------------------------------------------

  // Struct: Stream
  //
  // tail (ostream) ----------> head (istream)
  //
  struct Stream {
    
    key_type key {-1};
    key_type tail {-1};
    key_type head {-1};
    key_type tail_topology {-1};
    key_type head_topology {-1};
    
    std::string tail_host {"127.0.0.1"};
    std::string head_host {"127.0.0.1"};

    Stream(key_type, key_type, key_type);

    Stream() = default;
    Stream(Stream&&) = default;
    Stream(const Stream&) = default;

    Stream& operator = (Stream&&) = default;
    Stream& operator = (const Stream&) = default;
    
    template <typename ArchiverT>
    std::streamsize archive(ArchiverT& ar) {
      return ar(
        key,
        tail,
        head,
        tail_topology,
        head_topology,
        tail_host,
        head_host
      );
    }
  };
  
  //-----------------------------------------------------------------------------------------------
  
  key_type graph {-1};
  key_type id {-1};

  std::string file;
  std::vector <std::string> argv;
  std::unordered_map <std::string, std::string> envp;
  std::unordered_map <key_type, Vertex> vertices;
  std::unordered_map <key_type, Stream> streams;
  std::unordered_map <key_type, Container> containers;
  
  Topology() = default;
  Topology(key_type, key_type);
  Topology(const Topology&) = default;
  Topology(Topology&&) = default;

  Topology& operator = (const Topology&) = default;
  Topology& operator = (Topology&&) = default;

  inline auto c_file() const;
  inline auto c_argv() const;
  inline auto c_envp() const;  

  Resource resource() const;

  inline auto task_id() const;

  bool has_vertex(key_type) const;
  bool has_stream(key_type) const;
  bool has_container(key_type) const;
  bool has_intra_stream(key_type) const;
  bool has_inter_stream(key_type) const;
  bool has_inter_stream(key_type, std::ios_base::openmode) const;
  bool match(const Frontier&) const;

  size_t num_inter_streams() const;
  size_t num_intra_streams() const;

  std::string to_string(size_t = 0) const;

  std::filesystem::path cgroup_mount() const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(
      graph,
      id,
      file,
      argv,
      envp,
      vertices,
      streams,
      containers
    );
  }
};

// Function: c_file
inline auto Topology::c_file() const {
  return file.c_str();
}

// Function: c_argv
inline auto Topology::c_argv() const { 

  auto ptr = std::make_unique<char*[]>(argv.size() + 1);
  for(size_t i=0; i<argv.size(); ++i) {
    ptr[i] = const_cast<char*>(argv[i].c_str());
  }
  ptr[argv.size()] = nullptr;

  return ptr;
}

// Function: c_envp
inline auto Topology::c_envp() const {

  std::unique_ptr<char*, std::function<void(char**)>> ptr(
    new char*[envp.size() + 1],
    [sz=envp.size()+1](char** ptr) {
      for(size_t i=0; i<sz; ++i) {
        delete [] ptr[i];
      }
      delete [] ptr;
    }
  );
  
  auto idx = size_t{0};

  for(const auto& kvp : envp) {
    auto entry = kvp.first + "=" + kvp.second;
    ptr.get()[idx] = new char[entry.size() + 1];
    ::strncpy(ptr.get()[idx], entry.c_str(), entry.size() + 1);
    ++idx;
  }
  ptr.get()[idx] = nullptr;

  return ptr;
}

// Function: task_id
inline auto Topology::task_id() const {
  return TaskID{graph, id};
}

// Outputstream
std::ostream& operator<<(std::ostream&, const Topology&);


};  // End of namespace dtc::pb::Topology. --------------------------------------------------


#endif


