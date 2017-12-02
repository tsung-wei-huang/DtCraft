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
  return promise([M=this, key](){ return M->_remove_agent(key);});
}

// Function: _remove_agent
bool Master::_remove_agent(key_type key) {

  assert(is_owner());

  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    auto& A = aitr->second;
    for(auto& t : A.taskmeta) {
      _remove_graph(t.second.topology.graph, false);
    }
    _remove(std::move(A.istream));
    _remove(std::move(A.ostream));
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
key_type Master::_insert_agent(std::shared_ptr<Socket>& socket) {
  
  assert(is_owner());

  // Generate a key.
  static key_type generator {0};
  const key_type key = generator++;

  _agents.try_emplace(
    key,
    _insert_actor<Agent>(socket, key)(
      [M=this, key] (const std::error_code& errc) { M->remove_agent(key); },
      [M=this, key] (pb::Resource& res) { M->on_resource(key, std::move(res)); },
      [M=this, key] (pb::TaskInfo& info) { M->on_taskinfo(key, std::move(info)); }
    )
  );

  return key;
}

// Function: insert_agent
std::future<key_type> Master::insert_agent(std::shared_ptr<Socket>&& socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_agent(socket); });
}


//-------------------------------------------------------------------------------------------------

// Function: num_graphs
// Query the number of the graphs.
size_t Master::num_graphs() const {
  return _graphs.size();
}

// Function: _remove_graph
bool Master::_remove_graph(key_type key, bool report) {

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

    if(report) {
      (*G.ostream)(pb::Protobuf{std::move(*G.solution)});
    }

    _remove(std::move(G.istream));
    _remove(std::move(G.ostream));
    _graphs.erase(gitr);
    LOGI("Graph ", key, " is removed from the master");
    return true;
  }

  return false;
}

// Function: remove_graph
std::future<bool> Master::remove_graph(key_type key, bool report) {
  return promise([M=this, key, report] () mutable { return M->_remove_graph(key, report); });
}

// Function: _insert_graph
// Create an IO communication for an incoming connection from the user. Once connected, the
// first protobuf arrives should be the graph in the form of a topology defined by the user.
// We assign a uuid of type key_type to each arrived graph. The uuid is also used for indexing
// the graph through the dictionary that stores the mapping between uuid and the data structure
// to store the graph.
key_type Master::_insert_graph(std::shared_ptr<Socket>& socket) {

  // Create a pseudo uuid for the graph.
  static key_type generator {0};
  const key_type key = generator++;

  _graphs.try_emplace(
    key,
    _insert_actor<Graph>(socket, key) (
      [M=this, key] (const std::error_code& errc) { M->remove_graph(key, false); },
      [M=this, key] (pb::Topology& tpg) { M->on_topology(key, std::move(tpg)); }
    )  
  );

  return key;
}

// Function: insert_graph
std::future<key_type> Master::insert_graph(std::shared_ptr<Socket>&& socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_graph(socket); });
}

//-------------------------------------------------------------------------------------------------

// Constructor
Master::Master() : KernelBase {Policy::get().MASTER_NUM_THREADS()} {

  _insert_listener(Policy::get().AGENT_LISTENER_PORT())(
    [M=this] (std::shared_ptr<Socket>&& skt) { M->insert_agent(std::move(skt)); }
  );

  _insert_listener(Policy::get().GRAPH_LISTENER_PORT())(
    [M=this] (std::shared_ptr<Socket>&& skt) { M->insert_graph(std::move(skt)); }
  );
  
  _insert_listener(Policy::get().WEBUI_LISTENER_PORT())(
    [M=this] (std::shared_ptr<Socket>&& skt) { M->insert_webui(std::move(skt)); }
  );
  
  // Logging
  LOGI(
    "Master @", Policy::get().THIS_HOST(), " ", 
    "[agent:", Policy::get().AGENT_LISTENER_PORT(), 
    "|graph:", Policy::get().GRAPH_LISTENER_PORT(), 
    "|webui:", Policy::get().WEBUI_LISTENER_PORT(), "]"
  );
};

// Destructor
Master::~Master() {
}

// Function: _deploy
bool Master::_deploy(Graph& G) {

  assert(is_owner());
  assert(G.taskmeta.size() == 0 && G.topology);

  // Fetch resource bins from agents at this moment.
  std::vector<Bin> bins;
  for(auto& [K, A] : _agents) {
    if(A.resource) {
      bins.emplace_back(Bin{K, *A.released});
    }
  }
  
  // Invoke the scheduler. 
  auto packing = best_fit_bin_packing(*G.topology, std::move(bins));

  // Case 1: not able to deploy the graph at this moment.
  if(packing.empty()) {
    LOGI("Not able to deploy Graph ", G.key, " at this moment");
    return false;
  }
  
  // Case 2: Deploy topologies to corresponding agents based on the packing result.
  for(auto& [agent, topology] : packing) {

    const auto T = topology.task_id();
    LOGI("Topology ", T, " is scheduled to agent @", _agents.at(agent).resource->host);
    std::cout << topology << std::endl;

    // Send the task to the agent and assign the task meta.
    auto& A = _agents.at(agent);
    (*A.released) -= topology.resource();
    (*A.ostream)(pb::Protobuf(topology));
    A.taskmeta.try_emplace(T, Agent::TaskMeta{std::move(topology)});

    // Assign the meta data for the graph.
    G.taskmeta.try_emplace(T, Graph::TaskMeta{agent});
  }
  
  return true;
}

// Function: _on_taskinfo
void Master::_on_taskinfo(key_type key, pb::TaskInfo& info) {

  assert(is_owner());
      
  // Update the agent data structure accordingly.
  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    auto& A = aitr->second;
    if(auto titr = A.taskmeta.find(info.task_id); titr != A.taskmeta.end()) {
      LOGI("Topology ", info.task_id, " is finished @", A.resource->host);
      *A.released += titr->second.topology.resource();
      A.taskmeta.erase(titr);
    }
  }

  // Update the graph data structure accordingly.
  if(auto gitr = _graphs.find(info.task_id.graph); gitr != _graphs.end()) {

    auto& G = gitr->second;

    G.taskmeta.erase(info.task_id);
    (*G.solution).taskinfos.emplace_back(info);

    if(info.has_error() || G.taskmeta.size() == 0) {
      _remove_graph(info.task_id.graph, true);
    }
  }

  // Reinvoke the scheduler
  _dequeue();
}

// Function: on_taskinfo
// The public wrapper of the update topology function.
std::future<void> Master::on_taskinfo(key_type key, pb::TaskInfo&& info) {
  return promise(
    [M=this, key, info=std::move(info)] () mutable { 
      M->_on_taskinfo(key, info); 
    }
  );
}

// Function: _on_resource
// The private function to update the resource.
void Master::_on_resource(key_type key, pb::Resource& res) {

  assert(is_owner());

  if(auto aitr = _agents.find(key); aitr != _agents.end()) {
    aitr->second.resource = std::move(res);
    aitr->second.released = aitr->second.resource;
    LOGI("Agent ", key, " connected ", *(aitr->second.resource));
  }

  _dequeue();
}

// Function: on_resource
// The public wrapper of the update resource function.
std::future<void> Master::on_resource(key_type key, pb::Resource&& res) {
  return promise(
    [M=this, key, res=std::move(res)] () mutable { M->_on_resource(key, res); }
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
    LOGI("Graph ", G.key, " connected ", (*G.topology).to_string());
    
    if(!_enqueue(G)) {
      LOGW("Graph ", key, " doesn't fit with available resources");
      (*G.solution).errc = make_posix_error_code(EINVAL);
      _remove_graph(key, true);
    }
  }
    
  // TODO deque
  _dequeue();
}

// Function: _enqueue
bool Master::_enqueue(const Graph& G) {

  assert(is_owner());

  std::vector<Bin> bins;
   
  // Fetch the maximum resource.
  for(auto& [K, A] : _agents) {
    if(A.resource) {
      bins.emplace_back(Bin{K, *A.resource});
    }
  }
  
  // Graph cannot be scheduled with the maximum resources.
  if(auto ret = best_fit_bin_packing(*G.topology, std::move(bins)); ret.empty()) {
    return false;
  }
  
  LOGI("Enqueue Graph ", G.key, " into the queue");
  _queue.push(G.key);

  return true;
}

// Function: _dequeue
size_t Master::_dequeue() {
  
  assert(is_owner());
  
  size_t num_scheduled {0};

  while(!_queue.empty()) {
    
    auto key = _queue.front();

    if(auto gitr = _graphs.find(key); gitr != _graphs.end()) {
      if(!_deploy(gitr->second)) {
        break;
      }
    }

    _queue.pop();
    ++num_scheduled;
  }
  
  return num_scheduled;
}

// Function: on_topology
// The public wrapper of the update topology function.
std::future<void> Master::on_topology(key_type key, pb::Topology&& tpg) {
  return promise(
    [M=this, key, tpg=std::move(tpg)] () mutable { M->_on_topology(key, tpg); }
  );
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

  c.master = MasterInfo { Policy::get().THIS_HOST(), _agents.size(), _graphs.size()};

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
key_type Master::_insert_webui(std::shared_ptr<Socket>& socket) {
  
  assert(is_owner());

  // Generate a key.
  static key_type generator {0};
  const key_type key = generator++;

  _webuis.try_emplace(
    key,
    _insert_actor<WebUI>(socket, key)(
      [M=this, key] (const std::error_code& errc) { 
        LOGI("received webui error ", errc);
        M->remove_webui(key); 
      },
      [M=this, key, fd=socket->fd(), P=HttpRequestParser()] (InputStream& is) mutable {
        auto B = P(is.isbuf.string_view());
        P.on(
          [M, fd](HttpRequest& req) {
            try {
              send_response(fd, M->make_response(req));
            }
            catch (const std::system_error& se) {
              LOGE("Failed to send response! ", se.code());
            }
          }
        );
        is.isbuf.drop(B);
      }
    )
  );

  return key;
}

// Function: insert_webui
std::future<key_type> Master::insert_webui(std::shared_ptr<Socket>&& socket) {
  return promise([&, socket=std::move(socket)] () mutable { return _insert_webui(socket); });
}

// Function: _remove_webui
bool Master::_remove_webui(key_type key) {

  assert(is_owner());

  if(auto aitr = _webuis.find(key); aitr != _webuis.end()) {
    auto& W = aitr->second;
    _remove(std::move(W.istream));
    _remove(std::move(W.ostream));
    _webuis.erase(aitr);
    LOGI("WebUI ", key, " is removed from the master");
    return true;
  }

  return false;
}


// Function: remove_webui
// The public wrapper of the function to remove a webui
std::future<bool> Master::remove_webui(key_type key) {
  return promise([M=this, key](){ return M->_remove_webui(key);});
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
      auto p = Policy::get().WEBUI_DIR();
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







