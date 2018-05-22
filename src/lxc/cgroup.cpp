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

#include <dtc/lxc/cgroup.hpp>

namespace dtc {

// Cgroup utility
std::array<Subsystem, NUM_SUBSYSTEMS>& __subsystems__() {

  static std::array<Subsystem, NUM_SUBSYSTEMS> __subsystems {
    Subsystem("blkio"),
    Subsystem("cpu"),
    Subsystem("cpuacct"),
    Subsystem("cpuset"),
    Subsystem("devices"),
    Subsystem("freezer"),
    Subsystem("memory"),
    Subsystem("net_cls"),
    Subsystem("net_prio")
  };

  static std::once_flag _once_flag;
  
  // Initialize the control group data structures.
  std::call_once(_once_flag, [] () {

    // Assertion
    if(!std::filesystem::exists("/proc/mounts")) {
      throw std::runtime_error("/proc/mounts not found.");
    }

    // Extract all mounted paths.
    auto mifs = fopen("/proc/mounts", "re");
    
    struct mntent mntent;
    struct mntent* mptr {nullptr};
    char mntent_buffer[4*FILENAME_MAX];
    
    while ((mptr = getmntent_r(mifs, &mntent, mntent_buffer, sizeof(mntent_buffer)))) {

      if(strcmp(mptr->mnt_type, "cgroup")) {
        continue;
      }

      if(hasmntopt(mptr, "blkio")) {
        __subsystems[BLKIO].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "cpu")) {
        __subsystems[CPU].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "cpuacct")) {
        __subsystems[CPUACCT].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "cpuset")) {  
        __subsystems[CPUSET].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "devices")) {
        __subsystems[DEVICES].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "freezer")) {
        __subsystems[FREEZER].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "memory")) {
        __subsystems[MEMORY].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "net_cls")) {
        __subsystems[NET_CLS].mount = mptr->mnt_dir;
      }

      if(hasmntopt(mptr, "net_prio")) {
        __subsystems[NET_PRIO].mount = mptr->mnt_dir;
      }
    }
    
    fclose(mifs);
  
    for(const auto& s : __subsystems) {
      if(s.mount.empty()) {
        LOGE("subsystem ", s.name, " not found in cgroup mounts");
        std::exit(EXIT_FAILURE);
      }
    }

  });

  return __subsystems;
}

// ------------------------------------------------------------------------------------------------

// Constructor
ControlGroup::ControlGroup(const std::filesystem::path& p) : _path {p} {
  for(const auto& s : __subsystems__()) {
    std::filesystem::create_directories(s.mount / _path);
  }
}

// Destructor
ControlGroup::~ControlGroup() {
  if(!_path.empty()) {
    try {
      for(const auto& s : __subsystems__()) {
        if(std::filesystem::exists(s.mount / _path)) {
          std::filesystem::remove(s.mount / _path); 
        }
      }
    }
    catch(const std::exception& e) {
      // This is possible because some subdirectories might not be unlinked properly.
      LOGW("Failed to unmount cgroup (", e.what(), ")");
    }
  }
}

// Function: memory_mount
std::filesystem::path ControlGroup::memory_mount() const {
  return __subsystems__()[MEMORY].mount / _path;
}

// Function: cpuset_mount
std::filesystem::path ControlGroup::cpuset_mount() const {
  return __subsystems__()[CPUSET].mount / _path;
}

// Function: blkio_mount
std::filesystem::path ControlGroup::blkio_mount() const {
  return __subsystems__()[BLKIO].mount / _path;
}

// Procedure: add
void ControlGroup::add(pid_t pid) const {
  for(const auto& s : __subsystems__()) {
    _set(s.mount / _path / "tasks", std::to_string(pid));
  }
}

// Function: memory_oom_control
void ControlGroup::memory_oom_control(bool value) const {
  _set(__subsystems__()[MEMORY].mount / _path / "memory.oom_control", value ? "1" : "0");
}

// Function: memory_swappiness
void ControlGroup::memory_swappiness(int value) const {
  _set(__subsystems__()[MEMORY].mount / _path / "memory.swappiness", std::to_string(value));
}

// Function: memory_limit_in_bytes
void ControlGroup::memory_limit_in_bytes(uintmax_t value) const {
  _set(__subsystems__()[MEMORY].mount / _path / "memory.limit_in_bytes", std::to_string(value));
}

// Function: cpuset_cpus
void ControlGroup::cpuset_cpus(std::string_view value) const {
  _set(__subsystems__()[CPUSET].mount / _path / "cpuset.cpus", value);
}

// Function: memory_limit_in_bytes
uintmax_t ControlGroup::memory_limit_in_bytes() const {
  return std::stoull(_get(__subsystems__()[MEMORY].mount / _path / "memory.limit_in_bytes"));
}

// Function: memory_usage_in_bytes
uintmax_t ControlGroup::memory_usage_in_bytes() const {
  return std::stoull(_get(__subsystems__()[MEMORY].mount / _path / "memory.usage_in_bytes"));
}

// Function: memory_max_usage_in_bytes
uintmax_t ControlGroup::memory_max_usage_in_bytes() const {
  return std::stoull(_get(__subsystems__()[MEMORY].mount / _path / "memory.max_usage_in_bytes"));
}

// Function: blkio_weight
uintmax_t ControlGroup::blkio_weight() const {
  return std::stoull(_get(__subsystems__()[BLKIO].mount / _path / "blkio.weight"));
}

// Function: cpuacct_usage
uintmax_t ControlGroup::cpuacct_usage() const {
  return std::stoull(_get(__subsystems__()[CPUACCT].mount / _path / "cpuacct.usage"));
}

// Function: swappiness
int ControlGroup::swappiness() const {
  return std::stoi(_get(__subsystems__()[MEMORY].mount / _path / "memory.swappiness"));
}

// Function: _set
void ControlGroup::_set(const std::filesystem::path& path, std::string_view value) const {
  if(std::ofstream ofs(path); ofs.good()) {
    // Notice that the cgroup file doesn't allow std::endl.
    ofs << value;
  }
  else {
    DTC_THROW("failed to write ", value, " to ", path, " (", strerror(errno), ")");
  }
}

// Function: _get
std::string ControlGroup::_get(const std::filesystem::path& path) const {

  std::ifstream ifs(path);

  if(!ifs.good()) {
    DTC_THROW("failed to open ", path, " (", strerror(errno), ")");
  }

  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

// Function: cpuset_cpus
std::set<int> ControlGroup::cpuset_cpus() const {

  auto str = _get(__subsystems__()[CPUSET].mount / _path / "cpuset.cpus");
  
  std::set<int> cpus;

  auto extract_cpu = [&] (std::string str) {
    if(str.empty()) return;
    // Single number
    if(auto pos = str.find_first_of('-'); pos == std::string::npos) {
      cpus.insert(std::stoi(str));
    }
    // Range
    else {
      auto beg = std::stof(str.substr(0, pos));
      auto end = std::stof(str.substr(pos+1));
      for(int i=beg; i<=end; ++i) {
        cpus.insert(i);
      }
    }
  };

  std::string token;

  for(const auto c : str) {
    if(std::isspace(c)) continue;
    if(c == ',' || c == '\n' || c == '\r') {
      extract_cpu(std::move(token));
    }
    else {
      token.push_back(c);
    }
  }
  extract_cpu(std::move(token));

  return cpus;
}

};  // end of namespace dtc::cg. ------------------------------------------------------------------



