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

#ifndef DTC_KERNEL_VERTEX_HPP_
#define DTC_KERNEL_VERTEX_HPP_

#include <dtc/ipc/ipc.hpp>
#include <dtc/kernel/stream.hpp>

namespace dtc {

// Forward declaration.
class Executor;

// Class: Vertex
class Vertex {

  friend class Stream;
  friend class Prober;
  friend class Graph;
  friend class Executor;
  friend class VertexBuilder;

  struct Program {
    const key_type vertex;
    std::unique_ptr<char[]> c_file;
    std::unique_ptr<char*, std::function<void(char**)>> c_argv;
    std::unique_ptr<char*, std::function<void(char**)>> c_envp;  
    std::vector<std::shared_ptr<Device>> bridges;
  };

  public:

    const key_type key {-1};

    std::any any;

    Vertex(key_type);
    
    template <typename... T>
    void broadcast(T&&...) const;

    template <typename K, typename... T>
    void broadcast_to(K&&, T&&...) const;

    void remove_ostream(key_type) const;
    void remove_istream(key_type) const;

    std::shared_ptr<InputStream> istream(key_type) const;
    std::shared_ptr<OutputStream> ostream(key_type) const;

    inline const std::string& tag() const;

    bool program() const;

  private:

    Executor* _executor {nullptr};

    std::string _tag;
    
    std::once_flag _once_flag;

    std::function<void(Vertex&)> _on;
    
    std::unordered_map<key_type, Stream*> _istreams;
    std::unordered_map<key_type, Stream*> _ostreams;

    Runtime _runtime;

    Vertex& operator()();

    Program _prespawn();
};

// Procedure: broadcast
template <typename... T>
void Vertex::broadcast(T&&... ts) const {
  for(const auto& kvp : _ostreams) {
    if(auto os = kvp.second->ostream(); os) {
      (*os)(ts...); 
    }
  }
}

// Procedure: broadcast_to
template <typename K, typename... T>
void Vertex::broadcast_to(K&& keys, T&&... ts) const {
  for(const auto& k : keys) {
    if(auto os = ostream(k); os) {
      (*os)(ts...); 
    }
  }
}

// Function: tag
inline const std::string& Vertex::tag() const {
  return _tag;
}

// ------------------------------------------------------------------------------------------------

// class: Prober
class Prober {
  
  friend class Graph;
  friend class Executor;
  friend class ProberBuilder;

  public:

    Prober(key_type, Vertex*);
    
    const key_type key {-1};

    inline const std::string& tag() const;

  private:

    std::string _tag;

    std::chrono::steady_clock::time_point::duration _duration;

    std::weak_ptr<PeriodicEvent> _event;

    std::function<Event::Signal(Vertex&)> _on;

    Vertex* _vertex {nullptr};
    
    Event::Signal operator()() const;
};

// Function: tag
inline const std::string& Prober::tag() const {
  return _tag;
}




};  // end of namespace dtc. ----------------------------------------------------------------------


#endif





