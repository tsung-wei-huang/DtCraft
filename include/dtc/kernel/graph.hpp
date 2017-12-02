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

#ifndef DTC_KERNEL_GRAPH_HPP_
#define DTC_KERNEL_GRAPH_HPP_

#include <dtc/ipc/ipc.hpp>
#include <dtc/protobuf/topology.hpp>
#include <dtc/kernel/container.hpp>

namespace dtc {

// Forward declaration.
class Vertex;
class Stream;
class ContainerBuilder;
class VertexBuilder;
class StreamBuilder;
class Graph;

//-------------------------------------------------------------------------------------------------

// Class: Stream
class Stream final {

  friend class Vertex;
  friend class Graph;
  friend class Executor;
  friend class StreamBuilder;

  template <typename T> friend class Adjacency;

  public:

  enum Signal {
    CLOSE,
    DEFAULT,
  };

    const key_type key {-1};
    
    Stream(key_type, Vertex*, Vertex*);

    bool is_inter_stream(std::ios_base::openmode);

  private:
    
    std::weak_ptr<OutputStream> _ostream;
    std::weak_ptr<InputStream> _istream;
    
    Vertex* _tail {nullptr};
    Vertex* _head {nullptr};

    std::function<Signal(Vertex&, OutputStream&)> _on_ostream {[](auto&&, auto&&){return DEFAULT;}};
    std::function<Signal(Vertex&, InputStream&)>  _on_istream {[](auto&&, auto&&){return DEFAULT;}};

    Signal operator () (Vertex&, InputStream&);
    Signal operator () (Vertex&, OutputStream&);
};

//-------------------------------------------------------------------------------------------------
  
// Class: Vertex
class Vertex {
  
  friend class Stream;
  friend class Graph;
  friend class Executor;
  friend class VertexBuilder;

  public:
    
    const key_type key {-1};

    std::any any;

    Vertex(key_type);
    
    inline auto ostream(key_type) const;

  private:

    std::once_flag _once_flag;

    std::function<void(Vertex&)> _on {[](auto&&){}};
    
    std::unordered_map<key_type, Stream*> _istreams;
    std::unordered_map<key_type, Stream*> _ostreams;

    Vertex& operator()();
};
    
auto Vertex::ostream(key_type key) const {
  return [key, ptr=_ostreams.at(key)->_ostream.lock()] (auto&&... args) {
    if(ptr == nullptr) {
      THROW("ostream ", key, " is closed");
    }
    return (*ptr)(std::forward<decltype(args)>(args)...);
  };
}


//-------------------------------------------------------------------------------------------------

// Class: Graph
// The main class that definis the API for user to initiate a graph.
class Graph {

  friend class Executor;
  friend class ContainerBuilder;
  friend class VertexBuilder;
  friend class StreamBuilder;

  private:

    TaskID _task_id;

    std::unordered_map<key_type, Vertex> _vertices;
    std::unordered_map<key_type, Stream> _streams;
    
    std::deque<std::function<void(pb::Topology*)>> _tasks;

  public:

    Graph() = default;
    Graph(Graph&&) = default;
    Graph(const Graph&) = delete;
    ~Graph() = default;

    Graph& operator = (Graph&&) = default;
    Graph& operator = (const Graph&) = delete;
    
    VertexBuilder vertex();
    StreamBuilder stream(key_type, key_type);
    ContainerBuilder container();

  private:
    
    void _make(pb::Topology*);

    template <typename C>
    void _on(key_type, C&&);
    
    key_type _generate_key() const;

    pb::Topology _topologize();
};

// Procedure: _on
// Assign the callback to a stream or a vertex.
template <typename C>
void Graph::_on(key_type key, C&& c) {
  _tasks.emplace_back(
    [G=this, key=key, c=std::forward<C>(c)] (pb::Topology* tpg) mutable {
      if(!tpg || (tpg->id != -1 && (tpg->has_stream(key) || tpg->has_vertex(key)))) {
        if constexpr(std::is_invocable_v<C, Vertex&>) {
          G->_vertices.at(key)._on = std::move(c);
        }
        else if constexpr(std::is_invocable_v<C, Vertex&, OutputStream&>){
          G->_streams.at(key)._on_ostream = std::move(c);
        }
        else if constexpr(std::is_invocable_v<C, Vertex&, InputStream&>){
          G->_streams.at(key)._on_istream = std::move(c);
        }
        else {
          static_assert(dependent_false<C>::value);
        }
      }
    }
  );
}

//-------------------------------------------------------------------------------------------------

// Class: VertexBuilder
class VertexBuilder {

  friend class Graph;

  private:

    Graph* _graph {nullptr};

  public:

    VertexBuilder(Graph*, key_type);
    
    const key_type key {-1};

    inline operator key_type() const;
    
    template <typename C>
    VertexBuilder& on(C&&);
};

// Function: on
template <typename C>
VertexBuilder& VertexBuilder::on(C&& c) {
  _graph->_on(key, std::forward<C>(c));
  return *this;
}

// Implicit coversion.
inline VertexBuilder::operator key_type() const {
  return key;
}

//-------------------------------------------------------------------------------------------------

// Class: StreamBuilder
class StreamBuilder {
  
  friend class Graph;

  private:

    Graph* _graph {nullptr};

  public:

    StreamBuilder(Graph*, key_type);

    const key_type key;

    inline operator key_type() const;

    template <typename C>
    StreamBuilder& on(C&&);
};

// Function: on
template <typename C>
StreamBuilder& StreamBuilder::on(C&& c) {
  _graph->_on(key, std::forward<C>(c));
  return *this;
}

// Implicit coversion.
inline StreamBuilder::operator key_type() const {
  return key;
}

//-------------------------------------------------------------------------------------------------

// Class: ContainerBuilder
class ContainerBuilder {

  friend class Graph;

  private:
    
    Graph* _graph {nullptr};

  public:
    
    ContainerBuilder(Graph*, key_type);

    const key_type key;

    inline operator key_type() const;

    ContainerBuilder& add(key_type);
    ContainerBuilder& num_cpus(uintmax_t);
    ContainerBuilder& memory_limit_in_bytes(uintmax_t);
    ContainerBuilder& rootfs(const std::filesystem::path&);
};

// Implicit coversion.
inline ContainerBuilder::operator key_type() const {
  return key;
}

};  // End of namespace dtc::graph. -------------------------------------------------------


#endif



