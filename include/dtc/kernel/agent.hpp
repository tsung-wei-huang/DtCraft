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

namespace dtc {

// Class: Timer
class Timer {
  
  public:

    Timer() = default;

    template <typename D>
    D elapsed_time() const;

    inline auto elapsed_time_in_seconds() const;
    inline auto elapsed_time_in_milliseconds() const;
    inline auto elapsed_time_in_microseconds() const;
    inline auto elapsed_time_in_nanoseconds() const;

  private:

    std::chrono::steady_clock::time_point _boot {std::chrono::steady_clock::now()};
};

// Function: elapsed_time
template <typename D>
D Timer::elapsed_time() const {
  return std::chrono::duration_cast<D>(std::chrono::steady_clock::now() - _boot);
}

// Function: elapsed_time_in_seconds
inline auto Timer::elapsed_time_in_seconds() const { 
  return elapsed_time<std::chrono::seconds>().count();
}

// Function: elapsed_time_in_milliseconds
inline auto Timer::elapsed_time_in_milliseconds() const {
  return elapsed_time<std::chrono::milliseconds>().count();
}

// Function: elapsed_time_in_microseconds
inline auto Timer::elapsed_time_in_microseconds() const {
  return elapsed_time<std::chrono::microseconds>().count();
}

// Function: elapsed_time_in_nanoseconds
inline auto Timer::elapsed_time_in_nanoseconds() const {
  return elapsed_time<std::chrono::nanoseconds>().count();
}

//-------------------------------------------------------------------------------------------------

// Class: Agent
class Agent : public KernelBase {

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
    Timer timer;
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

    Hatchery& hatchery();
    Executor& executor();
    const Hatchery& hatchery() const; 
    const Executor& executor() const; 
  };
  
  // Mater channel.
  struct Master {
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  }; 
  
  private:

    ControlGroup _cgroup;
    Master _master;

    pb::LoadInfo loadinfo;
    
    std::set<int> _cpuset;
    std::unordered_map<TaskID, Task> _tasks;

    std::list<Frontier> _frontiers;
    
    void _remove_task(Task&, bool);
    void _remove_task(const TaskID&, bool);
    void _insert_frontier(Frontier&);
    void _init_frontier_listener();
    void _init_cgroup();
    void _init_master();

    bool _deploy(Task&);
    bool _insert_task(Task&);

  public:
    
    Agent();
    ~Agent();

    std::future<bool> insert_task(pb::Topology&&);
    std::future<void> remove_task(const TaskID&, bool);
    std::future<void> insert_frontier(Frontier&&);
};
    
 
};  // End of namespace dtc. --------------------------------------------------------------



#endif



