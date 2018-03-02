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
  
  struct Task {
    
    pb::Topology topology;

    struct Hatchery {
      size_t num_inter_streams;
      std::list<Frontier> frontiers;
      std::shared_ptr<Socket> stdout;
      std::shared_ptr<Socket> stderr;
    };
    
    struct Executor {
      Container container;
      std::shared_ptr<InputStream> istream;
      std::shared_ptr<OutputStream> ostream;
    };

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

    std::optional<Master> _master;

    std::unordered_map<TaskID, Task> _tasks;

    std::list<Frontier> _frontiers;
    
    bool _remove_task(const TaskID&, bool);
    bool _deploy(Task&);
    bool _insert_task(Task&);

    void _make_master();
    void _insert_frontier(Frontier&);
    void _make_frontier_listener();

  public:

    Agent();
    ~Agent() = default;
    
    std::future<bool> insert_task(pb::Topology&&);
    std::future<bool> remove_task(const TaskID&, bool);
    std::future<void> insert_frontier(Frontier&&);
};

};  // End of namespace dtc. --------------------------------------------------------------



#endif



