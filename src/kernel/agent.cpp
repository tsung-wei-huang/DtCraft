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

#include <dtc/kernel/agent.hpp>

namespace dtc {

// ------------------------------------------------------------------------------------------------

// Constructor
Agent::Task::Task(pb::Topology&& tpg) : 
  topology{std::move(tpg)} {

  auto& h = std::get<Hatchery>(handle);

  h.num_inter_streams = topology.num_inter_streams();
  
  // Set up the execution mode.
  //topology.runtime["DTC_EXECUTION_MODE"] = "distributed";
  topology.runtime.execution_mode(ExecutionMode::DISTRIBUTED);

  // Create two connectors for stdout and stderr.
  //h.stdout = make_socket_client(
  //  topology.runtime.at("DTC_THIS_HOST"), topology.runtime.at("DTC_STDOUT_LISTENER_PORT")
  //);
  //topology.runtime["DTC_STDOUT_FD"] = std::to_string(h.stdout->fd());

  //h.stderr = make_socket_client(
  //  topology.runtime.at("DTC_THIS_HOST"), topology.runtime.at("DTC_STDERR_LISTENER_PORT")
  //);
  //topology.runtime["DTC_STDERR_FD"] = std::to_string(h.stderr->fd());
  
  h.stdout = make_socket_client(
    topology.runtime.this_host(), topology.runtime.stdout_listener_port()
  );
  topology.runtime.stdout_fd(h.stdout->fd());

  h.stderr = make_socket_client(
    topology.runtime.this_host(), topology.runtime.stderr_listener_port()
  );
  topology.runtime.stderr_fd(h.stderr->fd());


  const auto vhosts = topology.runtime.vertex_hosts();
  
  // Build a connector for each inter-stream. 
  // Note that it must be the istream side of an inter-stream to initiate the socket connection,
  // because we don't want to mix the oder with user-defined read/write handlers. For example,
  for(const auto &[skey, stream] : topology.streams) {

    if(!topology.has_inter_stream(skey, std::ios_base::in)) {
      continue;
    }
    
    assert(vhosts.find(stream.tail) != vhosts.end());
    
    Frontier ftr{
      topology.graph, 
      skey, 
      make_socket_client(vhosts.at(stream.tail), env::frontier_listener_port())
    };
    
    FrontierPacket pkt {ftr.graph, ftr.stream};
    ftr.socket->flush(&pkt, sizeof(pkt));

    LOGI("Frontier written key/fd=", ftr.stream, "/", ftr.socket->fd());
    
    h.frontiers.push_back(std::move(ftr));
  }
}

// Function: ready
bool Agent::Task::ready() const {
  return hatchery().frontiers.size() == hatchery().num_inter_streams;
}

// Function: match
bool Agent::Task::match(const Frontier& ftr) const {
  return topology.graph == ftr.graph && topology.has_inter_stream(ftr.stream, std::ios_base::out);
}

// Procedure: splice_frontiers
void Agent::Task::splice_frontiers(std::list<Frontier>& src) {
  auto itr = std::partition(src.begin(), src.end(), [&] (const auto& ftr) { 
    return match(ftr); 
  });
  hatchery().frontiers.splice(hatchery().frontiers.begin(), src, src.begin(), itr);
}

// Function: frontiers_to_string
std::string Agent::Task::frontiers_to_string() const {
  std::ostringstream oss;
  for(const auto& ftr : hatchery().frontiers) {
    oss << ftr.stream << ":" << ftr.socket->fd() << ' ';
  }
  return oss.str();
}

// ------------------------------------------------------------------------------------------------

// Constructor
Agent::Agent() : KernelBase{env::agent_num_threads()} {
  
  _make_master();
  _make_frontier_listener();

  // Logging
  LOGI("Agent @", env::this_host(), " [frontier:", env::frontier_listener_port(), "]");
}

// Procedure: _make_master
// The procedure connects to the master and initiate I/O events to communicate with the master.
void Agent::_make_master() {

  assert(is_owner());

  // Connect to the master
  auto M = make_socket_client(env::master_host(), env::agent_listener_port());
  
  _master = std::make_optional<Master>();

  std::tie(_master->istream, _master->ostream) = insert_channel(std::move(M))(
    [this] (pb::BrokenIO& b) { 
      LOGE("Error on the master IO (", b.errc.message(), ")"); 
      std::exit(EXIT_BROKEN_CONNECTION);
    },
    [this] (pb::Topology& t) { 
      insert_task(std::move(t)); 
    },
    [this] (pb::KillTask& s) { 
      remove_task(s.task_id, true); 
    }
  );
  
  // Write the resource
  (*_master->ostream)(pb::Protobuf(pb::Resource().update()));
}

// Procedure: _make_frontier_listener
// Creates a socket server listenering to any in-coming frontiers for the inter stream
// communication. By default, the write end of an inter stream initiates a socket connection
// and the read end of the inter stream waits for receiving the connection. Once the connection
// is established, the read end will receive a frontier message indicating the information of
// the inter stream.
void Agent::_make_frontier_listener() {

  assert(is_owner());
  
  insert_listener(env::frontier_listener_port())(
    [this] (std::shared_ptr<Socket> skt) { 
      
      // Insert a read event. Notice that we need to share the ownership of the socket.
      insert<ReadEvent>(
        std::move(skt), 
        [this] (Event& event) {
          FrontierPacket pkt;
          event.device()->purge(&pkt, sizeof(pkt)); 
          LOGI("Frontier received for stream ", pkt.stream, " fd=", event.device()->fd());
          insert_frontier(Frontier{pkt.graph, pkt.stream, std::static_pointer_cast<Socket>(event.device())});
          return Event::REMOVE;
        }
      );
    }
  );
}

// Function: _insert_frontier
// Schedule a given frontier. The agent manages a list of open frontiers and checks for each
// new frontier whether the corresponding topology is ready to fork.
void Agent::_insert_frontier(Frontier& ftr) {

  assert(is_owner());

  // Find the corresponding task
  auto itr = std::find_if(_tasks.begin(), _tasks.end(), [&](const auto& kvp){
    return kvp.second.match(ftr);
  });
  
  if(itr == _tasks.end()) {
    _frontiers.push_back(std::move(ftr));
  }
  else {
    itr->second.hatchery().frontiers.push_back(std::move(ftr));
    if(itr->second.ready() && !_deploy(itr->second)) {
      _remove_task(itr->first, false);
    }
  }
}

// Function: insert_frontier
std::future<void> Agent::insert_frontier(Frontier&& ftr) {
  return promise([this, ftr=std::move(ftr)] () mutable {_insert_frontier(ftr);});
}

// Function: _insert_task
bool Agent::_insert_task(Task& task) {

  assert(is_owner());
  
  const auto key = task.topology.task_id();
    
  task.splice_frontiers(_frontiers);

  if(!task.ready() || _deploy(task)) {
    _tasks.insert_or_assign(key, std::move(task));
    return true;
  }
  else {
    (*_master->ostream)(pb::Protobuf(pb::TaskInfo{key, env::this_host(), -1}));
    return false;
  }
}

// Function: schedule
// Schedule a given topology. The agent initiate a ostream event for every ostream side of the
// inter stream and maintains a list of topology. Then the agent check whether the topology
// is ready based on the list of frontiers. A ready topology will be deployed to an task
// for further execution.
std::future<bool> Agent::insert_task(pb::Topology&& tpg) {

  const auto key = tpg.task_id();

  try {
    return promise(
      [this, task=Task{std::move(tpg)}] () mutable {
        return _insert_task(task);
      }
    );
  }
  catch(const std::exception& e) {
    LOGE("Failed to insert task ", key, " (", e.what(), ')');
    (*_master->ostream)(pb::Protobuf(pb::TaskInfo{key, env::this_host(), -1}));
    return std::async(std::launch::deferred, [](){ return false; });
  }
}

// Procedure: _deploy
bool Agent::_deploy(Task& task) {

  assert(is_owner());

  const auto key = task.topology.task_id();

  try {
    LOGI("Deploying task ", task.topology.task_id(), " ...");
    //LOGI("DTC_VERTEX_HOSTS=", task.topology.runtime.at("DTC_VERTEX_HOSTS"));
    
    // Create the frontier lists.
    //task.topology.runtime["DTC_FRONTIERS"] = task.frontiers_to_string();
    //LOGI("FRONTIERS=", task.topology.runtime["DTC_FRONTIERS"]);
    task.topology.runtime.frontiers(task.frontiers_to_string());

    // Create a communication channel based on domain sockets.
    auto [askt, eskt] = make_socket_pair();

    task.topology.runtime.topology_fd(eskt->fd());
    LOGI("Task communication channel ", askt->fd(), "/", eskt->fd());

    std::vector<ScopedOpenOnExec> devices;

    for(auto& ftr : task.hatchery().frontiers) {
      devices.emplace_back(std::move(ftr.socket));
    }
    devices.emplace_back(std::move(eskt));
    devices.emplace_back(std::move(task.hatchery().stderr));
    devices.emplace_back(std::move(task.hatchery().stdout));

    // Hatch into the executor.
    auto& executor = task.handle.emplace<Task::Executor>();
    executor.container.exec2(task.topology);
    std::tie(executor.istream, executor.ostream) = insert_channel(std::move(askt))(
      [this, key] (pb::BrokenIO&) { remove_task(key, false); }
    );
    (*(executor.ostream))(pb::Protobuf(task.topology));

    LOGI("Successfully deployed task ", key);
    return true;
  }
  catch (const std::exception& e) {
    LOGE("Failed to deploy task ", key, " (", e.what(), ")");
    return false;
  }
}

// Function: _remove_task
// Remove a given task from the agent. Upon removal the agent sends master a taskinfo 
// associated with the task.
bool Agent::_remove_task(const TaskID& key, bool kill) {

  assert(is_owner());

  if(auto titr = _tasks.find(key); titr != _tasks.end()) {

    auto status = std::visit(Functors{
      // Task is being incubated.
      [&] (Task::Hatchery& h) {
        return -1;
      },
      // Task is deployed.
      [&] (Task::Executor& e) {
        if(e.container.pid() != -1) {
          if(kill) {
            e.container.kill();
          }
          e.container.wait();
        }
        remove(std::move(e.istream), std::move(e.ostream));
        return e.container.status();
      }
    }, titr->second.handle);
    
    _tasks.erase(titr);
    LOGI("Task ", key, " is removed (status=", status, ")");

    (*_master->ostream)(pb::Protobuf(pb::TaskInfo{key, env::this_host(), status}));

    return true;
  }

  return false;
}

// Function: remove_task
// The public wrapper of remove task function.
std::future<bool> Agent::remove_task(const TaskID& key, bool kill) {
  return promise(
    [this, key, kill] () mutable { 
      return _remove_task(key, kill); 
    }
  );
}

};  // End of namespace dtc. --------------------------------------------------------------









