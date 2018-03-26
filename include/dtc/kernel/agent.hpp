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

#ifndef DTC_KERNEL_AGENT_HPP_
#define DTC_KERNEL_AGENT_HPP_

#include <dtc/kernel/manager.hpp>
#include <dtc/kernel/container.hpp>

namespace dtc {

// Placer
class Placer {

  friend class Agent;

  // Bucket
  struct Bucket {
    unsigned cpu;
    std::unordered_set<TaskID> tasks;
  };

  public:

    Placer() = default;
    Placer(const Placer&) = delete;
    Placer(Placer&&) = default;

    Placer& operator = (const Placer&) = delete;
    Placer& operator = (Placer&&) = default;

    inline size_t num_buckets() const;

  private:

    std::vector<Bucket> _buckets;
};

// Function: num_buckets
inline size_t Placer::num_buckets() const {
  return _buckets.size();
}

// ------------------------------------------------------------------------------------------------

// Class: Agent
class Agent : public KernelBase {

  using CGroup = cg::ControlGroup;
  
  // ---- Internal data structure ---------------

  struct FrontierPacket {
    key_type graph;
    key_type stream;
  };

  struct Frontier {
    key_type graph;
    key_type stream;
    std::shared_ptr<Socket> socket;
  };
    
  struct Hatchery {
    size_t num_inter_streams;
    std::list<Frontier> frontiers;
    std::shared_ptr<Socket> stdout;
    std::shared_ptr<Socket> stderr;
  };
  
  struct Executor {
    Executor(const std::filesystem::path&);
    Container container;
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  };
  
  struct Task {
    TaskID key;
    pb::Topology topology;
    std::chrono::steady_clock::time_point boot {std::chrono::steady_clock::now()};
    std::variant<Hatchery, Executor> handle;

    Task(pb::Topology&&);
    Task(Task&& rhs) = default;
    Task(const Task&) = delete;
    Task& operator = (Task&&) = default;
    Task& operator = (const Task&) = delete;

    bool ready() const;
    bool match(const Frontier&) const;

    std::string frontiers_to_string() const;
    
    void splice_frontiers(std::list<Frontier>&);

    template <typename D>
    D elapsed_time();

    inline auto& hatchery() { return std::get<Hatchery>(handle); }
    inline auto& executor() { return std::get<Executor>(handle); }
    inline const auto& hatchery() const { return std::get<Hatchery>(handle); }
    inline const auto& executor() const { return std::get<Executor>(handle); }
  };
  
  // Mater channel.
  struct Master {
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  }; 

  private:

    CGroup _cgroup;
    Placer _placer;
    Master _master;

    std::unordered_map<TaskID, Task> _tasks;

    std::list<Frontier> _frontiers;
    
    void _remove_task(Task&, bool);
    void _remove_task(const TaskID&, bool);
    void _insert_frontier(Frontier&);
    void _make_frontier_listener();

    bool _deploy(Task&);
    bool _insert_task(Task&);
    
    CGroup _make_cgroup();
    Placer _make_placer();
    Master _make_master();

  public:
    
    Agent();
    ~Agent();

    std::future<bool> insert_task(pb::Topology&&);
    std::future<void> remove_task(const TaskID&, bool);
    std::future<void> insert_frontier(Frontier&&);
};
    
 
// Function: elapsed_time
template <typename D>
D Agent::Task::elapsed_time() {
  return std::chrono::duration_cast<D>(std::chrono::steady_clock::now() - boot);
}

};  // End of namespace dtc. --------------------------------------------------------------



#endif



