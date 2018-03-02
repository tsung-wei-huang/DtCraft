
#include <dtc/kernel/container.hpp>

namespace dtc {

// ------------------------------------------------------------------------------------------------

// Destructor
Container::~Container() {
  if(_pid != -1) {
    LOGF("Destructing container while process ", _pid, " is running");
  }
}

// Move constructor
Container::Container(Container&& rhs) :
  _pid    {rhs._pid},
  _status {rhs._status},
  _stack  {std::move(rhs._stack)} {
  
  rhs._pid = -1;
  rhs._status = -1;
}

// Move operator
Container& Container::operator = (Container&& rhs) {

  // PID
  _pid = rhs._pid;
  rhs._pid = -1;

  // Status
  _status = rhs._status;
  rhs._status = -1;

  // Stack
  _stack = std::move(rhs._stack);

  return *this;
}

// Function: _entrypoint
int Container::_entrypoint(void* arg) {

  const auto& C = *(static_cast<ChildArgument*>(arg)); 
  
  // Close the parent-side communication.
  ::close(C.sync[0]);
      
  //printf("Child waiting for parent to finish uid/gid mapping\n");
  //if(int i=0; ::read(C.sync[1], &i, sizeof(int)) != sizeof(int)) {
  //  ::close(C.sync[1]);
  //  throw std::system_error(make_posix_error_code(errno), "Failed to synchronize with the parent");
  //}
  
  // Here we must check whether pid/uid is set correctly by the master program.
  //std::cout << getuid() << " ----------------------- " << getgid() << " ---------- " << getpid() << '\n';
  //assert(geteuid() == 0 && getegid() == 0);

  // TODO (Chun-Xun)
  //printf("ready to execve\n");
  ::mount(nullptr, "/proc", "proc", 0, nullptr);
  ::mount(nullptr, "/sys", "sysfs", 0, nullptr);
  ::mount(nullptr, std::filesystem::temp_directory_path().c_str(), "tmpfs", 0, nullptr);

  // Initialize container attributes.
  ::execve(
    C.topology.runtime.c_file().get(), 
    C.topology.runtime.c_argv().get(), 
    C.topology.runtime.c_envp().get()
  );  
      
  if(int e=errno; ::write(C.sync[1], &e, sizeof(e)) != sizeof(e)) {
    ::close(C.sync[1]);
    throw std::system_error(make_posix_error_code(e), "Child failed to exec");
  }

  ::close(C.sync[1]);

  return EXIT_CONTAINER_EXEC_FAILED;
}

// Function: _write
void Container::_write(const std::filesystem::path& path, std::string_view value) const {
  if(std::ofstream ofs(path); ofs.good()) {
    // Notice that the cgroup file doesn't allow std::endl.
    ofs << value;
  }
  else {
    LOGE("Container failed to write ", value, " to ", path);
  }
}

// Function: alive
// User kill to send a signal to the given process. If sig is 0, then no signal is sent, 
// but error checking is still performed; this can be used to check for the existence of 
// a process ID or process group ID.
bool Container::alive() const {
  if(_pid == -1) return false;
  return ::kill(_pid, 0) == 0 || errno == EPERM;
}

// Procedure: exec2
void Container::exec2(const pb::Topology& tpg) {
  
  if(_pid != -1) {
    throw std::system_error(make_posix_error_code(EAGAIN), "Double forked");
  }

  ChildArgument C{tpg};

  if(::socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, C.sync) == -1) {
    throw std::system_error(make_posix_error_code(errno), "Container failed to creatd socket pair");
  }
  
  constexpr size_t STACK_SIZE = 1024*1024;

  _stack = std::make_unique<char[]>(STACK_SIZE);
  
  // Create a new process under new namespaces. NEWNET is the most time-consuming.
  _pid = ::clone(
    _entrypoint, _stack.get() + STACK_SIZE, 
    CLONE_NEWUSER | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, 
    &C
  );
    
  // Close the child side communication channel.
  ::close(C.sync[1]);

  // Clone failed.
  if(_pid == -1) {
    ::close(C.sync[0]);
    throw std::system_error(make_posix_error_code(errno), "Clone failed");
  }
  
  // Write uid and gid mapping.
  //std::filesystem::path path(std::filesystem::path("/proc") / std::to_string(_pid));

  //_write(path / "uid_map", "0 0 1");
  //_write(path / "gid_map", "0 0 1");

  //// Move the process to control group.
  //_group(_pid);

  // Synchronize with child
  //printf("parent done with uid/gid mapping, synchronizing with child %d\n", _pid);
  //if(int i=0; ::write(C.sync[0], &i, sizeof(int)) != sizeof(int)) {
  //  ::close(C.sync[0]);
  //  throw std::system_error(make_posix_error_code(errno), "Failed to synchronize with the child");
  //}

  //printf("parent synchronize with child exec %d\n", _pid);
  if(int e=0; ::read(C.sync[0], &e, sizeof(e)) != 0) {
    ::close(C.sync[0]);
    assert(::waitpid(_pid, &_status, 0) == _pid);
    _pid = -1;
    throw std::system_error(make_posix_error_code(e), "Exec failed");
  }
  
  //printf("Done exec %d\n", _pid);

  ::close(C.sync[0]);
}

// Procedure: exec
void Container::exec(const pb::Topology& tpg) {

  if(_pid != -1) {
    throw std::system_error(make_posix_error_code(EAGAIN), "Double forked");
  }

  // Create a pipe for synchronization
  if(int fd[2]; ::pipe2(fd, O_CLOEXEC | O_DIRECT) == -1) {
    throw std::system_error(make_posix_error_code(errno), "Pipe failed");
  }
  else {
    // Case 1: failure (parent scope)
    if(_pid = ::fork(); _pid == -1) {
      ::close(fd[0]);
      ::close(fd[1]);
      throw std::system_error(make_posix_error_code(errno), "Fork failed");
    }
    // Case 2: child (child scope)
    else if(_pid == 0) {
      ::execve(tpg.runtime.c_file().get(), tpg.runtime.c_argv().get(), tpg.runtime.c_envp().get());
      int e = errno;
      assert(::write(fd[1], &e, sizeof(e)) == sizeof(e));
      std::exit(EXIT_CONTAINER_EXEC_FAILED);
    }
    // Case 3: parent (parent scope)
    else {
      ::close(fd[1]);
      if(int e=0; ::read(fd[0], &e, sizeof(e)) != 0) {
        ::close(fd[0]);
        assert(::waitpid(_pid, &_status, 0) == _pid);
        _pid = -1;
        throw std::system_error(make_posix_error_code(e), "Exec failed");
      }
      ::close(fd[0]);
    }
  }
}

// Function: wait a process
void Container::wait() {

  // Process does not exist.
  if(_pid == -1) {
    return;
  }
  
  // Reap the process.
  int r;

  do {
    r = ::waitpid(_pid, &_status, 0);
  }while((r == -1 && errno == EINTR) || (r != -1 && !WIFEXITED(_status) && !WIFSIGNALED(_status)));

  if(r != _pid) {
    throw std::system_error(make_posix_error_code(errno), "Failed to wait");
  }
  else {
    _pid = -1;
  }
}

// Function: kill
void Container::kill() {
  if(_pid != -1 && ::kill(_pid, SIGKILL) == -1) {
    throw std::system_error(make_posix_error_code(errno), "Failed to kill");
  }
}

//-------------------------------------------------------------------------------------------------

//// ChildArgument
//Container::ChildArgument::ChildArgument(const char* f, char* const* a, char* const* e) : 
//  file{f}, argv{a}, envp{e} {
//}
//
////-------------------------------------------------------------------------------------------------
//
//// Static declaration
//std::array<Container::Subsystem, Container::NUM_SUBSYSTEMS> Container::_subsystems {
//  Container::Subsystem("blkio"),
//  Container::Subsystem("cpu"),
//  Container::Subsystem("cpuacct"),
//  Container::Subsystem("cpuset"),
//  Container::Subsystem("devices"),
//  Container::Subsystem("freezer"),
//  Container::Subsystem("memory"),
//  Container::Subsystem("net_cls"),
//  Container::Subsystem("net_prio")
//};
//
////-------------------------------------------------------------------------------------------------
//// Constructor
//Container::Container(const std::string& mount) : 
//  _mount {mount} {
//  
//  static std::once_flag _once_flag;
//  
//  // Initialize the control group data structures.
//  std::call_once(_once_flag, [] () mutable {
//
//    // Assertion
//    if(!std::filesystem::exists("/proc/mounts")) {
//      LOGF("/proc/mounts not found. cgroup is not supported in the machine.");
//    }
//
//    // Extract all mounted paths.
//    auto mifs = fopen("/proc/mounts", "re");
//    
//    struct mntent mntent;
//    struct mntent* mptr {nullptr};
//    char mntent_buffer[4*FILENAME_MAX];
//    
//    while ((mptr = getmntent_r(mifs, &mntent, mntent_buffer, sizeof(mntent_buffer)))) {
//
//      if(strcmp(mptr->mnt_type, "cgroup")) {
//        continue;
//      }
//
//      if(hasmntopt(mptr, "blkio")) _subsystems[BLKIO].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "cpu")) _subsystems[CPU].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "cpuacct")) _subsystems[CPUACCT].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "cpuset")) _subsystems[CPUSET].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "devices")) _subsystems[DEVICES].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "freezer")) _subsystems[FREEZER].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "memory")) _subsystems[MEMORY].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "net_cls")) _subsystems[NET_CLS].mount = mptr->mnt_dir;
//      if(hasmntopt(mptr, "net_prio")) _subsystems[NET_PRIO].mount = mptr->mnt_dir;
//    }
//		
//    // Check whether all subsystems are mounted.
//    for(const auto& s : _subsystems) {
//      if(s.mount.empty()) {
//        LOGF(s.name, " is not mounted in cgroup");
//      }
//    }
//
//    fclose(mifs);
//  });
//    
//  // Attach the group to every one of the subsystem.
//  for(const auto& s : _subsystems) {
//    std::filesystem::create_directories(s.mount / _mount);
//  }
//}
//
//// Destructor
//Container::~Container() {
//  
//  // By callong the destructor the child process is supposed to die already.
//  assert(::waitpid(_pid, nullptr, 0) == _pid);
//  
//  // TODO: move all suspended processes to the parent?
//  for(const auto& s : _subsystems) {
//    const auto path = s.mount / _mount;
//    if(std::filesystem::exists(path)) {
//      std::filesystem::remove(path);
//    }
//  }
//}
//
//// Function: _entrypoint
//// The main entry point for the cloned child process.
//int Container::_entrypoint(void* arg) {
//  
//  auto carg = static_cast<ChildArgument*>(arg); 
//  
//  //printf("clond %s\n", carg->file);
//
//  assert(is_fd_valid(carg->sync[0]) && is_fd_valid(carg->sync[1]));
//
//  assert(
//    ::close(carg->sync[1]) == 0 &&
//    ::mount(nullptr, "/proc", "proc", 0, nullptr) == 0 &&
//    ::mount(nullptr, "/sys", "sysfs", 0, nullptr) == 0 &&
//    ::mount(nullptr, std::filesystem::temp_directory_path().c_str(), "tmpfs", 0, nullptr) == 0
//  );
//  
//  // Synchronized with parent until EOF.
//  char c;
//  auto ret = ::read(carg->sync[0], &c, sizeof(char));
//  assert(ret == 0 && ::close(carg->sync[0]) == 0);
//  
//  // Invoke the executor.
//  assert(::execve(carg->file, carg->argv, carg->envp) != -1);
//
//  return -1;
//}
//
//// Function: _group
//void Container::_group(const pid_t p) const {
//  for(const auto& s : _subsystems) {
//    _write(s.mount / _mount / "tasks", std::to_string(p));
//  }
//}
//
//// Procedure: blkio_weight
//void Container::blkio_weight(const int value) const {
//  _write(_subsystems[BLKIO].mount / _mount / "blkio.weight", std::to_string(value));
//}
//
//// Function: cpu_shares
//void Container::cpu_shares(const int value) const {
//  _write(_subsystems[CPU].mount / _mount / "cpu.shares", std::to_string(value));
//}
//
//// Function: cpuset_cpus
//void Container::cpuset_cpus(const std::string& value) const {
//  _write(_subsystems[CPUSET].mount / _mount / "cpuset.cpus", value);
//}
//
//// Function: freezer_state
//void Container::freezer_state(const std::string& value) const {
//  _write(_subsystems[FREEZER].mount / _mount / "freezer.state", value);
//}
//    
//// Function: memory_limit_in_bytes
//void Container::memory_limit_in_bytes(const size_t value) const {
//  _write(_subsystems[MEMORY].mount / _mount / "memory.limit_in_bytes", std::to_string(value));
//}
//
//// Function: memory_oom_control
//void Container::memory_oom_control(const bool value) const {
//  _write(_subsystems[MEMORY].mount / _mount / "memory.oom_control", value ? "1" : "0");
//}
//
//// Function: _write
//void Container::_write(const std::filesystem::path& path, const std::string& value) const {
//  std::ofstream ofs(path);
//  // Notice that the cgroup file doesn't allow std::endl.
//  if(ofs.good()) {
//    ofs << value;
//  }
//  else {
//    LOGE("Failed to write ", value, " to ", path);
//  }
//}
//
//// Function: _read
//std::string Container::_read(const std::filesystem::path& path) const {
//  std::ostringstream oss;
//  std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
//	if(ifs) {
//    oss << ifs.rdbuf();
//  }
//	else {
//    LOGE("Failed to open ", path);
//	}
//  return oss.str();
//}
//
//// Function: blkio_weight
//int Container::blkio_weight() const {
//  return std::stoi(_read(_subsystems[BLKIO].mount / _mount / "blkio.weight"));
//}
//
//// Function: cpu_shares
//int Container::cpu_shares() const {
//  return std::stoi(_read(_subsystems[CPU].mount / _mount / "cpu.shares"));
//}
//
//// Function: freezer_state
//std::string Container::freezer_state() const {
//  auto str = _read(_subsystems[FREEZER].mount / _mount / "freezer.state");
//  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
//  return str;
//}
//
//// Function: memory_limit_in_bytes
//size_t Container::memory_limit_in_bytes() const {
//	return std::stoull(_read(_subsystems[MEMORY].mount / _mount / "memory.limit_in_bytes"));
//}
//
//// Function: memory_usage_in_bytes
//size_t Container::memory_usage_in_bytes() const {
//	return std::stoull(_read(_subsystems[MEMORY].mount / _mount / "memory.usage_in_bytes"));
//}
//
//// Function: memory_max_usage_in_bytes
//size_t Container::memory_max_usage_in_bytes() const {
//	return std::stoull(_read(_subsystems[MEMORY].mount / _mount / "memory.max_usage_in_bytes"));
//}

};  // End of namespace dtc. ----------------------------------------------------------------------





