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

#include <dtc/lxc/container.hpp>

namespace dtc {

// Procedure: mount_cgroup
Container::Container(const std::filesystem::path& cgroup) : 
  _cgroup {cgroup} {
}

// Destructor
Container::~Container() {
  assert(_pid == -1);
}

// Move constructor
Container::Container(Container&& rhs) :
  _pid    {rhs._pid},
  _status {rhs._status},
  _stack  {std::move(rhs._stack)},
  _cgroup {std::move(rhs._cgroup)} {
  
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

  // Cgroup
  _cgroup = std::move(rhs._cgroup);

  return *this;
}

// Function: _entrypoint
int Container::_entrypoint(void* arg) {

  auto& C = *(static_cast<ChildArgument*>(arg)); 
  
  // Close the parent-side communication.
  C.sync[0].reset();
      
  //printf("Child waiting for parent to finish uid/gid mapping\n");
  if(int i=0; ::read(C.sync[1]->fd(), &i, sizeof(int)) != sizeof(int)) {
    LOGE("Failed to sync with the parent (", strerror(errno), ")");
    return EXIT_CONTAINER_SPAWN_FAILED;
  }
  
  // Here we must check whether pid/uid is set correctly by the master program.
  //std::cout << getuid() << " ----------------------- " << getgid() << " ---------- " << getpid() << '\n';
  //assert(geteuid() == 0 && getegid() == 0);

  //printf("ready to execve\n");
  if(::mount(nullptr, "/proc", "proc", 0, nullptr) == -1) {
    LOGE("Failed to cmount /proc (", strerror(errno), ")");
    return EXIT_CONTAINER_SPAWN_FAILED;
  }

  if(::mount(nullptr, "/sys", "sysfs", 0, nullptr) == -1) {
    LOGE("Failed to cmount /sys (", strerror(errno), ")");
    return EXIT_CONTAINER_SPAWN_FAILED;
  }

  if(::mount(nullptr, std::filesystem::temp_directory_path().c_str(), "tmpfs", 0, nullptr) == -1) {
    LOGE("Failed to mount ", std::filesystem::temp_directory_path(), "(", strerror(errno), ")");
    return EXIT_CONTAINER_SPAWN_FAILED;
  }

  // Initialize container attributes.
  ::execve(
    C.topology.runtime.c_file().get(), 
    C.topology.runtime.c_argv().get(), 
    C.topology.runtime.c_envp().get()
  );  
      
  if(int e=errno; ::write(C.sync[1]->fd(), &e, sizeof(e)) != sizeof(e)) {
    LOGE("Failed to exec (", strerror(e), ")");
    return EXIT_CONTAINER_SPAWN_FAILED;
  }

  return EXIT_CONTAINER_SPAWN_FAILED;
}

// Function: alive
// User kill to send a signal to the given process. If sig is 0, then no signal is sent, 
// but error checking is still performed; this can be used to check for the existence of 
// a process ID or process group ID.
bool Container::alive() const {
  if(_pid == -1) return false;
  return ::kill(_pid, 0) == 0 || errno == EPERM;
}

// Procedure: spawn
void Container::spawn(const pb::Topology& tpg) {
  
  if(_pid != -1) {
    DTC_THROW("Container already spawned");
  }

  ChildArgument C{tpg};

  std::tie(C.sync[0], C.sync[1]) = make_sync_socket_pair();
  
  constexpr size_t STACK_SIZE = 1024*1024;

  _stack = std::make_unique<char[]>(STACK_SIZE);
  
  // Create a new process under new namespaces. NEWNET is the most time-consuming.
  _pid = ::clone(
    _entrypoint, _stack.get() + STACK_SIZE, 
    CLONE_NEWUSER | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, 
    &C
  );
    
  // Close the child side communication channel.
  C.sync[1].reset();
  //::close(C.sync[1]);

  // Clone failed.
  if(_pid == -1) {
    //::close(C.sync[0]);
    throw std::system_error(make_posix_error_code(errno), "Clone failed");
  }

  // Enable the cgroup.
  _cgroup.add(_pid);
  
  // Write uid and gid mapping.
  //std::filesystem::path path(std::filesystem::path("/proc") / std::to_string(_pid));

  //_write(path / "uid_map", "0 0 1");
  //_write(path / "gid_map", "0 0 1");

  // Synchronize with child
  //printf("parent done with uid/gid mapping, synchronizing with child %d\n", _pid);
  if(int i=0; ::write(C.sync[0]->fd(), &i, sizeof(int)) != sizeof(int)) {
    throw std::system_error(make_posix_error_code(errno), "Failed to synchronize with the child");
  }

  //printf("parent synchronize with child exec %d\n", _pid);
  if(int e=0; ::read(C.sync[0]->fd(), &e, sizeof(e)) != 0) {
    assert(::waitpid(_pid, &_status, 0) == _pid);
    _pid = -1;
    throw std::system_error(make_posix_error_code(e), "Exec failed");
  }
}

/*// Procedure: exec
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
      std::exit(EXIT_CONTAINER_SPAWN_FAILED);
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
}*/

// Function: wait a process
void Container::wait() {

  // Process does not exist.
  if(_pid == -1) {
    return;
  }
  
  // Reap the process.
  int r {-1};

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

};  // End of namespace dtc. ----------------------------------------------------------------------





