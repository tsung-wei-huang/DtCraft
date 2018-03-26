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

#ifndef DTC_KERNEL_CONTAINER_HPP_
#define DTC_KERNEL_CONTAINER_HPP_

#include <dtc/kernel/cgroup.hpp>
#include <dtc/ipc/ipc.hpp>
#include <dtc/protobuf/protobuf.hpp>

namespace dtc {

// Class: Container
class Container {

  friend class Agent;

  struct ChildArgument {
    const pb::Topology& topology;
    std::shared_ptr<Socket> sync[2];
  };

  public:
    
    Container(const std::filesystem::path&);
    Container(const Container&) = delete;
    Container(Container&&);
    
    ~Container();

    Container& operator = (const Container&) = delete;
    Container& operator = (Container&&);

    bool alive() const;

    inline pid_t pid() const;
    inline int status() const;
    inline const cg::ControlGroup& cgroup() const;
    
    void spawn(const pb::Topology&);
    void kill();
    void wait();

  private:
    
    pid_t _pid {-1};

    int _status {-1};

    std::unique_ptr<char[]> _stack;

    cg::ControlGroup _cgroup;

    static int _entrypoint(void*);
};

// Function: pid
inline pid_t Container::pid() const {
  return _pid;
}

// Function: status
inline int Container::status() const {
  return _status;
}

// Function: cgroup
inline const cg::ControlGroup& Container::cgroup() const {
  return _cgroup;
}

//-------------------------------------------------------------------------------------------------

// Class: Container
//class Container {
//
//  struct ChildArgument {
//
//    int sync[2] = {-1, -1};
//
//    const char* file {nullptr};
//    char* const* argv {nullptr};
//    char* const* envp {nullptr};
//
//    ChildArgument(const char*, char* const*, char* const*);
//  };
//  
//  public:
//  
//  enum {
//    BLKIO = 0,
//    CPU,
//    CPUACCT,
//    CPUSET,
//    DEVICES,
//    FREEZER,
//    MEMORY,
//    NET_CLS,
//    NET_PRIO,
//    NUM_SUBSYSTEMS
//  };
//  

//  // Class: Subsystem
//  struct Subsystem {
//    Subsystem(const std::string& in_mount) : name {in_mount} {}
//    const std::string name;
//    std::filesystem::path mount;
//  };
//
//  public:
//
//    Container(const std::string&);
//    ~Container();
//
//    inline pid_t pid() const;
//    
//    inline const std::array<Subsystem, NUM_SUBSYSTEMS>& subsystems() const;
//    
//    inline void hostname(const std::string&);
//    inline void rootfs(const std::filesystem::path&); 
//
//    inline const std::string& hostname() const;
//    inline const std::filesystem::path& rootfs() const;
//    inline const std::filesystem::path& mount() const;
//
//    int cpu_shares() const;
//    int blkio_weight() const;
//    
//    void execve(const char*, char* const*, char* const*);
//    void blkio_weight(const int) const;
//    void cpu_shares(const int) const;
//    void cpuset_cpus(const std::string&) const;
//    void freezer_state(const std::string&) const;
//    void memory_limit_in_bytes(const size_t) const;
//    void memory_oom_control(const bool) const;
//
//		size_t memory_limit_in_bytes() const;
//		size_t memory_usage_in_bytes() const;
//		size_t memory_max_usage_in_bytes() const;
//
//		std::string freezer_state() const;
//
//  private:
//    
//    static std::array<Subsystem, NUM_SUBSYSTEMS> _subsystems;
//    
//    std::filesystem::path _mount;
//    pid_t _pid {-1};
//    
//    static int _entrypoint(void*);
//
//    std::unique_ptr<char[]> _stack;
//
//    void _write(const std::filesystem::path&, const std::string&) const;
//    void _group(const pid_t) const;
//
//    std::string _read(const std::filesystem::path&) const;
//};
//
//// Function: subsystems
//inline const std::array<Container::Subsystem, Container::NUM_SUBSYSTEMS>& 
//Container::subsystems() const {
//  return _subsystems;
//}
//
//// Function: pid
//inline pid_t Container::pid() const {
//  return _pid;
//}
//
//// Function: mount
//const std::filesystem::path& Container::mount() const {
//  return _mount;
//}

}; // End of namespace dtc. -----------------------------------------------------------------------


#endif

