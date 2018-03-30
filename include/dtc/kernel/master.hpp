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

#ifndef DTC_KERNEL_MASTER_HPP_
#define DTC_KERNEL_MASTER_HPP_

#include <dtc/kernel/manager.hpp>

namespace dtc {

// Class: Master
class Master : public KernelBase {
  
  // Webui-specific structures.

  struct AgentInfo {
    const key_type key;
    pb::Resource resource;
    pb::Resource released;
    size_t num_tasks;
    json to_json() const;
  };

  struct MasterInfo {
    std::string host;
    size_t num_agents;
    size_t num_graphs; 
    json to_json() const;
  };

  struct ClusterInfo {
    MasterInfo master;
    std::vector<AgentInfo> agents;
    json to_json() const;
  };


  // ---- Channel definitions.

  // Agent channel.
  struct Agent {
    
    const key_type key;

    std::optional<pb::Resource> resource; 
    std::optional<pb::Resource> released;
    std::optional<pb::LoadInfo> loadinfo;

    struct CpuBin {
      int idx;
      std::unordered_set<TaskID> tasks;
    };

    struct Task {
      pb::Topology topology;
      std::unordered_set<int> cpu_bins;
    };

    std::vector<CpuBin> cpu_bins;
    std::unordered_map<TaskID, Task> tasks;

    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;

    Agent(key_type k) : key {k} {}

    void kill(const TaskID&);
    void remove(const TaskID&);
  };

  // Graph channel.
  struct Graph {
    
    const key_type key;

    float weight;

    std::optional<pb::Topology> topology;  
    std::optional<pb::Solution> solution;

    std::unordered_map<TaskID, key_type> placement;

    //struct TaskMeta {
    //  const key_type agent;
    //  TaskMeta(key_type k) : agent {k} {}
    //};

    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;

    Graph(key_type k) : key {k} {}

    void update(const pb::TaskInfo&);
  };

  // Webui channel.
  struct WebUI {

    const key_type key;

    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;

    WebUI(key_type k) : key {k} {}
  };


    bool _enqueue(Graph&);

    size_t _dequeue();
    
    void _remove_agent(key_type);
    void _remove_graph(key_type);
    void _remove_webui(key_type);
    void _on_resource(key_type, pb::Resource&);
    void _on_topology(key_type, pb::Topology&);
    void _on_taskinfo(key_type, pb::TaskInfo&);

    key_type _insert_graph(std::shared_ptr<Socket>);
    key_type _insert_agent(std::shared_ptr<Socket>);
    key_type _insert_webui(std::shared_ptr<Socket>);

    ClusterInfo _cluster_info() const;

    bool _try_enqueue(Graph&);
    bool _try_dequeue(Graph&);

  public:

    Master();
    ~Master();
    
    std::future<void> on_resource(key_type, pb::Resource&&);
    std::future<void> on_topology(key_type, pb::Topology&&);
    std::future<void> on_taskinfo(key_type, pb::TaskInfo&&);
    std::future<void> remove_agent(key_type);
    std::future<void> remove_graph(key_type);
    std::future<void> remove_webui(key_type);

    std::future<key_type> insert_agent(std::shared_ptr<Socket>);
    std::future<key_type> insert_graph(std::shared_ptr<Socket>);
    std::future<key_type> insert_webui(std::shared_ptr<Socket>);

    HttpResponse make_response(const HttpRequest&);
    std::string agent_to_json(std::string_view);

    size_t num_graphs() const;
    size_t num_agents() const;
    size_t num_webuis() const;

    std::future<ClusterInfo> cluster_info();

  private:
    
    std::unordered_map<key_type, Agent> _agents;
    std::unordered_map<key_type, Graph> _graphs;
    std::unordered_map<key_type, WebUI> _webuis;

    std::queue<key_type> _queue;
};


};  // End of namespace dtc. --------------------------------------------------------------

#endif




