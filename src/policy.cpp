/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
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
    
// Constructor
// Parse the environment variables.
Policy::Policy() {

  // Configure the policy values from the environments.

  if(auto str = std::getenv("DTC_SOCKET_CONNECTION_TIMEOUT")){
    _SOCKET_CONNECTION_TIMEOUT = std::chrono::seconds(std::stoi(str));
  }

  if(auto str = std::getenv("DTC_FIFO_CONNECTION_TIMEOUT")){
    _FIFO_CONNECTION_TIMEOUT = std::chrono::seconds(std::stoi(str));
  }

  if(auto str = std::getenv("DTC_RESOURCE_PERIODIC")){
    _RESOURCE_PERIODIC = std::chrono::seconds(std::stoi(str));    
  }

	if(auto str = std::getenv("DTC_THIS_HOST")) {
    _THIS_HOST = str;
	}

	if(auto str = std::getenv("DTC_MASTER_HOST")) {
    _MASTER_HOST = str;
	}

  if(auto str = std::getenv("DTC_AGENT_LISTENER_PORT")){
    _AGENT_LISTENER_PORT = str;
  }

  if(auto str = std::getenv("DTC_GRAPH_LISTENER_PORT")){
    _GRAPH_LISTENER_PORT = str;
  }
  
  if(auto str = std::getenv("DTC_SHELL_LISTENER_PORT")){
    _SHELL_LISTENER_PORT = str;
  }
  
  if(auto str = std::getenv("DTC_WEBUI_LISTENER_PORT")){
    _WEBUI_LISTENER_PORT = str;
  }

  if(auto str = std::getenv("DTC_STDOUT_LISTENER_PORT")){
    _STDOUT_LISTENER_PORT = str;
  }
  
  if(auto str = std::getenv("DTC_STDERR_LISTENER_PORT")){
    _STDERR_LISTENER_PORT = str;
  }

  if(auto str = std::getenv("DTC_FRONTIER_LISTENER_PORT")){
    _FRONTIER_LISTENER_PORT = str;
  }

  if(auto str = std::getenv("DTC_EXECUTION_MODE")) {
    if(std::string_view mode(str); mode=="local") {
      _EXECUTION_MODE = LOCAL;  
    }
    else if(mode == "submit") {
      _EXECUTION_MODE = SUBMIT;
    }
    else if(mode == "distributed") {
      _EXECUTION_MODE = DISTRIBUTED;
    }
    else {
      throw std::runtime_error("Invalid execution mode");
    }
  }
  
  if(auto str = std::getenv("DTC_SUBMIT_ARGV")) {
    _SUBMIT_ARGV = str;
  }
  
  if(auto str = std::getenv("DTC_SUBMIT_FILE")) {
    _SUBMIT_FILE = str;
  }
  
  if(auto str = std::getenv("DTC_TOPOLOGY_FD")) {
    _TOPOLOGY_FD = std::stoi(str);
  }
  
  if(auto str = std::getenv("DTC_STDOUT_FD")) {
    _STDOUT_FD = std::stoi(str);
  }
  
  if(auto str = std::getenv("DTC_STDERR_FD")) {
    _STDERR_FD = std::stoi(str);
  }

  if(auto str = std::getenv("DTC_LOG_FILE")) {
    _LOG_FILE = str;
  }
  
  if(auto str = std::getenv("DTC_FRONTIERS")) {
    const static std::regex e("[^\\s:]+"); 
    std::string s(str);
    auto sbeg = std::sregex_token_iterator(s.begin(), s.end(), e);
    auto send = std::sregex_token_iterator();
    assert((std::distance(sbeg, send) & 1) == 0);
    for(auto itr=sbeg; itr!=send;) {
      std::string k = *itr++;
      std::string v = *itr++;
      _FRONTIERS.try_emplace(std::move(k), std::move(v));
    }
  }

  //---------------------------------------
  // Register signal handler.             |
  //---------------------------------------

  // Ignore the SIGCHLD to enable automatica reaping.
  //::signal(SIGCHLD, SIG_IGN);
  ::signal(SIGPIPE, SIG_IGN);
  
  //---------------------------------------
  // Initialize policy-dependent members. |
  //---------------------------------------



  // Logging facilities.
  LOGTO(_LOG_FILE);

  // LevelDB field.
  //std::filesystem::create_directories(_IOSDB_DIR);
  //leveldb::Options options;
  //options.create_if_missing = true;
  //leveldb::Status status = leveldb::DB::Open(options, _IOSDB_DIR, &_iosdb);
  //if(status.ok() != true) {
  //  LOGF("Failed to open iosdb on ", _IOSDB_DIR, " (", status.ToString(), ")");
  //}
}


// Destructor.
Policy::~Policy() {

  // LevelDB field.
  //delete _iosdb;
  //std::filesystem::remove_all(_IOSDB_DIR);
}


};  // End of namespace dtc. ----------------------------------------------------------------------









