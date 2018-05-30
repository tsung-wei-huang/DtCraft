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

#include <dtc/kernel/graph.hpp>
#include <dtc/kernel/executor.hpp>

namespace dtc {

// Constructor
Executor::Executor(Graph& G) : 
  KernelBase {env::executor_num_threads()},
  _graph {G} {
}

// Destructor
Executor::~Executor() {
}

// Procedure: run
void Executor::run() {

  switch(mode) {
    case ExecutionMode::LOCAL:
      _setup_local();
    break;

    case ExecutionMode::SUBMIT:
      _setup_submit();
    break;

    case ExecutionMode::DISTRIBUTED:
      _setup_distributed();
    break;
  }

  dispatch();

  switch(mode) {
    case ExecutionMode::LOCAL:
      _teardown_local();
    break;

    case ExecutionMode::SUBMIT:
      _teardown_submit();
    break;
    
    case ExecutionMode::DISTRIBUTED:
      _teardown_distributed();
    break;
  }
}

// Function: num_streams
// Query the number of streams associated to this executor.
size_t Executor::num_streams() const {
  return _graph._streams.size();
}

// Function: num_vertices
// Query the number of vertices associated to this executor.
size_t Executor::num_vertices() const {
  return _graph._vertices.size();
}

//// Function: num_probers
//// Query the number of probers associated to this executor.
//size_t Executor::num_probers() const {
//  return _graph._probers.size();
//}

// Function: graph_size
// Query the number of streams and vertices associated to this executor.
size_t Executor::graph_size() const {
  return num_streams() + num_vertices();
}

// Procedure: _remove_istream
// Remove the stream from the istream side.
void Executor::_remove_istream(key_type key) {
  
  assert(is_owner());

  if(auto s = _graph._stream(key); s) {

    if(s->_critical) std::exit(EXIT_CRITICAL_STREAM);

    if(s->is_intra_stream()) {
      remove(s->istream(), s->ostream());
    }
    else {
      remove(s->istream());
    }
  }
}

// Function: remove_istream
std::future<void> Executor::remove_istream(key_type key) {
  return promise([this, key] () mutable { _remove_istream(key); });
}

// Procedure: _remove_ostream
// Remove the stream from the ostream side.
void Executor::_remove_ostream(key_type key) {

  assert(is_owner());

  if(auto s = _graph._stream(key); s) {

    if(s->_critical) std::exit(EXIT_CRITICAL_STREAM);

    if(s->is_intra_stream()) {
      if(auto os = s->ostream(); os) {
        os->remove_on_flush();
      }
    }
    else {
      remove(s->istream());
      if(auto os = s->ostream(); os) {
        os->remove_on_flush();
      }
    }
  }
}

// Function: remove_ostream
std::future<void> Executor::remove_ostream(key_type key) {
  return promise([this, key] () mutable { _remove_ostream(key); });
}

//// Function: remove_prober
//std::future<void> Executor::remove_prober(key_type key) {
//  return promise([this, key] () mutable { _remove_prober(key); });
//}

// Procedure: _setup_local
// Launch the graph in local mode. Local mode is the default execution policy running from user's
// terminal without using the submission script. Running a stream graph in local mode does not 
// involve any resource control managed by the agent.
void Executor::_setup_local() {
  assert(is_owner());
  //LOGI("Executor launched in local mode");
  _make_graph(nullptr);
}

// Procedure: _teardown_local
void Executor::_teardown_local() {
}

// Procedure: _setup_submit
// Launch the executor in submit mode. The procedure first connects to the master through the
// environment variable set up by the submission script. By default, it is set to localhost, 
// i.e., 127.0.0.1. Once connected, the executor topologize the graph
void Executor::_setup_submit() {

  assert(is_owner());

  _stdout_listener = insert_stdout_listener();
  _stderr_listener = insert_stderr_listener();

  LOGI(
    "Executor @", env::this_host(), " ",
    "[stdout:", env::stdout_listener_port(), 
    "|stderr:", env::stderr_listener_port(), "]"
  );

  LOGI("Submit graph to master @", env::master_host(), ":", env::graph_listener_port()); 
  
  auto M = make_socket_client(env::master_host(), env::graph_listener_port());

  _master = std::make_optional<Master>();

  std::tie(_master->istream, _master->ostream) = insert_channel(std::move(M))(
    [this] (pb::BrokenIO& b) { 
      LOGE("Error on master IO (", b.errc.message(), ")"); 
      std::exit(EXIT_BROKEN_CONNECTION);
    },
    [this] (pb::Solution& s) { 
      LOGI("Solution received\n", s);
      break_loop(); 
    }
  );

  (*_master->ostream)(pb::Protobuf(_graph._topologize()));
}

// Procedure: _teardown_submit
void Executor::_teardown_submit() {
  remove(
    std::move(_master->istream), std::move(_master->ostream),
    std::move(_stdout_listener), std::move(_stderr_listener)
  );
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

  const auto fd = env::topology_fd();

  make_fd_close_on_exec(fd);

  auto skt = std::make_shared<Socket>(fd);

  _agent = std::make_optional<Agent>();

  std::tie(_agent->istream, _agent->ostream) = insert_channel(std::move(skt))(
    [this] (pb::BrokenIO& b) -> void { 
      LOGE("Error on agent IO (", b.errc.message(), ")"); 
      std::exit(EXIT_BROKEN_CONNECTION);
    },
    [this] (pb::Topology& t) -> void { 
      make_graph(std::move(t)); 
    }
  );

  // Build standard stream channels.
  _agent->stdout_fd = duplicate_fd(STDOUT_FILENO);
  _agent->stderr_fd = duplicate_fd(STDERR_FILENO);

  assert(is_fd_valid(env::stdout_fd()));
  assert(is_fd_valid(env::stderr_fd()));

  redirect_fd(STDOUT_FILENO, env::stdout_fd());
  redirect_fd(STDERR_FILENO, env::stderr_fd());
}

// Procedure: _teardown_distributed
void Executor::_teardown_distributed() {

  assert(is_owner());

  std::cout.flush();
  std::cerr.flush();
  ::fsync(STDOUT_FILENO);
  ::fsync(STDERR_FILENO);
  redirect_fd(STDOUT_FILENO, _agent->stdout_fd);
  redirect_fd(STDERR_FILENO, _agent->stderr_fd);
  _agent->stdout_fd = STDOUT_FILENO;
  _agent->stderr_fd = STDERR_FILENO;
  
  remove(_agent->ostream, _agent->istream);
}

// Procedure: make_graph
// The public call to initialize the graph from the given topology.
// will only take place once. This function is called by submit and distributed mode.
std::future<void> Executor::make_graph(pb::Topology&& tpg) {
  return promise([this, tpg=std::move(tpg)] () mutable { _make_graph(&tpg); });
}

// Procedure: _make_graph
// Initialize the graph from a given topology. This can be called in either local or distributed
// mode, depending on the tpg pointer value.
void Executor::_make_graph(pb::Topology* tpg) {

  assert(is_owner());

  // Initialize the graph from the topology.
  _graph._make(tpg);
  
  // Create stream events.
  _insert_streams(tpg);

  // Create prober events.
  //_insert_probers(tpg);

  // Create vertex events (this must come after streams).
  _insert_vertices(tpg);
  
  // Stop the executor when the event count is fewer than two.
  if(mode == ExecutionMode::DISTRIBUTED) {
    assert(tpg != nullptr);
    threshold(2);
  }
}
  
// Procedure: _insert_vertices
void Executor::_insert_vertices(pb::Topology* tpg) {

  for(auto& kvp : _graph._vertices) {

    // Assign the executor pointer.
    kvp.second._executor = this;
    
    // Create a timeout event for each vertex.
    insert<TimeoutEvent>(0ms, [this, &v=kvp.second] (Event& e) mutable {
      v();
      if(v.program()) {
        promise([this, program=v._prespawn()] () mutable {
          _spawn(program);
        });  
      }
    });
  }
}

// Procedure: _spawn
// Spawn a vertex program.
void Executor::_spawn(Vertex::Program& program) {

  assert(is_owner());

  try {
    auto& v = _graph._vertices.at(program.vertex);

    LOGI("Spawn vertex pgoram ", program.vertex, " [", v._runtime.program(), "]");
    // Create a communication channel based on domain sockets.
    auto [rp, wp] = make_pipe();

    // Open-on-exec all related devices.
    std::vector<ScopedDeviceRestorer> devices;
    for(auto& bridge : program.bridges) {
      devices.emplace_back(std::move(bridge));
    }
    devices.emplace_back(std::move(wp));
    
    // Fork-exec
    auto pid = spawn(program.c_file.get(), program.c_argv.get(), program.c_envp.get());

    // Build a connection channel.
    LOGI("Successfully spawned vertex program ", program.vertex);
    insert<ReadEvent>(std::move(rp), [pid] (Event& ev) {
      int s;
      assert(::read(ev.device()->fd(), &s, sizeof(s)) == 0); 
      assert(::waitpid(pid, &s, 0) == pid);
      if((WIFEXITED(s) && WEXITSTATUS(s) != EXIT_SUCCESS) || WIFSIGNALED(s)) {
        std::exit(s);
      }
      LOGI("Vertex program sccessfully exited");
      return Event::REMOVE;
    });
  }
  catch(std::exception& e) {
    LOGE("Failed to spawn vertex program ", program.vertex, " (", e.what(), ")");
    std::exit(EXIT_VERTEX_PROGRAM_FAILED);
  }
}

// Procedure: _insert_streams
// Create an IO event for each stream of the graph. The stream and fd information is stored 
// in the runtime variable of topology.
void Executor::_insert_streams(pb::Topology* tpg) {

  auto frontiers = tpg ? tpg->runtime.frontiers() : std::unordered_map<key_type, int>();
  
  for(auto& [key, stream] : _graph._streams) {

    // Case 1: intra stream
    if(auto fitr=frontiers.find(key); fitr == frontiers.end()) {

      assert(!stream.is_inter_stream());

      auto [rdev, wdev] = make_socket_pair();

      //LOGI("Created an intra stream for ", key, " fd=", rdev->fd());
      _insert_istream(stream, std::move(rdev));
      _insert_ostream(stream, std::move(wdev));
    }
    // Case 2: inter stream
    else {

      assert(stream.is_inter_stream());

      make_fd_close_on_exec(fitr->second);
      //LOGI("Created an inter stream for ", key, " fd=", fitr->second);
      
      if(stream.is_inter_stream(std::ios_base::in)) {
        _insert_istream(stream, std::make_shared<Socket>(fitr->second));
      }
      else if(stream.is_inter_stream(std::ios_base::out)) {
        _insert_ostream(stream, std::make_shared<Socket>(fitr->second));
      }
      else {
        assert(false);
      }
    }
  }
}

// Procedure: _insert_istream
void Executor::_insert_istream(Stream& stream, std::shared_ptr<Device> idev) {

  if(stream._head->program()) {
    stream._reader = std::move(idev);
    return;
  }

  std::tie(stream._reader, std::ignore) = insert_channel(std::move(idev), std::ios_base::in)(
    [this, &stream] (pb::BrokenIO& b) {
      remove_istream(stream.key);
    },
    [this, &stream] (InputStream& istream) {
      assert(stream._head);
      if(auto r = stream(istream); r == Event::REMOVE) {
        remove_istream(stream.key);
      }
    }
  );
}

// Procedure: _insert_ostream
void Executor::_insert_ostream(Stream& stream, std::shared_ptr<Device> odev) {

  if(stream._tail->program()) {
    stream._writer = std::move(odev);
    return;
  }
        
  if(stream.is_inter_stream(std::ios_base::out)) {
    std::tie(stream._reader, std::ignore) = insert_channel(odev, std::ios_base::in)(
      [this, &stream] (pb::BrokenIO& b) {
        remove_ostream(stream.key);
      }
    );
  }

  std::tie(std::ignore, stream._writer) = insert_channel(std::move(odev), std::ios_base::out)(
    [this, &stream] (pb::BrokenIO& b) {
      remove_ostream(stream.key);
    },
    [this, &stream] (OutputStream& ostream) {
      if(auto r = stream(ostream); r == Event::REMOVE) {
        remove_ostream(stream.key);
      }
    }
  );

}

}  // End of namespace dtc::graph. ----------------------------------------------------


