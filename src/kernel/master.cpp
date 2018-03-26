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

#include <dtc/kernel/master.hpp>

namespace dtc {

// Function: num_agents
// Query the number of the agents.
size_t Master::num_agents() const {
  return _agents.size();
}

// Function: remove_agent
// The public wrapper of the function to remove an egent.
std::future<bool> Master::remove_agent(key_type key) {
  return promise([this, key](){ return _remove_agent(key);});
}

// Function: _remove_agent
bool Master::_remove_agent(key_type key) {

  assert(is_owner());

  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    auto& A = aitr->second;
    for(auto& t : A.taskmeta) {
      _remove_graph(t.second.topology.graph);
    }
    remove(std::move(A.istream), std::move(A.ostream));
    _agents.erase(aitr);
    LOGI("Agent ", key, " is removed from the master");

    return true;
  }

  return false;
}

// Function: insert_agent
// Create an IO communication channel between the master program and the in-coming agent. The
// first message arriving in the master should be the resource information of the agent. 
// The resource information is used for the scheduler to decide the deployment of the graph in 
// iterms of topologies. When a topology is completed by a local executor, a taskinfo as well
// as an updated taskinfo will be sent to the master.
key_type Master::_insert_agent(std::shared_ptr<Socket> socket) {
  
  assert(is_owner());

  // Generate a key.
  static key_type generator {0};
  const key_type key = generator++;

  auto& agent = _agents.try_emplace(key, key).first->second;

  std::tie(agent.istream, agent.ostream) = insert_channel(std::move(socket))(
    [this, key] (pb::BrokenIO& b) { remove_agent(key); },
    [this, key] (pb::Resource& r) { on_resource(key, std::move(r)); },
    [this, key] (pb::TaskInfo& i) { on_taskinfo(key, std::move(i)); }
  );
  
  return key;
}

// Function: insert_agent
std::future<key_type> Master::insert_agent(std::shared_ptr<Socket> socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_agent(std::move(socket)); });
}

//-------------------------------------------------------------------------------------------------

// Function: num_graphs
// Query the number of the graphs.
size_t Master::num_graphs() const {
  return _graphs.size();
}

// Function: _remove_graph
bool Master::_remove_graph(key_type key) {

  assert(is_owner());

  if(auto gitr = _graphs.find(key); gitr != _graphs.end()) {

    auto& G = gitr->second;

    // Shut down all remaining tasks
    for(const auto& [tid, meta] : G.taskmeta) {
      if(auto aitr = _agents.find(meta.agent); aitr != _agents.end()) {
        LOGI("Ask agent to kill the task ", tid.to_string());
        (*aitr->second.ostream)(pb::Protobuf(pb::KillTask{tid}));
      }
    }

    (*G.ostream)(pb::Protobuf{std::move(*G.solution)});

    remove(std::move(G.istream), std::move(G.ostream));
    _graphs.erase(gitr);
    LOGI("Graph ", key, " is removed from the master");
    return true;
  }

  return false;
}

// Function: remove_graph
std::future<bool> Master::remove_graph(key_type key) {
  return promise([this, key] () mutable { return _remove_graph(key); });
}

// Function: _insert_graph
key_type Master::_insert_graph(std::shared_ptr<Socket> socket) {

  // Create a pseudo uuid for the graph.
  static key_type generator {0};
  const key_type key = generator++;

  auto& graph = _graphs.try_emplace(key, key).first->second;

  std::tie(graph.istream, graph.ostream) = insert_channel(std::move(socket))(
    [this, key] (pb::BrokenIO& b) { remove_graph(key); },
    [this, key] (pb::Topology& t) { on_topology(key, std::move(t)); }
  );

  return key;
}

// Function: insert_graph
std::future<key_type> Master::insert_graph(std::shared_ptr<Socket> socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_graph(std::move(socket)); });
}

//-------------------------------------------------------------------------------------------------

// Constructor
Master::Master() : KernelBase {env::master_num_threads()} {

  insert_listener(env::agent_listener_port())(
    [this] (std::shared_ptr<Socket> skt) { insert_agent(std::move(skt)); }
  );

  insert_listener(env::graph_listener_port())(
    [this] (std::shared_ptr<Socket> skt) { insert_graph(std::move(skt)); }
  );
  
  insert_listener(env::webui_listener_port())(
    [this] (std::shared_ptr<Socket> skt) { insert_webui(std::move(skt)); }
  );
  
  // Logging
  LOGI(
    "Master @", env::this_host(), " ", 
    "[agent:",  env::agent_listener_port(), 
    "|graph:",  env::graph_listener_port(), 
    "|webui:",  env::webui_listener_port(), "]"
  );
};

// Destructor
Master::~Master() {
}

// Function: _on_taskinfo
void Master::_on_taskinfo(key_type key, pb::TaskInfo& info) {

  assert(is_owner());
      
  // Update the agent data structure accordingly.
  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    auto& A = aitr->second;
    if(auto titr = A.taskmeta.find(info.task_id); titr != A.taskmeta.end()) {
      LOGI(info);
      *A.released += titr->second.topology.resource();
      A.taskmeta.erase(titr);
    }
  }

  // Update the graph data structure accordingly.
  if(auto gitr = _graphs.find(info.task_id.graph); gitr != _graphs.end()) {

    auto& G = gitr->second;

    G.taskmeta.erase(info.task_id);
    G.solution->taskinfos.emplace_back(info);

    if(info.has_error() || G.taskmeta.size() == 0) {
      _remove_graph(info.task_id.graph);
    }
  }

  // Reinvoke the scheduler
  _dequeue();
}

// Function: on_taskinfo
// The public wrapper of the update topology function.
std::future<void> Master::on_taskinfo(key_type key, pb::TaskInfo&& info) {
  return promise(
    [this, key, info=std::move(info)] () mutable { _on_taskinfo(key, info); }
  );
}

// Function: _on_resource
// The private function to update the resource.
void Master::_on_resource(key_type key, pb::Resource& res) {

  assert(is_owner());

  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    aitr->second.resource = res;
    aitr->second.released = res;
    LOGI("Agent ", key, " connected ", res);
  }

  _dequeue();
}

// Function: on_resource
// The public wrapper of the update resource function.
std::future<void> Master::on_resource(key_type key, pb::Resource&& res) {
  return promise(
    [this, key, res=std::move(res)] () mutable { _on_resource(key, res); }
  );
}

// Function: _on_topology
// The private function to update the topology.
void Master::_on_topology(key_type key, pb::Topology& tpg) {

  assert(is_owner());

  if(auto gitr = _graphs.find(key); gitr != _graphs.end()) {

    tpg.graph = key;

    auto& G = gitr->second;

    G.topology = std::move(tpg);
    G.solution = pb::Solution {key};

    LOGI("Graph ", key, " connected ", (*G.topology).to_string());
    
    if(!_enqueue(G)) {
      LOGW("Graph ", key, " doesn't fit with available resources");
      G.solution->what = "cluster does not have enough resources";
      _remove_graph(key);
      return;
    }
  
    // TODO deque
    _dequeue();
  }
}

// Function: on_topology
// The public wrapper of the update topology function.
std::future<void> Master::on_topology(key_type key, pb::Topology&& tpg) {
  return promise(
    [this, key, tpg=std::move(tpg)] () mutable { _on_topology(key, tpg); }
  );
}

// Function: _enqueue
bool Master::_enqueue(Graph& G) {

  assert(is_owner());

  if(!_try_enqueue(G)) {
    return false;
  }
  
  _queue.push(G.key);
  LOGI("Enqueue graph ", G.key, " (queue size=", _queue.size(), ")");

  return true;
}

// Function: _dequeue
size_t Master::_dequeue() {
  
  assert(is_owner());
  
  size_t num_dequeued {0};

  while(!_queue.empty()) {
    
    auto key = _queue.front();

    if(auto gitr = _graphs.find(key); gitr != _graphs.end()) {
      if(!_try_dequeue(gitr->second)) {
        break;
      }
    }

    _queue.pop();
    ++num_dequeued;
  }
  
  return num_dequeued;
}

// ---- Webui field -------------------------------------------------------------------------------

json Master::MasterInfo::to_json() const {
  return {
    {"host", host},
    {"num_agents", num_agents},
    {"num_graphs", num_graphs}
  };
}

json Master::AgentInfo::to_json() const {
  return {
    {"host", resource.host},
    {"resource", {{"num_cpus", resource.num_cpus}, 
                  {"memory_limit_in_bytes", resource.memory_limit_in_bytes},
                  {"space_limit_in_bytes", resource.space_limit_in_bytes}}},
    {"released", {{"num_cpus", released.num_cpus}, 
                  {"memory_limit_in_bytes", released.memory_limit_in_bytes},
                  {"space_limit_in_bytes", released.space_limit_in_bytes}}},
    {"num_tasks", num_tasks},
  };
}

// Function: _cluster_info
Master::ClusterInfo Master::_cluster_info() const {
  
  assert(is_owner());

  ClusterInfo c;

  c.master = MasterInfo { env::this_host(), _agents.size(), _graphs.size()};

  for(const auto& [k, a] : _agents) {
    if(a.resource) {
      c.agents.emplace_back( AgentInfo{k, *a.resource, *a.released, a.taskmeta.size()} );
    }
  }

  return c;
}

// Function: agent_infos
std::future<Master::ClusterInfo> Master::cluster_info() {
  return promise([&] () { return _cluster_info(); });
}

// Function:
json Master::ClusterInfo::to_json() const {

  // Fill in the master info
  auto jsonp = json::array();  // json with padding
  for(const auto& a : agents) {
    jsonp.emplace_back(a.to_json());
  }
  
  return {
    {"master", master.to_json()},
    {"agents", jsonp}
  };
}

// Function: num_webuis
size_t Master::num_webuis() const {
  return _webuis.size();
}

// Function: insert_webui
key_type Master::_insert_webui(std::shared_ptr<Socket> socket) {
  
  assert(is_owner());

  // Generate a key.
  static key_type generator {0};
  const key_type key = generator++;

  auto& webui = _webuis.try_emplace(key, key).first->second;

  std::tie(webui.istream, webui.ostream) = insert_channel(std::move(socket))(
    [this, key] (pb::BrokenIO& b) -> void { 
      remove_webui(key); 
    },
    [this, key, P=HttpRequestParser()] (InputStream& is) mutable -> void {
      auto B = P(is.isbuf.string_view());
      P.on(
        [this, fd=is.device()->fd()](HttpRequest& req) {
          try {
            send_response(fd, make_response(req));
          }
          catch (const std::system_error& se) {
            LOGE("Failed to send response! ", se.code());
          }
        }
      );
      is.isbuf.drop(B);
    }
  );

  return key;
}

// Function: insert_webui
std::future<key_type> Master::insert_webui(std::shared_ptr<Socket> socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_webui(std::move(socket)); });
}

// Function: _remove_webui
bool Master::_remove_webui(key_type key) {

  assert(is_owner());

  if(auto aitr = _webuis.find(key); aitr != _webuis.end()) {
    auto& webui = aitr->second;
    remove(std::move(webui.istream), std::move(webui.ostream));
    _webuis.erase(aitr);
    LOGI("WebUI ", key, " is removed from the master");
    return true;
  }

  return false;
}


// Function: remove_webui
// The public wrapper of the function to remove a webui
std::future<bool> Master::remove_webui(key_type key) {
  return promise([this, key](){ return _remove_webui(key);});
}

// Function:
std::string Master::agent_to_json(std::string_view query){

  const auto info = cluster_info().get();
  
  // TODO: jsonp instead of '='?
  return std::string {query.substr(query.find("=")+1)} + "(" + info.to_json().dump() + ")";
}

// Function: make_response
HttpResponse Master::make_response(const HttpRequest& req) {

  auto [query, path] = req.url.extract(URL::QUERY, URL::PATH);

  switch(req.body_type()){
    
    case HttpBodyType::QUERY: {
      //LOGI("JSON: URL=", req.url);
      static const std::regex rgx(".*/(\\w+)\\?+.*");
      if(std::smatch match; std::regex_search(req.url.begin(), req.url.end(), match, rgx)){
        if(match[1] == "cluster") {
          return HttpResponse { HttpStatusCode::OK, HttpBodyType::QUERY, ".json", agent_to_json(query), req.keep_alive };
        }
      }
    }
    break;

    case HttpBodyType::FILE: {
      auto p = env::webui_dir();
      //LOGI("FILE: URL_PATH is ", path);
      p /= (path == "/") ? "index.html" : path;
      //std::cout << "examining path " << p << std::endl;
      if( std::filesystem::is_regular_file(p) ){
        return HttpResponse{ 
          HttpStatusCode::OK, HttpBodyType::FILE, p.extension().c_str(), p.c_str(), req.keep_alive 
        };
      }
    }
    break;
    
    default:
    break;

  };

  return HttpResponse{HttpStatusCode::NOT_FOUND, HttpBodyType::UNDEFINED, "", "", false};
}


};  // End of namespace dtc. ----------------------------------------------------------------------








