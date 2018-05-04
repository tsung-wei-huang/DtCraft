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

#ifndef DTC_UTILITY_CGROUP_HPP_
#define DTC_UTILITY_CGROUP_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/static/logger.hpp>

namespace dtc::cg {

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
    std::set<int> cpuset_cpus() const;

    inline const std::filesystem::path& path() const;

    const std::filesystem::path memory_mount() const;
    const std::filesystem::path cpuset_mount() const;
  
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



