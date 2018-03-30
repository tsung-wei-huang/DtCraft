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

#include <dtc/kernel/cgroup.hpp>

namespace dtc::cg {

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

// Function: __set
static void __set(const std::filesystem::path& path, std::string_view value) {
  if(std::ofstream ofs(path); ofs.good()) {
    // Notice that the cgroup file doesn't allow std::endl.
    ofs << value;
  }
  else {
    LOGE("Failed to write ", value, " to ", path, " (", strerror(errno), ")");
  }
}

// Function: __get
static std::string __get(const std::filesystem::path& path) {
  if(std::ifstream ifs(path); ifs.good()) {
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
  }
  else {
    LOGE("Failed to open ", path, " (", strerror(errno), ")");
    return "";
  }
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
      LOGW("Failed to destroy cgroup (", e.what(), ")");
    }
  }
}

// Function: memory_mount
const std::filesystem::path ControlGroup::memory_mount() const {
  return __subsystems__()[MEMORY].mount / _path;
}

// Function: cpuset_mount
const std::filesystem::path ControlGroup::cpuset_mount() const {
  return __subsystems__()[CPUSET].mount / _path;
}

// Procedure: add
void ControlGroup::add(pid_t pid) const {
  for(const auto& s : __subsystems__()) {
    __set(s.mount / _path / "tasks", std::to_string(pid));
  }
}

// Function: memory_oom_control
void ControlGroup::memory_oom_control(bool value) const {
  __set(__subsystems__()[MEMORY].mount / _path / "memory.oom_control", value ? "1" : "0");
}

// Function: memory_swappiness
void ControlGroup::memory_swappiness(int value) const {
  __set(__subsystems__()[MEMORY].mount / _path / "memory.swappiness", std::to_string(value));
}

// Function: memory_limit_in_bytes
void ControlGroup::memory_limit_in_bytes(uintmax_t value) const {
  __set(__subsystems__()[MEMORY].mount / _path / "memory.limit_in_bytes", std::to_string(value));
}

// Function: cpuset_cpus
void ControlGroup::cpuset_cpus(std::string_view value) const {
  __set(__subsystems__()[CPUSET].mount / _path / "cpuset.cpus", value);
}

// Function: memory_limit_in_bytes
uintmax_t ControlGroup::memory_limit_in_bytes() const {
  return std::stoull(__get(__subsystems__()[MEMORY].mount / _path / "memory.limit_in_bytes"));
}

// Function: memory_usage_in_bytes
uintmax_t ControlGroup::memory_usage_in_bytes() const {
  return std::stoull(__get(__subsystems__()[MEMORY].mount / _path / "memory.usage_in_bytes"));
}

// Function: memory_max_usage_in_bytes
uintmax_t ControlGroup::memory_max_usage_in_bytes() const {
  return std::stoull(__get(__subsystems__()[MEMORY].mount / _path / "memory.max_usage_in_bytes"));
}

// Function: cpuacct_usage
uintmax_t ControlGroup::cpuacct_usage() const {
  return std::stoull(__get(__subsystems__()[CPUACCT].mount / _path / "cpuacct.usage"));
}

// Function: swappiness
int ControlGroup::swappiness() const {
  return std::stoi(__get(__subsystems__()[MEMORY].mount / _path / "memory.swappiness"));
}

// Function: cpuset_cpus
std::set<int> ControlGroup::cpuset_cpus() const {

  constexpr auto nexttoken = [] (const char *q,  int sep) {
    if (q)
      q = ::strchr(q, sep);
    if (q)
      q++;
    return q;
  };

  auto str = __get(__subsystems__()[CPUSET].mount / _path / "cpuset.cpus");
  
  str.erase(std::remove_if(
    str.begin(), 
    str.end(), 
    [] (const char c) { return c == ' ' || c == '\n' || c == '\r'; }
  ), str.end());

  std::set<int> cpus;
  const char *p {nullptr};
  const char *q {nullptr};
  int r = 0;

  q = str.c_str();

  while (1) {

    p = q;
    q = nexttoken(q, ',');

    if(!p) break;

    int a;   // beg range
    int b;   // end range
    int s;   // stride
    const char *c1, *c2;
    char c;

    if ((r = ::sscanf(p, "%u%c", &a, &c)) < 1) {
      //goto failure;
      //break;
      DTC_THROW("cpuset.cpus format error: ", str);
    }

    b = a;
    s = 1;

    c1 = nexttoken(p, '-');
    c2 = nexttoken(p, ',');
    if (c1 != nullptr && (c2 == nullptr || c1 < c2)) {
      if ((r = ::sscanf(c1, "%u%c", &b, &c)) < 1) {
        //goto failure;
        //break;
        DTC_THROW("cpuset.cpus format error: ", str);
      }
      c1 = nexttoken(c1, ':');
      if (c1 != nullptr && (c2 == nullptr || c1 < c2)) {
        if ((r = sscanf(c1, "%u%c", &s, &c)) < 1) {
          //goto failure;
          //break;
          DTC_THROW("cpuset.cpus format error: ", str);
        }
        if (s == 0) {
          //goto failure;
          //break;
          DTC_THROW("cpuset.cpus format error: ", str);
        }
      }
    }

    if (!(a <= b)) {
      DTC_THROW("cpuset.cpus format error: ", str);
      //goto failure;
      //break;
    }

    while (a <= b) {
      //cpus.push_back(a);
      cpus.insert(a);
      a += s;
    }
  }

  if (r == 2) {
    DTC_THROW("cpuset.cpus format error: ", str);
    //goto failure;
  }

  //std::sort(cpus.begin(), cpus.end());
  //cpus.erase(std::unique(cpus.begin(), cpus.end()), cpus.end());

  return cpus;
}

};  // end of namespace dtc::cg. ------------------------------------------------------------------



