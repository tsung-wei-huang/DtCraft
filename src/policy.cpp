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

#include <dtc/policy.hpp>

namespace dtc {

// Move constructor
Runtime::Runtime(Runtime&& rhs) : _map {std::move(rhs._map)} {
}

// Copy constructor
Runtime::Runtime(const Runtime& rhs) : _map {rhs._map} {
}

// Move operator
Runtime& Runtime::operator = (Runtime&& rhs) {
  _map = std::move(rhs._map);
  return *this;
}

// Copy operator
Runtime& Runtime::operator = (const Runtime& rhs) {
  _map = rhs._map;
  return *this;
}
 
// Move Operator =   
Runtime& Runtime::operator = (std::unordered_map<std::string, std::string>&& rhs) {
  _map = std::move(rhs);
  return *this;
}

// Copy Operator =
Runtime& Runtime::operator = (const std::unordered_map<std::string, std::string>& rhs) {
  _map = rhs;
  return *this;
}

// Function: execution_mode
ExecutionMode Runtime::execution_mode() const {
  if(auto itr = _map.find("DTC_EXECUTION_MODE"); itr != _map.end()) {
    if(itr->second == "local") {
      return ExecutionMode::LOCAL;  
    }
    else if(itr->second == "submit") {
      return ExecutionMode::SUBMIT;
    }
    else if(itr->second == "distributed") {
      return ExecutionMode::DISTRIBUTED;
    }
    else throw std::runtime_error("Invalid execution mode");
  }
  else {
    return ExecutionMode::LOCAL;
  }
}

// Procedure: execution_mode
Runtime& Runtime::execution_mode(ExecutionMode mode) {
  switch(mode) {
    case ExecutionMode::DISTRIBUTED:
      _map["DTC_EXECUTION_MODE"] = "distributed";
    break;

    case ExecutionMode::LOCAL:
      _map["DTC_EXECUTION_MODE"] = "local";
    break;

    case ExecutionMode::SUBMIT:
      _map["DTC_EXECUTION_MODE"] = "submit";
    break;
  }
  return *this;
}

// Procedure: merge
Runtime& Runtime::merge(std::unordered_map<std::string, std::string>&& src) {
  _map.merge(std::move(src));
  return *this;
}

// Procedure: submit_file
Runtime& Runtime::submit_file(std::string str) {
  _map.insert_or_assign("DTC_SUBMIT_FILE", std::move(str));
  return *this;
}

// Function: submit_file
std::string Runtime::submit_file() const {
  if(auto itr = _map.find("DTC_SUBMIT_FILE"); itr != _map.end()) {
    return itr->second; 
  }
  return "";
}

// Procedure: submit_argv
Runtime& Runtime::submit_argv(std::string str) {
  _map.insert_or_assign("DTC_SUBMIT_ARGV", std::move(str));
  return *this;
}

// Function: submit_argv
std::string Runtime::submit_argv() const {
  if(auto itr = _map.find("DTC_SUBMIT_ARGV"); itr != _map.end()) {
    return itr->second; 
  }
  return "";
}

// Procedure: program
Runtime& Runtime::program(std::string str) {
  _map.insert_or_assign("DTC_PROGRAM", std::move(str));
  return *this;
}

// Function: program
std::string Runtime::program() const {
  if(auto itr = _map.find("DTC_PROGRAM"); itr != _map.end()) {
    return itr->second; 
  }
  return "";
}

// Function: this_host
std::string Runtime::this_host() const {
  if(auto itr = _map.find("DTC_THIS_HOST"); itr != _map.end()) {
    return itr->second; 
  }
  return "127.0.0.1";
}

// Function: master_host
std::string Runtime::master_host() const {
  if(auto itr = _map.find("DTC_MASTER_HOST"); itr != _map.end()) {
    return itr->second; 
  }
  return "127.0.0.1";
}

// Procedure: stdout_fd
Runtime& Runtime::stdout_fd(int fd) {
  _map.insert_or_assign("DTC_STDOUT_FD", std::to_string(fd));
  return *this;
}

// Function: stdout_listener_port
std::string Runtime::stdout_listener_port() const {
  if(auto itr = _map.find("DTC_STDOUT_LISTENER_PORT"); itr != _map.end()) {
    return itr->second; 
  }
  return "0";
}

// Procedure: stderr_fd
Runtime& Runtime::stderr_fd(int fd) {
  _map.insert_or_assign("DTC_STDERR_FD", std::to_string(fd));
  return *this;
}

// Function: stderr_listener_port
std::string Runtime::stderr_listener_port() const {
  if(auto itr = _map.find("DTC_STDERR_LISTENER_PORT"); itr != _map.end()) {
    return itr->second; 
  }
  return "0";
}

//// Function: bridges
//std::unordered_map<std::string, int> Runtime::bridges() const {
//
//}

// Procedure: bridges
Runtime& Runtime::bridges(std::string str) {
  _map.insert_or_assign("DTC_BRIDGES", std::move(str));
  return *this;
}

// Procedure: frontiers
Runtime& Runtime::frontiers(std::string str) {
  _map.insert_or_assign("DTC_FRONTIERS", std::move(str));
  return *this;
}

// Procedure: remove_frontiers
Runtime& Runtime::remove_frontiers() {
  _map.erase("DTC_FRONTIERS");
  return *this;
}

// Procedure: topology_fd
Runtime& Runtime::topology_fd(int fd) {
  _map.insert_or_assign("DTC_TOPOLOGY_FD", std::to_string(fd));
  return *this;
}

// Procedure: vertex_fd
Runtime& Runtime::vertex_fd(int fd) {
  _map.insert_or_assign("DTC_VERTEX_FD", std::to_string(fd));
  return *this;
}

// Procedure: remove_topology_fd
Runtime& Runtime::remove_topology_fd() {
  _map.erase("DTC_TOPOLOGY_FD");
  return *this;
}

// Procedure: vertex_hosts
Runtime& Runtime::vertex_hosts(std::string str) {
  _map.insert_or_assign("DTC_VERTEX_HOSTS", std::move(str));
  return *this;
}

// Procedure: remove_vertex_hosts
Runtime& Runtime::remove_vertex_hosts() {
  _map.erase("DTC_VERTEX_HOSTS");
  return *this;
}

// Function: vertex_hosts
std::unordered_map<key_type, std::string> Runtime::vertex_hosts() const {

  std::unordered_map<key_type, std::string> vhosts;

  if(auto itr = _map.find("DTC_VERTEX_HOSTS"); itr != _map.end()) {
    const static std::regex e("[^\\s:]+"); 
    auto sbeg = std::sregex_token_iterator(itr->second.begin(), itr->second.end(), e);
    auto send = std::sregex_token_iterator();
    assert((std::distance(sbeg, send) & 1) == 0);  // must be a pair
    for(auto itr=sbeg; itr!=send;) {
      key_type k = std::stoi(*itr++);
      std::string v = *itr++;
      vhosts.try_emplace(k, std::move(v));
    }
  }

  return vhosts;
}

// Function: frontiers
std::unordered_map<key_type, int> Runtime::frontiers() const {

  std::unordered_map<key_type, int> frontiers;

  if(auto itr = _map.find("DTC_FRONTIERS"); itr != _map.end()) {
    const static std::regex e("[^\\s:]+"); 
    auto sbeg = std::sregex_token_iterator(itr->second.begin(), itr->second.end(), e);
    auto send = std::sregex_token_iterator();
    assert((std::distance(sbeg, send) & 1) == 0);  // must be a pair
    for(auto itr=sbeg; itr!=send;) {
      key_type k = std::stoi(*itr++);
      int fd = std::stoi(*itr++);
      frontiers.try_emplace(k, fd);
    }
  }

  return frontiers;

}

// Function: c_file
std::unique_ptr<char[]> Runtime::c_file() const {
  if(const auto itr = _map.find("DTC_SUBMIT_FILE"); itr != _map.end()) { 
    auto ptr = std::make_unique<char[]>(itr->second.size() + 1);
    std::strcpy(ptr.get(), itr->second.c_str());
    return ptr;
  }
  else return nullptr;
}

// Function: c_argv
std::unique_ptr<char*, std::function<void(char**)>> Runtime::c_argv() const { 
 
  if(const auto p = _map.find("DTC_SUBMIT_ARGV"); p != _map.end()) {
     
    const static std::regex ws_re("\\s+|\\n+|\\t+"); 
    
    auto itr = std::sregex_token_iterator(p->second.begin(), p->second.end(), ws_re, -1);
    auto end = std::sregex_token_iterator();
    auto num = std::distance(itr, end);

    std::unique_ptr<char*, std::function<void(char**)>> ptr(
      new char*[num + 1],
      [sz=num+1] (char** ptr) {
        for(auto i=0; i<sz; ++i) {
          delete [] ptr[i];
        }
        delete [] ptr;
      }
    );

    for(int i=0; i<num; ++i, ++itr) {
      ptr.get()[i] = new char [itr->length() + 1];
      std::strcpy(ptr.get()[i], itr->str().c_str());
    }

    ptr.get()[num] = nullptr;

    return ptr;
  }
  else return nullptr;
}

// Function: c_envp
std::unique_ptr<char*, std::function<void(char**)>> Runtime::c_envp() const {

  std::unique_ptr<char*, std::function<void(char**)>> ptr(
    new char*[_map.size() + 1],
    [sz=_map.size()+1](char** ptr) {
      for(size_t i=0; i<sz; ++i) {
        delete [] ptr[i];
      }
      delete [] ptr;
    }
  );
  
  auto idx = size_t{0};

  for(const auto& [k, v] : _map) {
    auto entry = k + "=" + v;
    ptr.get()[idx] = new char[entry.size() + 1];
    std::strcpy(ptr.get()[idx], entry.c_str());
    ++idx;
  }
  ptr.get()[idx] = nullptr;

  return ptr;
}


};  // end of namespace dtc -----------------------------------------------------------------------


