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

#ifndef DTC_POLICY_HPP_
#define DTC_POLICY_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility/utility.hpp>

/*namespace dtc {

class Policy final : public EnableSingletonFromThis<Policy> {

  friend EnableSingletonFromThis<Policy>;

  public:

  enum ExecutionMode {
    LOCAL = 0,
    SUBMIT,
    DISTRIBUTED
  };

    Policy();
    ~Policy();

    inline const std::chrono::seconds& SOCKET_CONNECTION_TIMEOUT() const;
    inline const std::chrono::seconds& FIFO_CONNECTION_TIMEOUT() const;
    inline const std::chrono::seconds& RESOURCE_PERIODIC() const;
    
    inline const auto& THIS_HOST() const;
    inline const auto& MASTER_HOST() const;
    inline const auto& AGENT_LISTENER_PORT() const;
    inline const auto& GRAPH_LISTENER_PORT() const;
    inline const auto& SHELL_LISTENER_PORT() const;
    inline const auto& WEBUI_LISTENER_PORT() const;
    inline const auto& STDOUT_LISTENER_PORT() const;
    inline const auto& STDERR_LISTENER_PORT() const;
    inline const auto& FRONTIER_LISTENER_PORT() const;
    inline const auto& SUBMIT_ARGV() const;
    inline const auto& SUBMIT_FILE() const;
    
    inline ExecutionMode EXECUTION_MODE() const;
    
    inline void STDOUT_LISTENER_PORT(std::string_view);
    inline void STDERR_LISTENER_PORT(std::string_view);

    inline const std::filesystem::path& WEBUI_DIR() const;
    inline const std::filesystem::path& CGROUP_MOUNT() const;
    inline const std::filesystem::path& LOG_FILE() const;
    inline const std::filesystem::path& WORKSPACE_DIR() const;

    inline auto TOPOLOGY_FD() const;
    inline auto STDOUT_FD() const;
    inline auto STDERR_FD() const;
    inline unsigned AGENT_NUM_THREADS() const; 
    inline unsigned MASTER_NUM_THREADS() const; 
    inline unsigned EXECUTOR_NUM_THREADS() const;

  private:

    std::chrono::seconds _SOCKET_CONNECTION_TIMEOUT {10};
    std::chrono::seconds _FIFO_CONNECTION_TIMEOUT {10};
    std::chrono::seconds _RESOURCE_PERIODIC {10};

    std::string _THIS_HOST {"127.0.0.1"};
    std::string _MASTER_HOST {"127.0.0.1"};
    std::string _AGENT_LISTENER_PORT {"9909"};
    std::string _GRAPH_LISTENER_PORT {"9910"};
    std::string _SHELL_LISTENER_PORT {"9911"};
    std::string _WEBUI_LISTENER_PORT {"9912"};
    std::string _STDOUT_LISTENER_PORT {"0"};
    std::string _STDERR_LISTENER_PORT {"0"};
    std::string _FRONTIER_LISTENER_PORT {"9914"};

    std::string _SUBMIT_ARGV;
    std::string _SUBMIT_FILE;
    
    ExecutionMode _EXECUTION_MODE {LOCAL};

    unsigned _AGENT_NUM_THREADS {std::thread::hardware_concurrency()};
    unsigned _EXECUTOR_NUM_THREADS {std::thread::hardware_concurrency()};
    unsigned _MASTER_NUM_THREADS {std::thread::hardware_concurrency()};

    const std::filesystem::path _WEBUI_DIR {DTC_HOME "/webui"};
    const std::filesystem::path _CGROUP_MOUNT {"dtc"};
    const std::filesystem::path _WORKSPACE_DIR {std::filesystem::temp_directory_path()};

    std::filesystem::path _LOG_FILE {""};

    int _TOPOLOGY_FD {-1};
    int _STDOUT_FD {STDOUT_FILENO};
    int _STDERR_FD {STDERR_FILENO};
};

inline const std::chrono::seconds& Policy::SOCKET_CONNECTION_TIMEOUT() const {
  return _SOCKET_CONNECTION_TIMEOUT;
}

inline const std::chrono::seconds& Policy::FIFO_CONNECTION_TIMEOUT() const {
  return _FIFO_CONNECTION_TIMEOUT;
}

inline const std::chrono::seconds& Policy::RESOURCE_PERIODIC() const {
  return _RESOURCE_PERIODIC;
}

inline const auto& Policy::THIS_HOST() const {
  return _THIS_HOST;
}

inline const auto& Policy::MASTER_HOST() const {
  return _MASTER_HOST;
}

inline const auto& Policy::AGENT_LISTENER_PORT() const {
  return _AGENT_LISTENER_PORT;
}

inline const auto& Policy::GRAPH_LISTENER_PORT() const {
  return _GRAPH_LISTENER_PORT;
}

inline const auto& Policy::STDOUT_LISTENER_PORT() const {
  return _STDOUT_LISTENER_PORT;
}

inline const auto& Policy::STDERR_LISTENER_PORT() const {
  return _STDERR_LISTENER_PORT;
}

inline const auto& Policy::FRONTIER_LISTENER_PORT() const {
  return _FRONTIER_LISTENER_PORT;
}

inline const auto& Policy::SHELL_LISTENER_PORT() const {
  return _SHELL_LISTENER_PORT;
}

inline const auto& Policy::WEBUI_LISTENER_PORT() const {
  return _WEBUI_LISTENER_PORT;
}

inline const auto& Policy::SUBMIT_ARGV() const {
  return _SUBMIT_ARGV;
}

inline const auto& Policy::SUBMIT_FILE() const {
  return _SUBMIT_FILE;
}

inline Policy::ExecutionMode Policy::EXECUTION_MODE() const {
  return _EXECUTION_MODE;
}

inline const std::filesystem::path& Policy::WEBUI_DIR() const {
  return _WEBUI_DIR;
}

inline const std::filesystem::path& Policy::CGROUP_MOUNT() const {
  return _CGROUP_MOUNT;
}

inline const std::filesystem::path& Policy::WORKSPACE_DIR() const {
  return _WORKSPACE_DIR;
}

//inline const std::filesystem::path& Policy::IOSDB_DIR() const {
//  return _IOSDB_DIR;
//}

inline const std::filesystem::path& Policy::LOG_FILE() const {
  return _LOG_FILE;
}

inline auto Policy::TOPOLOGY_FD() const {
  return _TOPOLOGY_FD;
}

inline auto Policy::STDOUT_FD() const {
  return _STDOUT_FD;
}

inline auto Policy::STDERR_FD() const {
  return _STDERR_FD;
}

inline unsigned Policy::AGENT_NUM_THREADS() const {
  return _AGENT_NUM_THREADS;
}

inline unsigned Policy::MASTER_NUM_THREADS() const {
  return _MASTER_NUM_THREADS;
}

inline unsigned Policy::EXECUTOR_NUM_THREADS() const {
  return _EXECUTOR_NUM_THREADS;
}
    
inline void Policy::STDOUT_LISTENER_PORT(std::string_view str) {
  ::setenv("DTC_STDOUT_LISTENER_PORT", (_STDOUT_LISTENER_PORT = str).c_str(), 1);
}

inline void Policy::STDERR_LISTENER_PORT(std::string_view str) {
  ::setenv("DTC_STDERR_LISTENER_PORT", (_STDERR_LISTENER_PORT = str).c_str(), 1);
}

};  // End of namespace dtc. ---------------------------------------------------------------------- 
*/

namespace dtc {

enum class ExecutionMode {
  SUBMIT,
  DISTRIBUTED,
  LOCAL 
};

// Class: Runtime
class Runtime {

  public:

    Runtime() = default;
    Runtime(Runtime&&);
    Runtime(const Runtime&);

    ExecutionMode execution_mode() const;

    std::unordered_map<key_type, std::string> vertex_hosts() const;
    std::unordered_map<key_type, int> frontiers() const;
    std::unordered_map<std::string, int> bridges() const;
 
    std::unique_ptr<char[]> c_file() const;
    std::unique_ptr<char*, std::function<void(char**)>> c_argv() const;
    std::unique_ptr<char*, std::function<void(char**)>> c_envp() const;  
    
    Runtime& execution_mode(ExecutionMode);
    Runtime& vertex_hosts(std::string);
    Runtime& bridges(std::string);
    Runtime& frontiers(std::string);
    Runtime& submit_file(std::string);
    Runtime& submit_argv(std::string);
    Runtime& program(std::string);
    Runtime& merge(std::unordered_map<std::string, std::string>&&);
    Runtime& topology_fd(int);
    Runtime& vertex_fd(int);
    Runtime& stdout_fd(int);
    Runtime& stderr_fd(int);
    Runtime& remove_vertex_hosts();
    Runtime& remove_topology_fd();
    Runtime& remove_frontiers();

    std::string master_host() const;
    std::string this_host() const;
    std::string submit_file() const;
    std::string submit_argv() const;
    std::string program() const;
    std::string stdout_listener_port() const;
    std::string stderr_listener_port() const;

    Runtime& operator = (Runtime&&);
    Runtime& operator = (const Runtime&);

    Runtime& operator = (std::unordered_map<std::string, std::string>&&);
    Runtime& operator = (const std::unordered_map<std::string, std::string>&);
    
    template <typename ArchiverT>
    auto archive(ArchiverT&);

  private:

    std::unordered_map<std::string, std::string> _map;
};

// Function: archive
template <typename ArchiverT>
auto Runtime::archive(ArchiverT& ar) {
  return ar(_map);
}


};  // End of namespace dtc. ---------------------------------------------------------------------- 


namespace dtc::env {

inline std::string this_host() {
  if(auto str = std::getenv("DTC_THIS_HOST")) {
    return str;
  }
  else return "127.0.0.1";
}

inline std::string master_host() {
  if(auto str = std::getenv("DTC_MASTER_HOST")) {
    return str;
  }
  else return "127.0.0.1";
}

inline std::string agent_listener_port() {
  if(auto str = std::getenv("DTC_AGENT_LISTENER_PORT")){
    return str;
  }
  else return "9909";
}

inline std::string graph_listener_port() {
  if(auto str = std::getenv("DTC_GRAPH_LISTENER_PORT")){
    return str;
  }
  else return "9910";
}

inline std::string stdout_listener_port() {
  if(auto str = std::getenv("DTC_STDOUT_LISTENER_PORT")){
    return str;
  }
  else return "0";
}

inline std::string stderr_listener_port() {
  if(auto str = std::getenv("DTC_STDERR_LISTENER_PORT")){
    return str;
  }
  else return "0";
}

inline std::string frontier_listener_port() {
  if(auto str = std::getenv("DTC_FRONTIER_LISTENER_PORT")){
    return str;
  }
  else return "9913";
}

inline std::string shell_listener_port() {
  if(auto str = std::getenv("DTC_SHELL_LISTENER_PORT")){
    return str;
  }
  else return "9911";
}

inline std::string webui_listener_port() {
  if(auto str = std::getenv("DTC_WEBUI_LISTENER_PORT")){
    return str;
  }
  else return "9912";
}

inline std::string submit_argv() {
  if(auto str = std::getenv("DTC_SUBMIT_ARGV")) {
    return str;
  }
  else return "";
}

inline std::string submit_file() {
  if(auto str = std::getenv("DTC_SUBMIT_FILE")) {
    return str;
  }
  else return "";
}

inline ExecutionMode execution_mode() {
  if(std::string_view mode(std::getenv("DTC_EXECUTION_MODE")); !mode.empty()) {
    if(mode == "local") {
      return ExecutionMode::LOCAL;  
    }
    else if(mode == "submit") {
      return ExecutionMode::SUBMIT;
    }
    else if(mode == "distributed") {
      return ExecutionMode::DISTRIBUTED;
    }
    else throw std::runtime_error("Invalid execution mode");
  }
  else return ExecutionMode::LOCAL;
}

inline std::filesystem::path webui_dir() {
  return DTC_HOME "/webui";
}

inline std::filesystem::path log_file() {
  if(auto str = std::getenv("DTC_LOG_FILE")) {
    return str;
  }
  else return "";
}

inline int topology_fd() {
  if(auto str = std::getenv("DTC_TOPOLOGY_FD")) {
    return std::stoi(str);
  }
  else return -1;
}

inline int stdout_fd() {
  if(auto str = std::getenv("DTC_STDOUT_FD")) {
    return std::stoi(str);
  }
  else return STDOUT_FILENO;
}

inline int stderr_fd() {
  if(auto str = std::getenv("DTC_STDERR_FD")) {
    return std::stoi(str);
  }
  else return STDERR_FILENO;
}

inline unsigned master_num_threads() {
  if(auto str = std::getenv("DTC_MASTER_NUM_THREADS"); str) {
    return std::stoul(str);
  }
  return std::thread::hardware_concurrency();
}

inline unsigned agent_num_threads() {
  if(auto str = std::getenv("DTC_AGENT_NUM_THREADS"); str) {
    return std::stoul(str);
  }
  return std::thread::hardware_concurrency();
}

inline unsigned executor_num_threads() {
  if(auto str = std::getenv("DTC_EXECUTOR_NUM_THREADS"); str) {
    return std::stoul(str);
  }
  return std::thread::hardware_concurrency();
}

inline void stdout_listener_port(std::string_view str) {
  ::setenv("DTC_STDOUT_LISTENER_PORT", str.data(), 1);
}

inline void stderr_listener_port(std::string_view str) {
  ::setenv("DTC_STDERR_LISTENER_PORT", str.data(), 1);
}

inline std::filesystem::path cgroup_mount() {
  if(auto str = std::getenv("DTC_CGROUP_MOUNT"); str) {
    return str;
  }
  return "dtc";
}

inline std::filesystem::path agent_cgroup() {
  if(auto str = std::getenv("DTC_AGENT_CGROUP"); str) {
    return str;
  }
  return "dtc/agent";
}

};  // End of namespace dtc::env ------------------------------------------------------------------



#endif






