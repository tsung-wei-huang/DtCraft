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
Agent::Executor::Executor(const std::filesystem::path& cgroup) :
  container {cgroup} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Agent::Task::Task(pb::Topology&& tpg) : 
  key       {tpg.task_id()},
  topology  {std::move(tpg)} {

  hatchery().num_inter_streams = topology.num_inter_streams();
  
  // Set up the execution mode.
  topology.runtime.execution_mode(ExecutionMode::DISTRIBUTED);

  hatchery().stdout = make_socket_client(
    topology.runtime.this_host(), topology.runtime.stdout_listener_port()
  );
  topology.runtime.stdout_fd(hatchery().stdout->fd());

  hatchery().stderr = make_socket_client(
    topology.runtime.this_host(), topology.runtime.stderr_listener_port()
  );
  topology.runtime.stderr_fd(hatchery().stderr->fd());

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
    
    hatchery().frontiers.push_back(std::move(ftr));
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

// Function: hatchery
inline Agent::Hatchery& Agent::Task::hatchery() { 
  return std::get<Hatchery>(handle); 
}

// Function: executor
inline Agent::Executor& Agent::Task::executor() { 
  return std::get<Executor>(handle); 
}

// Function: hatchery
inline const Agent::Hatchery& Agent::Task::hatchery() const { 
  return std::get<Hatchery>(handle); 
}

// Function: executor
inline const Agent::Executor& Agent::Task::executor() const { 
  return std::get<Executor>(handle); 
}

// ------------------------------------------------------------------------------------------------

// Constructor
Agent::Agent() : 
  KernelBase {env::agent_num_threads()},
  _cgroup    {env::agent_cgroup()} {
  
  // Initialization
  _init_cgroup();
  _init_frontier_listener();
  _init_master();

  // Logging
  LOGI("Agent @", env::this_host(), " [frontier:", env::frontier_listener_port(), "]");

  // Control group
  LOGI("cg-subsys.memory ", _cgroup.memory_mount(), "[limit:", _cgroup.memory_limit_in_bytes(), "]");
  LOGI("cg-subsys.cpuset ", _cgroup.cpuset_mount(), "[#cpus:", _cpuset.size(), "]");
}

// Destructor
Agent::~Agent() {
}

// Procedure: _init_cgroup
void Agent::_init_cgroup() {

  // Set up the memory limit.
  _cgroup.memory_limit_in_bytes(
    std::min(_cgroup.memory_limit_in_bytes(), Statgrab::get().memory_limit_in_bytes())
  );

  // Set up the cpus
  _cpuset = _cgroup.cpuset_cpus();

  // Group this process.
  _cgroup.add(::getpid());
}

// Procedure: _init_master
// The procedure connects to the master and initiate I/O events to communicate with the master.
void Agent::_init_master() {

  assert(is_owner());

  // Connect to the master
  auto M = make_socket_client(env::master_host(), env::agent_listener_port());
  
  std::tie(_master.istream, _master.ostream) = insert_channel(std::move(M))(
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

  // Write the resource to the master
  pb::Resource resource;
  resource.num_cpus = _cpuset.size();
  resource.memory_limit_in_bytes = _cgroup.memory_limit_in_bytes();
  resource.space_limit_in_bytes = Statgrab::get().space_limit_in_bytes();

  (*_master.ostream)(pb::Protobuf(std::move(resource)));
}

// Procedure: _init_frontier_listener
// Creates a socket server listenering to any in-coming frontiers for the inter stream
// communication. By default, the write end of an inter stream initiates a socket connection
// and the read end of the inter stream waits for receiving the connection. Once the connection
// is established, the read end will receive a frontier message indicating the information of
// the inter stream.
void Agent::_init_frontier_listener() {

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
      _remove_task(itr->first, true);
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
    _remove_task(task, true);
    return false;
  }
}

// Function: insert_task
// Prepare a task hatchery for a give topology.
std::future<bool> Agent::insert_task(pb::Topology&& tpg) {

  try {
    return promise(
      [this, task=Task{std::move(tpg)}] () mutable {
        return _insert_task(task);
      }
    );
  }
  catch(const std::exception& e) {
    const auto key = tpg.task_id();
    LOGE("Failed to insert task ", key, " (", e.what(), ')');
    (*_master.ostream)(pb::Protobuf(pb::TaskInfo{key, env::this_host(), -1}));
    return std::async(std::launch::deferred, [](){ return false; });
  }
}

// Procedure: _deploy
bool Agent::_deploy(Task& task) {

  assert(is_owner());

  try {
    LOGI("Deploying task ", task.topology.task_id(), " ...");
    //LOGI("DTC_VERTEX_HOSTS=", task.topology.runtime.at("DTC_VERTEX_HOSTS"));
    
    // Create the frontier lists.
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
    auto& executor = task.handle.emplace<Executor>(_cgroup.path() / task.key.to_string());

    // Extract the resource request.
    assert(task.topology.containers.size() == 1);
    auto resource = task.topology.resource();
    
    // Set up the memory
    executor.container.cgroup().memory_limit_in_bytes(resource.memory_limit_in_bytes);

    // Set up the cpu
    
    // Spawn the container.
    executor.container.spawn(task.topology);

    // Build up the communication channel.
    std::tie(executor.istream, executor.ostream) = insert_channel(std::move(askt))(
      [this, key=task.key] (pb::BrokenIO&) { remove_task(key, false); }
    );

    // Send the topology to the executor.
    (*(executor.ostream))(pb::Protobuf(task.topology));

    LOGI("Task ", task.key, " successfully deployed [pid=", executor.container.pid(), "]");
    return true;
  }
  catch (const std::exception& e) {
    LOGE("Failed to deploy task ", task.key, " (", e.what(), ")");
    return false;
  }
}

// Procedure: _remove_taks
void Agent::_remove_task(Task& task, bool kill) {
  
  assert(is_owner() && _tasks.find(task.key) == _tasks.end());
  
  pb::TaskInfo taskinfo {task.key, env::this_host(), -1};
  
  // Here we use get_if because task might be valueless.
  if(auto eptr = std::get_if<Executor>(&task.handle)) {
    if(eptr->container.pid() != -1) {
      if(kill) {
        eptr->container.kill();
      }
      eptr->container.wait();
    }  
    remove(std::move(eptr->istream), std::move(eptr->ostream));

    taskinfo.status = eptr->container.status();
    taskinfo.memory_limit_in_bytes = eptr->container.cgroup().memory_limit_in_bytes();
    taskinfo.memory_max_usage_in_bytes = eptr->container.cgroup().memory_max_usage_in_bytes();
  }    
  
  // Measure the elapsed time.
  taskinfo.elapsed_time = task.clock.elapsed_time_in_nanoseconds();
 
  // Send master the task information.
  (*_master.ostream)(pb::Protobuf{std::move(taskinfo)});
  
  LOGI("Task ", task.key, " is removed");
}

// Function: _remove_task
// Remove a given task from the agent. Upon removal the agent sends master a taskinfo 
// associated with the task.
void Agent::_remove_task(const TaskID& key, bool kill) {

  assert(is_owner());

  if(auto node = _tasks.extract(key); node) {
    _remove_task(node.mapped(), kill);
  }
}

// Function: remove_task
// The public wrapper of remove task function.
std::future<void> Agent::remove_task(const TaskID& key, bool kill) {
  return promise(
    [this, key, kill] () mutable { 
      _remove_task(key, kill); 
    }
  );
}

};  // End of namespace dtc. --------------------------------------------------------------









