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

#ifndef DTC_KERNEL_EXECUTOR_HPP_
#define DTC_KERNEL_EXECUTOR_HPP_

#include <dtc/kernel/manager.hpp>
#include <dtc/kernel/graph.hpp>

namespace dtc {

class Executor : public KernelBase {
  
  struct Master : ActorBase {};

  struct Agent : ActorBase {
    int stderr {STDERR_FILENO};    
    int stdout {STDOUT_FILENO};
  }; 

  private:
    
    std::optional<Master> _master;
    std::optional<Agent> _agent;

    Graph& _graph;

    void _teardown_distributed();

    void _setup_local();
    void _setup_submit();
    void _setup_distributed();
    void _make_vertices();
    void _make_streams();
    void _make_graph(pb::Topology*);
    void _remove_stream(key_type, std::error_code&);

    size_t _num_task_events() const;
    size_t _num_streams() const;
    size_t _num_vertices() const;
    size_t _graph_size() const;

  public:

    Executor(Graph&);
    ~Executor();

    std::future<void> remove_stream(key_type, std::error_code);
    std::future<void> make_graph(pb::Topology&&);

    std::future<size_t> num_vertices();
    std::future<size_t> num_streams();
    std::future<size_t> graph_size();

    void run();
};

};  // End of namespace dtc::executor. ----------------------------------------------------


#endif

