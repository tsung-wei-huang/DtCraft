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

#include <dtc/kernel/executor.hpp>

namespace dtc {

// Constructor
Executor::Executor(Graph& G) : 
  KernelBase {Policy::get().EXECUTOR_NUM_THREADS()},
  _graph {G} {
}

// Destructor
Executor::~Executor() {
}

// Procedure: run
void Executor::run() {

  switch(Policy::get().EXECUTION_MODE()) {
    case Policy::LOCAL:
      _setup_local();
    break;

    case Policy::SUBMIT:
      _setup_submit();
    break;

    case Policy::DISTRIBUTED:
      _setup_distributed();
    break;
  }

  dispatch();

  switch(Policy::get().EXECUTION_MODE()) {
    case Policy::LOCAL:
    break;

    case Policy::SUBMIT:
    break;
    
    case Policy::DISTRIBUTED:
      _teardown_distributed();
    break;
  }
}

// Function: num_vertices
// Query the number of vertices associated to this executor.
std::future<size_t> Executor::num_vertices() {
  return promise([E=this] () { return E->_graph._vertices.size(); });
}

// Function: _num_vertices
// Query the number of streams associated to this executor.
size_t Executor::_num_vertices() const {
  assert(is_owner());
  return _graph._vertices.size();
}

// Function: _num_task_events
size_t Executor::_num_task_events() const {

  assert(is_owner());

  switch(Policy::get().EXECUTION_MODE()) {
    case Policy::LOCAL:
      return num_events();
    break;

    case Policy::DISTRIBUTED:
      return num_events() - 2;
    break;

    default:
      return 0;
    break;
  }
}

// Function: num_streams
// Query the number of streams associated to this executor.
std::future<size_t> Executor::num_streams() {
  return promise([E=this] () { return E->_num_streams(); });
}

// Function: _num_streams
// Query the number of streams associated to this executor.
size_t Executor::_num_streams() const {
  assert(is_owner());
  return _graph._streams.size();
}

// Function: graph_size
// Query the number of streams and vertices associated to this executor.
std::future<size_t> Executor::graph_size() {
  return promise([E=this] () { return E->_graph_size(); });
}

// Function: _graph_size
// Query the number of streams and vertices associated to this executor.
size_t Executor::_graph_size() const {
  assert(is_owner());
  return _num_streams() + _num_vertices();
}

// Function: _remove_stream
void Executor::_remove_stream(key_type key, std::error_code& errc) {

  // Remove the stream event.
  if(auto sitr = _graph._streams.find(key); sitr != _graph._streams.end()) {
    _remove(sitr->second._ostream.lock());
    _remove(sitr->second._istream.lock());
  }

  // Stop the executor in case of an error
  if(errc) {
    LOGE("Executor encountered stream error: ", errc.message());
    for(auto& kvp : _graph._streams) {
      _remove(kvp.second._ostream.lock());
      _remove(kvp.second._istream.lock());
    }
    goto shutdown;
  }

  // Update the task error code
  if(_num_task_events() != 0) {
    return;
  }

  shutdown:

  // Shut down the executor.
  if(_agent) {
    _remove(std::move(_agent->ostream));
    _remove(std::move(_agent->istream));
  }

  assert(num_events() == 0);
}

// Function: remove_stream
// Update the task with a given error code. A task refers to either a vertex event or a stream
// event that is associated with a key. 
std::future<void> Executor::remove_stream(key_type key, std::error_code errc) {
  return promise([E=this, key, errc=std::move(errc)] () mutable { E->_remove_stream(key, errc); });
}

// Procedure: _setup_local
// Launch the graph in local mode. Local mode is the default execution policy running from user's
// terminal without using the submission script. Running a stream graph in local mode does not 
// involve any resource control managed by the agent.
void Executor::_setup_local() {
  assert(is_owner());
  //LOGI("Executor launched in local mode");
  _make_graph(nullptr);
}

// Procedure: _setup_submit
// Launch the executor in submit mode. The procedure first connects to the master through the
// environment variable set up by the submission script. By default, it is set to localhost, 
// i.e., 127.0.0.1. Once connected, the executor topologize the graph
void Executor::_setup_submit() {

  assert(is_owner());

  _insert_stdout_listener();
  _insert_stderr_listener();

  LOGI(
    "Executor @", Policy::get().THIS_HOST(), " ",
    "[stdout:", Policy::get().STDOUT_LISTENER_PORT(), 
    "|stderr:", Policy::get().STDERR_LISTENER_PORT(), "]"
  );

  LOGI(
    "Submitting graph to master @",
    Policy::get().MASTER_HOST(), ":", Policy::get().GRAPH_LISTENER_PORT()
  ); 
  
  auto M = make_socket_client(Policy::get().MASTER_HOST(), Policy::get().GRAPH_LISTENER_PORT());
  
  _master = _insert_actor<Master>(M)(
    [E=this] (const std::error_code& errc) { 
      LOGF("Error on master IO (", errc.message(), ")"); 
    },
    [E=this] (pb::Solution& s) { 
      LOGI("Solution received\n", s);
      E->shutdown(); 
    }
  );

  (*_master->ostream)(pb::Protobuf(_graph._topologize()));
}
    
// Procedure: _setup_distributed
// Launch the executor in the distributed mode. The procedure first gets the file descriptor to 
// communicate with the agent and initiates two I/O events. The istream is responsible for receiving
// a list of commands from the agent, including the topology. Once the topology is received, the 
// graph associated with this executor will be initialized together with vertex events and stream
// events.
void Executor::_setup_distributed() {    

  assert(is_owner());
    
  //LOGI("Executor launched in distributed mode");

  const auto fd = Policy::get().TOPOLOGY_FD();

  make_fd_close_on_exec(fd);

  auto skt = std::make_shared<Socket>(fd);

  _agent = _insert_actor<Agent>(skt)(
    [E=this] (const std::error_code& errc) { LOGF("Error on agent IO (", errc.message(), ")"); },
    [E=this] (pb::Topology& t) { E->make_graph(std::move(t)); }
  );

  // Build standard stream channels.
  _agent->stdout = duplicate_fd(STDOUT_FILENO);
  _agent->stderr = duplicate_fd(STDERR_FILENO);

  assert(is_fd_valid(Policy::get().STDOUT_FD()));
  assert(is_fd_valid(Policy::get().STDERR_FD()));

  redirect_fd(STDOUT_FILENO, Policy::get().STDOUT_FD());
  redirect_fd(STDERR_FILENO, Policy::get().STDERR_FD());
}

// Procedure: _teardown_distributed
void Executor::_teardown_distributed() {
  assert(is_owner());
  std::cout.flush();
  std::cerr.flush();
  ::fsync(STDOUT_FILENO);
  ::fsync(STDERR_FILENO);
  redirect_fd(STDOUT_FILENO, _agent->stdout);
  redirect_fd(STDERR_FILENO, _agent->stderr);
  _agent->stdout = STDOUT_FILENO;
  _agent->stderr = STDERR_FILENO;
}

// Procedure: make_graph
// The public call to initialize the graph from the given topology.
// will only take place once. This function is called by submit and distributed mode.
std::future<void> Executor::make_graph(pb::Topology&& tpg) {
  return promise([E=this, tpg=std::move(tpg)] () mutable { E->_make_graph(&tpg); });
}

// Procedure: _make_graph
// The bottom-most call to initiate the graph from a given topology. Thie procedure should be
// called by only once as stream events and vertex events cannot be duplicated.
// Notice that the procedure must be atomic and private, as all stream handlers will not be ready
// until the call of _make_streams. Thanks to the design of the reactor, we guarantee events
// will happen after this private function call.
// The procedure is only called by submit and distributed mode.
void Executor::_make_graph(pb::Topology* tpg) {
  assert(is_owner());
  _graph._make(tpg);
  _make_vertices();
  _make_streams();
}

// Procedure: _make_vertices
// Create a timeout event for each vertex. The callback from the vertex will be executed once
// before all adjacent stream events can take place.
// Notice here we create a timeout event for every vertex in order to synchronize all events.
void Executor::_make_vertices() {
  assert(is_owner());
  for(auto& kvp : _graph._vertices) {
    _insert<TimeoutEvent>(0ms, [E=this, &v=kvp.second] (Event& e) { v(); });
  }
}

// Procedure: _make_streams
// Create IO events for each stream of the graph. For inter stream, we simply create a file
// descriptor pair with self-pipe trick to enable non-blocking communcation. For distributed
// mode the file descriptors for inter stream is stored in the environment variables DTC_FRONTIERS 
// initiated from the agent in the format of "key_type:fd_value".
void Executor::_make_streams() {

  assert(is_owner());

  const auto& ftrs = Policy::get().FRONTIERS();

  // Create iostream for each stream.
  for(auto& [key, stream] : _graph._streams) {

    std::shared_ptr<Device> device;

    // Case 1: intra stream
    if(auto fitr=ftrs.find(std::to_string(key)); fitr == ftrs.end()) {
      assert(stream._head && stream._tail);
      device = make_shared_memory();
      //LOGI("Created an intra stream for ", key, " fd=", device->fd());
    }
    // Case 2: inter stream
    else {
      make_fd_close_on_exec(std::stoi(fitr->second));
      //LOGI("Created an inter stream for ", key, " fd=", fitr->second);
      device = std::make_shared<Socket>(std::stoi(fitr->second)); 
    }

    stream._istream = _insert_stream(device, std::ios_base::in)(
      [E=this, &stream] (const std::error_code& errc) {
        auto e = stream.is_inter_stream(std::ios_base::in) ? errc : std::error_code();
        E->remove_stream(stream.key, e);
      },
      [E=this, &stream] (InputStream& istream) {
        assert(stream._head);
        if(auto r = stream(*(stream._head), istream); r == Stream::CLOSE) {
          E->remove_stream(stream.key, std::error_code());
        }
      }
    ).first;

    stream._ostream = _insert_stream(device, std::ios_base::out)(
      [E=this, &stream] (const std::error_code& errc) {
        auto e = stream.is_inter_stream(std::ios_base::out) ? errc : std::error_code();
        E->remove_stream(stream.key, e);
      },
      [E=this, &stream] (OutputStream& ostream) {
        assert(stream._tail);
        if(auto r = stream(*(stream._tail), ostream); r == Stream::CLOSE) {
          E->remove_stream(stream.key, std::error_code());
        }
      }
    ).second;

  }
}



}  // End of namespace dtc::graph. ----------------------------------------------------


