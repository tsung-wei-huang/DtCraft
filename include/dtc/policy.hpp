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

#ifndef DTC_POLICY_HPP_
#define DTC_POLICY_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility.hpp>

namespace dtc {

// Class: Policy
// Policy of running various programs in dtc (e.g., timeout values).
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

    inline const std::unordered_map<std::string, std::string>& FRONTIERS() const;
    
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

    std::unordered_map<std::string, std::string> _FRONTIERS;

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

inline const std::unordered_map<std::string, std::string>& Policy::FRONTIERS() const {
	return _FRONTIERS;
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
    
//inline leveldb::DB* Policy::iosdb() const {
//  return _iosdb;
//}
//
//inline std::string Policy::next_iosdb_kbase() const {
//  static std::atomic<long long> counter {0};
//  return std::to_string(++counter) + "-";
//}

inline void Policy::STDOUT_LISTENER_PORT(std::string_view str) {
  ::setenv("DTC_STDOUT_LISTENER_PORT", (_STDOUT_LISTENER_PORT = str).c_str(), 1);
}

inline void Policy::STDERR_LISTENER_PORT(std::string_view str) {
  ::setenv("DTC_STDERR_LISTENER_PORT", (_STDERR_LISTENER_PORT = str).c_str(), 1);
}


};  // End of namespace dtc. ----------------------------------------------------------------------

#endif


