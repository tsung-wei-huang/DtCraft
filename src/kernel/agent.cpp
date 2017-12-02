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

#include <dtc/kernel/agent.hpp>

namespace dtc {

// Function: ready
bool Agent::Topology::ready() const {
  return num_frontiers && frontiers.size() == *num_frontiers;
}

// ------------------------------------------------------------------------------------------------

// Constructor
Agent::Agent() : KernelBase{Policy::get().AGENT_NUM_THREADS()} {
  
  _make_master();
  _make_frontier_listener();

  // Logging
  LOGI(
    "Agent @", Policy::get().THIS_HOST(), " [frontier:", Policy::get().FRONTIER_LISTENER_PORT(), "]"
  );
}

// Procedure: _make_master
// The procedure connects to the master and initiate I/O events to communicate with the master.
void Agent::_make_master() {

  assert(is_owner());

  // Connect to the master
  auto M = make_socket_client(Policy::get().MASTER_HOST(), Policy::get().AGENT_LISTENER_PORT());
  
  _master = _insert_actor<Master>(M)(
    [A=this] (const std::error_code& errc) { LOGF("Error on the master IO (", errc.message(), ")"); },
    [A=this] (pb::Topology& t) { A->schedule(std::move(t)); },
    [A=this] (pb::KillTask& s) { A->remove_executor(s.task_id, true); }
  );
  
  // Write the resource
  (*_master.ostream)(pb::Protobuf(pb::Resource().update()));
}

// Procedure: _make_frontier_listener
// Creates a socket server listenering to any in-coming frontiers for the inter stream
// communication. By default, the write end of an inter stream initiates a socket connection
// and the read end of the inter stream waits for receiving the connection. Once the connection
// is established, the read end will receive a frontier message indicating the information of
// the inter stream.
void Agent::_make_frontier_listener() {

  assert(is_owner());
  
  _insert_listener(Policy::get().FRONTIER_LISTENER_PORT())(
    [A=this] (std::shared_ptr<Socket>&& skt) { 
      A->insert_stream(std::move(skt), std::ios_base::in)(
        [] (const std::error_code& e) {
          LOGF("Frontier error on read! ", e.message());
        },
        [A] (InputStream& istream) {
          if(pb::Frontier ftr; istream(ftr) != -1) {
            LOGI("Frontier received for stream ", ftr.stream, " fd=", istream.isbuf.device()->fd());
            assert(istream.isbuf.in_avail() == 0);
            ftr.socket = std::static_pointer_cast<Socket>(istream.isbuf.device());
            A->schedule(std::move(ftr));
            A->remove(istream.shared_from_this());
          }
        }
      );

    }
  );
}

// Function: _schedule
// Schedule a given frontier. The agent manages a list of open frontiers and checks for each
// new frontier whether the corresponding topology is ready to fork.
void Agent::_schedule(pb::Frontier& ftr) {
  assert(is_owner());
  auto key = ftr.task_id();
  _topologies[key].frontiers.emplace_back(std::move(ftr));
  if(_topologies[key].ready()) {
    _deploy(key);
  }
}

// Function: schedule
std::future<void> Agent::schedule(pb::Frontier&& ftr) {
  return promise([A=this, ftr=std::move(ftr)] () mutable {A->_schedule(ftr);});
}

// Function: _schedule
void Agent::_schedule(pb::Topology& tpg, int stdout, int stderr) {

  assert(is_owner());
    
  LOGI("Received topology ", tpg.task_id());

  auto key = tpg.task_id();

  /*// Build a connector for each inter-stream.
  for(const auto &[skey, stream] : tpg.streams) {

    if(!tpg.has_inter_stream(skey, std::ios_base::in)) {
      continue;
    }

    //LOGD("building istream for key=", skey, " to ", stream.tail_host); 
    
    // TODO: the following may throw
    auto skt = make_socket_client(stream.tail_host, Policy::get().FRONTIER_LISTENER_PORT());
    assert(stream.head_topology == tpg.id);
    pb::Frontier ftr{tpg.graph, stream.tail_topology, skey, std::move(skt)};

    write_at_once(ftr.socket, ftr);
    LOGI("Frontier written key/fd=", ftr.stream, "/", ftr.socket->fd());

    ftr.topology = stream.head_topology;

    _schedule(ftr);    
  }*/

  _topologies[key].num_frontiers = tpg.num_inter_streams();
  _topologies[key].topology = std::move(tpg);
  _topologies[key].stdout = stdout;
  _topologies[key].stderr = stderr;

  if(_topologies[key].ready()) {
    _deploy(key);
  }
}

// Function: schedule
// Schedule a given topology. The agent initiate a ostream event for every ostream side of the
// inter stream and maintains a list of topology. Then the agent check whether the topology
// is ready based on the list of frontiers. A ready topology will be deployed to an executor
// for further execution.
// Note that it must be the istream side of an inter-stream to initiate the socket connection,
// because we don't want to mix the oder with user-defined read/write handlers. For example,
// if the ostream side of an inter-stream builds up the socket connection, it can easily get
// mixed with user-space data and the read end may accidentally synchronize user-space data 
// in the agent.
std::future<void> Agent::schedule(pb::Topology&& tpg) {

  // Create two connectors for stdout and stderr.
  auto stdout = make_socket_client_fd(
    tpg.envp.at("DTC_THIS_HOST"), tpg.envp.at("DTC_STDOUT_LISTENER_PORT")
  );

  auto stderr = make_socket_client_fd(
    tpg.envp.at("DTC_THIS_HOST"), tpg.envp.at("DTC_STDERR_LISTENER_PORT")
  );
  
  // Build a connector for each inter-stream.
  for(const auto &[skey, stream] : tpg.streams) {

    if(!tpg.has_inter_stream(skey, std::ios_base::in)) {
      continue;
    }

    //LOGD("building istream for key=", skey, " to ", stream.tail_host); 
    
    // TODO: the following may throw
    auto skt = make_socket_client(stream.tail_host, Policy::get().FRONTIER_LISTENER_PORT());
    assert(stream.head_topology == tpg.id);
    pb::Frontier ftr{tpg.graph, stream.tail_topology, skey, std::move(skt)};

    write_at_once(ftr.socket, ftr);
    LOGI("Frontier written key/fd=", ftr.stream, "/", ftr.socket->fd());

    ftr.topology = stream.head_topology;

    schedule(std::move(ftr));    
  }

  return promise(
    [A=this, tpg=std::move(tpg), stdout, stderr] () mutable {
      A->_schedule(tpg, stdout, stderr);
    }
  );
}

// Procedure: _deploy
void Agent::_deploy(const TaskID& key) {

  assert(is_owner());
  
  auto node = _topologies.extract(key);

  assert(!node.empty() && node.mapped().topology);
  
  LOGI("Deploying topology ", key, " (distributed mode) ...");

  node.mapped().topology->envp["DTC_EXECUTION_MODE"] = "distributed";

  for(auto& ftr : node.mapped().frontiers) {
    ftr.socket->open_on_exec(true);
    node.mapped().topology->envp["DTC_FRONTIERS"] += (ftr.to_kvp() + ' ');
  }
  
  LOGI("FRONTIERS=", node.mapped().topology->envp["DTC_FRONTIERS"]);

  // Create a communication channel based on domain sockets.
  auto dp = make_domain_pair();
  std::get<1>(dp)->open_on_exec(true);
  node.mapped().topology->envp["DTC_TOPOLOGY_FD"] = std::to_string(std::get<1>(dp)->fd());

  // Create a stdout/stderr channel.
  node.mapped().topology->envp["DTC_STDOUT_FD"] = std::to_string(*(node.mapped().stdout));
  node.mapped().topology->envp["DTC_STDERR_FD"] = std::to_string(*(node.mapped().stderr));

  make_fd_open_on_exec(*(node.mapped().stdout));
  make_fd_open_on_exec(*(node.mapped().stderr));

  LOGI("Executor communication channel ", std::get<0>(dp)->fd(), "/", std::get<1>(dp)->fd());
  
  auto& E = _executors.try_emplace(
    key,
    _insert_actor<Executor>(std::get<0>(dp), key)(
      [A=this, key] (const std::error_code&) { A->remove_executor(key, false); }
    )
  ).first->second;  

  // Fork-exec
  try {
    E.container.exec2(*node.mapped().topology);
    (*E.ostream)(pb::Protobuf(*(node.mapped().topology)));
  }
  catch (const std::system_error& s) {
    _remove_executor(key, false, s.code());
  }

  // Make frontier close-on-exec to avoid leak. At this point, the istream/ostream of the 
  // frontier may still exist in the reactor. If the next fork comes before them are removed,
  // the file descriptor can leak.
  for(auto& ftr : node.mapped().frontiers) {
    ftr.socket->open_on_exec(false);
  }
  
  make_fd_close_on_exec(*(node.mapped().stdout));
  make_fd_close_on_exec(*(node.mapped().stderr));

  close(*(node.mapped().stdout));
  close(*(node.mapped().stderr));
  
  LOGI("Topology ", key, " successfully deployed!");
};

// Function: _remove_executor
// Remove a given task from the agent. Upon removal the agent sends master a taskinfo 
// associated with the task.
bool Agent::_remove_executor(const TaskID& key, bool kill, std::error_code errc) {

  assert(is_owner());

  if(auto titr = _executors.find(key); titr != _executors.end()) {

    _remove(std::move(titr->second.istream));
    _remove(std::move(titr->second.ostream));
    
    int status = 0;

    if(kill) {
      //LOGI("killing task ", key.to_string());
      titr->second.container.kill();
    }
    //LOGI("reaping task ", key.to_string());
    status = titr->second.container.reap(); 
    
    _executors.erase(titr);
    LOGI("Executor ", key, " is removed from the agent (status=", status, ", errc=", errc.message(), ")");

    (*_master.ostream)(pb::Protobuf( pb::TaskInfo{key, errc, status} ));

    return true;
  }
  return false;
}

// Function: remove_executor
// The public wrapper of remove task function.
std::future<bool> Agent::remove_executor(const TaskID& key, bool kill) {
  return promise(
    [A=this, key, kill] () mutable { 
      return A->_remove_executor(key, kill, std::error_code()); 
    }
  );
}

};  // End of namespace dtc. --------------------------------------------------------------









