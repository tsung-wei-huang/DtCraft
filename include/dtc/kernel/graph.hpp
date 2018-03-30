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

#ifndef DTC_KERNEL_GRAPH_HPP_
#define DTC_KERNEL_GRAPH_HPP_

#include <dtc/kernel/vertex.hpp>
#include <dtc/kernel/stream.hpp>
#include <dtc/protobuf/topology.hpp>

namespace dtc {

// Forward declaration.
class ContainerBuilder;
class VertexBuilder;
class StreamBuilder;
class ProberBuilder;
class Graph;

// ------------------------------------------------------------------------------------------------

// Class: Graph
// The main class that definis the API for user to initiate a graph.
class Graph {

  friend class Executor;
  friend class ContainerBuilder;
  friend class VertexBuilder;
  friend class StreamBuilder;
  friend class ProberBuilder;

  private:

    TaskID _task_id;

    std::unordered_map<key_type, Vertex> _vertices;
    std::unordered_map<key_type, Stream> _streams;
    std::unordered_map<key_type, Prober> _probers;
    
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
    StreamBuilder stream(PlaceHolder&, key_type);
    StreamBuilder stream(key_type, PlaceHolder&);
    ContainerBuilder container();
    ProberBuilder prober(key_type);

    template <template<typename...> class C, typename... ArgsT>
    auto insert(ArgsT&&...);

  private:
    
    void _emplace_stream(key_type, key_type, key_type);
    void _make(pb::Topology*);

    Vertex* _vertex(key_type);
    Stream* _stream(key_type);
    Prober* _prober(key_type);

    key_type _generate_key() const;

    pb::Topology _topologize();
};

template <template<typename...> class C, typename... ArgsT>
auto Graph::insert(ArgsT&&... args) {
  return C(this, std::forward<ArgsT>(args)...);
}

//-------------------------------------------------------------------------------------------------

// Class: VertexBuilder
class VertexBuilder {

  friend class Graph;

  private:

    Graph* const _graph {nullptr};

  public:

    VertexBuilder(Graph*, key_type);
    VertexBuilder(const VertexBuilder&);
    
    const key_type key {-1};

    inline operator key_type() const;
    
    template <typename C>
    VertexBuilder& on(C&&);

    VertexBuilder& tag(std::string);
    VertexBuilder& program(std::string);
};

// Function: on
template <typename C>
VertexBuilder& VertexBuilder::on(C&& c) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, c=std::forward<C>(c)] (pb::Topology* tpg) mutable {
      // Case 1: vertex needs to be initialized (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        G->_vertices.at(key)._on = std::move(c);
      }
      // Case 2: no need to handle submit mode.
    }
  );
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

    Graph* const _graph {nullptr};

  public:

    StreamBuilder(Graph*, key_type, std::optional<key_type>, std::optional<key_type>);
    StreamBuilder(const StreamBuilder&);

    const key_type key;
    const std::optional<key_type> tail;
    const std::optional<key_type> head;

    inline operator key_type() const;

    template <typename C>
    StreamBuilder& on(C&&);

    StreamBuilder& critical(bool);
    StreamBuilder& tag(std::string);
};

// Function: on
template <typename C>
StreamBuilder& StreamBuilder::on(C&& c) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=key, c=std::forward<C>(c)] (pb::Topology* tpg) mutable {
      // Case 1: stream needs to be initialized (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_stream(key))) {
        if constexpr(std::is_invocable_v<C, Vertex&, InputStream&>) {
          G->_streams.at(key)._on_istream = std::move(c);
        }
        else if constexpr(std::is_invocable_v<C, Vertex&, OutputStream&>) {
          G->_streams.at(key)._on_ostream = std::move(c);
        }
        else static_assert(dependent_false_v<C>, "Unsupported stream callback");
      }
      // Case 2: no need to handle the submit mode.
    }
  );
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
    
    Graph* const _graph {nullptr};

  public:
    
    ContainerBuilder(Graph*, key_type);
    ContainerBuilder(const ContainerBuilder&);

    const key_type key;

    inline operator key_type() const;

    ContainerBuilder& add(key_type);
    ContainerBuilder& cpu(uintmax_t);
    ContainerBuilder& memory(uintmax_t);
    ContainerBuilder& memory_limit_in_bytes(uintmax_t);
    ContainerBuilder& space(uintmax_t);
    ContainerBuilder& hosts(auto&&... ts);
    ContainerBuilder& preferred_hosts(auto&&... ts);
};

// Implicit coversion.
inline ContainerBuilder::operator key_type() const {
  return key;
}

// Function: hosts
ContainerBuilder& ContainerBuilder::hosts(auto&&... ts) {
  _graph->_tasks.emplace_back(
    [ts..., c=key] (pb::Topology* tpg) mutable {
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).hosts(std::move(ts)...);
      }
    }
  );
  return *this;
}


// Function: preferred_hosts
ContainerBuilder& ContainerBuilder::preferred_hosts(auto&&... ts) {
  _graph->_tasks.emplace_back(
    [ts..., c=key] (pb::Topology* tpg) mutable {
      if(tpg && tpg->topology == -1) {
        tpg->containers.at(c).preferred_hosts(std::move(ts)...);
      }
    }
  );
  return *this;
}

//-------------------------------------------------------------------------------------------------

// Class: ProberBuilder
class ProberBuilder {

  friend class Graph;

  private:

    Graph* const _graph;

  public:

    ProberBuilder(Graph*, key_type);
    ProberBuilder(const ProberBuilder&);

    const key_type vertex {-1};

    template <typename C>
    ProberBuilder& on(C&&);

    template <typename D>
    ProberBuilder& duration(D&&);

    ProberBuilder& tag(std::string);
};

// Function: on
template <typename C>
ProberBuilder& ProberBuilder::on(C&& c) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=vertex, c=std::forward<C>(c)] (pb::Topology* tpg) mutable {
      // Case 1: vertex needs to be initialized (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        assert(G->_probers.find(key) != G->_probers.end());
        G->_probers.at(key)._on = std::move(c);
      }
      // Case 2: no need to handle submit mode.
    }
  );
  return *this;
}

// Function: duration
template <typename D>
ProberBuilder& ProberBuilder::duration(D&& d) {
  _graph->_tasks.emplace_back(
    [G=_graph, key=vertex, d=std::forward<D>(d)] (pb::Topology* tpg) mutable {
      // Case 1: vertex needs to be initialized (local/distributed mode)
      if(tpg == nullptr || (tpg->topology != -1 && tpg->has_vertex(key))) {
        assert(G->_probers.find(key) != G->_probers.end());
        G->_probers.at(key)._duration = std::move(d);
      }
      // Case 2: no need to handle submit mode.
    }
  ); 
  return *this;
}

};  // End of namespace dtc::graph. -------------------------------------------------------


#endif






