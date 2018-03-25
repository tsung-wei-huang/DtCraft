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

#ifndef DTC_PROTOBUF_TOPOLOGY_HPP_
#define DTC_PROTOBUF_TOPOLOGY_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/common.hpp>
#include <dtc/protobuf/resource.hpp>

namespace dtc::pb {

// Forward declaration.
struct Resource;

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

    void host(std::string);
    void preferred_host(std::string);
    void preferred_hosts(auto&&... hs);

    std::string host() const;
    std::vector<std::string> preferred_hosts() const;
    
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
        head
      );
    }
  };
  
  //-----------------------------------------------------------------------------------------------
  
  key_type graph {-1};
  key_type topology {-1};

  Runtime runtime;
  std::unordered_map <key_type, Vertex> vertices;
  std::unordered_map <key_type, Stream> streams;
  std::unordered_map <key_type, Container> containers;

  Topology() = default;
  Topology(key_type, key_type);
  Topology(const Topology&) = default;
  Topology(Topology&&) = default;

  Topology& operator = (const Topology&) = default;
  Topology& operator = (Topology&&) = default;

  Resource resource() const;
  Topology extract(key_type) const;

  inline auto task_id() const;

  bool has_vertex(key_type) const;
  bool has_stream(key_type) const;
  bool has_container(key_type) const;
  bool has_intra_stream(key_type) const;
  bool has_inter_stream(key_type) const;
  bool has_inter_stream(key_type, std::ios_base::openmode) const;

  size_t num_inter_streams() const;
  size_t num_intra_streams() const;

  key_type max_container_key() const;
  key_type min_container_key() const;

  std::string to_string(size_t = 0) const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(
      graph,
      topology,
      runtime,
      vertices,
      streams,
      containers
    );
  }
};

// Function: task_id
inline auto Topology::task_id() const {
  return TaskID{graph, topology};
}

// Outputstream
std::ostream& operator<<(std::ostream&, const Topology&);

// -----------------------------------------------------------------------------------------

// Procedure: preferred_hosts
void Topology::Container::preferred_hosts(auto&&... hosts) {
  (preferred_host(std::move(hosts)), ...);
}

};  // End of namespace dtc::pb. -----------------------------------------------------------


#endif


