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

#ifndef DTC_LXC_CGROUP_HPP_
#define DTC_LXC_CGROUP_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/static/logger.hpp>

namespace dtc {

#ifndef CLONE_FS
#  define CLONE_FS                0x00000200
#endif
#ifndef CLONE_NEWNS
#  define CLONE_NEWNS             0x00020000
#endif
#ifndef CLONE_NEWCGROUP
#  define CLONE_NEWCGROUP         0x02000000
#endif
#ifndef CLONE_NEWUTS
#  define CLONE_NEWUTS            0x04000000
#endif
#ifndef CLONE_NEWIPC
#  define CLONE_NEWIPC            0x08000000
#endif
#ifndef CLONE_NEWUSER
#  define CLONE_NEWUSER           0x10000000
#endif
#ifndef CLONE_NEWPID
#  define CLONE_NEWPID            0x20000000
#endif
#ifndef CLONE_NEWNET
#  define CLONE_NEWNET            0x40000000
#endif


// SubsystemType
enum SubsystemType : int {
  BLKIO = 0,
  CPU,
  CPUACCT,
  CPUSET,
  DEVICES,
  FREEZER,
  MEMORY,
  NET_CLS,
  NET_PRIO,
  NUM_SUBSYSTEMS
};

// Subsystem
struct Subsystem {
  Subsystem(const std::string& in) : name {in} {}
  const std::string name;
  std::filesystem::path mount;
};

std::array<Subsystem, NUM_SUBSYSTEMS>& __subsystems__();

//-------------------------------------------------------------------------------------------------
    
// Class: ControlGroup
class ControlGroup {

  public:

    ControlGroup(const std::filesystem::path&);
    ControlGroup(const ControlGroup&) = delete;
    ControlGroup(ControlGroup&&) = default;

    ~ControlGroup();

    ControlGroup& operator = (const ControlGroup&) = delete;
    ControlGroup& operator = (ControlGroup&&) = default;

    void memory_swappiness(int) const;
    void memory_limit_in_bytes(uintmax_t) const;
    void memory_oom_control(bool) const;
    void add(pid_t) const;
    void cpuset_cpus(std::string_view) const;
    
    int swappiness() const;
    uintmax_t cpuacct_usage() const;
    uintmax_t memory_limit_in_bytes() const;
    uintmax_t memory_usage_in_bytes() const;
    uintmax_t memory_max_usage_in_bytes() const;
    uintmax_t blkio_weight() const;
    std::set<int> cpuset_cpus() const;

    inline const std::filesystem::path& path() const;

    std::filesystem::path memory_mount() const;
    std::filesystem::path cpuset_mount() const;
    std::filesystem::path blkio_mount() const;
  
  private:  

    std::filesystem::path _path;

    void _set(const std::filesystem::path&, std::string_view) const;
    std::string _get(const std::filesystem::path&) const;
};

// Function: path
inline const std::filesystem::path& ControlGroup::path() const {
  return _path;
}

};  // end of namespace dtc::cg. ----------------------------------------------------------------------


#endif



