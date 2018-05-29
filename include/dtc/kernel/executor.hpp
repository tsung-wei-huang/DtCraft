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

#ifndef DTC_KERNEL_EXECUTOR_HPP_
#define DTC_KERNEL_EXECUTOR_HPP_

#include <dtc/kernel/manager.hpp>

namespace dtc {

class Graph;

class Executor : public KernelBase {

  struct Master {
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  };

  struct Agent {
    int stderr {STDERR_FILENO};    
    int stdout {STDOUT_FILENO};
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  }; 

  private:

    std::shared_ptr<ReadEvent> _stdout_listener;
    std::shared_ptr<ReadEvent> _stderr_listener;
    
    std::optional<Master> _master;
    std::optional<Agent> _agent;

    Graph& _graph;

    void _teardown_distributed();
    void _teardown_submit();
    void _teardown_local();
    void _setup_local();
    void _setup_submit();
    void _setup_distributed();
    void _make_graph(pb::Topology*);
    void _insert_vertices(pb::Topology*);
    void _insert_streams(pb::Topology*);
    void _insert_istream(Stream&, std::shared_ptr<Device>);
    void _insert_ostream(Stream&, std::shared_ptr<Device>);
    void _remove_ostream(key_type);
    void _remove_istream(key_type);
    void _spawn(Vertex::Program&);

  public:
    
    const ExecutionMode mode {env::execution_mode()};

    Executor(Graph&);
    ~Executor();

    std::future<void> remove_ostream(key_type);
    std::future<void> remove_istream(key_type);
    std::future<void> make_graph(pb::Topology&&);

    size_t num_streams() const;
    size_t num_vertices() const;
    size_t graph_size() const;

    void run();
};



};  // End of namespace dtc::executor. ----------------------------------------------------


#endif




