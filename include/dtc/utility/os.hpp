/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_OS_HPP_
#define DTC_UTILITY_OS_HPP_

#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string_view>
#include <regex>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <experimental/filesystem>

namespace std {
  namespace filesystem = experimental::filesystem;
};

namespace dtc {

//-------------------------------------------------------------------------------------------------
// Environment utilities.
//-------------------------------------------------------------------------------------------------

// Function: environment_variables
// Accessing the list of environment variables is platform-dependent. On OS X, the environ symbol 
// sould be accessed through _NSGetEnviron(), while it suffices to directly use environ on other
// platforms.
std::unordered_map<std::string, std::string> environment_variables();

//-------------------------------------------------------------------------------------------------
// File descriptor utilities.
//-------------------------------------------------------------------------------------------------

// Function: get_fd_flags
// Get the flags of a file descriptor. File descriptor flags are miscellaneous attributes of 
// a file descriptor. These flags are associated with particular file descriptors, so that if 
// you have created duplicate file descriptors from a single opening of a file, each descriptor 
// has its own set of flags.
int get_fd_flags(const int, std::error_code&) noexcept;
int get_fd_flags(const int);

// Function: get_fs_flags
// Get the flags of the file status. Each open file description has certain associated status 
// flags, initialized by open and possibly modified by fcntl. Duplicated file descriptors refer
// to the same open file description, and thus share the same file status flags.
int get_fs_flags(const int, std::error_code&) noexcept;
int get_fs_flags(const int);

// Function: is_fd_valid
// Check whether a file descriptor is valid.
bool is_fd_valid(const int) noexcept;

// Procedure: make_fd_nonblocking
// Make a file descriptor non-blocking. Read and write access to this file descriptor can be done
// through select or other demultiplexing protocols.
void make_fd_nonblocking(const int);
bool is_fd_nonblocking(const int) noexcept;

// Procedure: make_fd_blocking
// Make a file descriptor blocking.
void make_fd_blocking(const int);
bool is_fd_blocking(const int) noexcept;

// Procedure: make_fd_close_on_exec
// It sets the close-on-exec flag for the file descriptor, which causes the file descriptor to 
// be automatically (and atomically) closed when any of the exec-family functions succeed.
void make_fd_close_on_exec(const int);
bool is_fd_close_on_exec(const int) noexcept;

// Procedure: make_fd_open_on_exec
// Make the give file descriptor open when any exec family call is invoked.
void make_fd_open_on_exec(const int);
bool is_fd_open_on_exec(const int) noexcept;

// Procedure: make_socket_reuseable
// Specifies that the rules used in validating addresses supplied to bind should allow reuse
// of local addresses, if this is supported by the protocol. This option takes an int value.
// This is a boolean option.
void make_socket_reuseable(const int);

// Procedure: keep_socket_keepalive
// Keeps connection active by enabling the periodic transmission of messages, if this is
// supported by the protocol. If the connected socket fails to respond to these messages,
// the connection is broken and threads writing to that socket are notified with a SIGPIPE
// signal. This is a boolean option.
void make_socket_keep_alive(const int);

// Function: duplicate_fd
// Duplicate a file descriptor.
int duplicate_fd(int);
void duplicate_fd(int, int);

// Procedure: redirect_fd
// Redirect a file descriptor to another one.
void redirect_fd(int, int);

// Function: write_all
// Write all the data to the given file descriptor or until an error happens. Notice that
// these two function calls transparently deal with non-blocking error (e.g., EAGAIN).
std::streamsize write_all(int, const void*, std::streamsize);
std::streamsize write_all(int, const std::filesystem::path&);

// Procedure: make_new_user_namespace
void unshare(int);
void unshare_user_namespace();

// Procedure: make_socket_nosigpipe
// When a connection closes, by default, the process receives a SIGPIPE signal. If the program 
// does not handle or ignore this signal, it will quit immediately. Therefore, the procedure
// tells the socket not to send the signal in the first place if the other side is not connected.
//inline void make_socket_nosigpipe(const int sfd) {
//  #ifdef SO_NOSIGPIPE
//  int one = 1;
//  if(setsockopt(sfd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one)) == -1) {
//    //LOGE("Failed to make socket nosigpipe (", strerror(errno), ")");
//  }
//  #endif
//}

// Procedure: show_fd_info
void show_fd_info();
void show_fd_info(int);

// Function: spawn
pid_t spawn(const char*, char *const[], char *const[]);

//-------------------------------------------------------------------------------------------------
// Resource utilities.
//-------------------------------------------------------------------------------------------------

std::tuple<double, double, double> cpu_load_average(std::error_code&) noexcept;
std::tuple<double, double, double> cpu_load_average();

//// Function: make_temp_file
//// Create a temp file and return the name of the temp file. 
//inline auto make_temp_file(const std::string& fpath = "/tmp/XXXXXX") {
//  std::unique_ptr <char[]> temp = std::make_unique<char[]>(fpath.size() + 1);
//  int fd = ::mkstemp(::strcpy(temp.get(), fpath.c_str()));
//  return (fd < 0 || ::close(fd) == -1) ? std::string("") : std::string(temp.get());
//}

/*// Function: access
// Check whether the calling process can access the file "fpath".
// The mode specifies the accessibility check(s) to be performed, and is either the value F_OK, 
// or a mask consisting of the bitwise OR of one or more of R_OK, W_OK, and X_OK.  F_OK tests 
// for the existence of the file.  R_OK, W_OK, and X_OK test whether the file exists and grants
// read, write, and execute permissions, respectively.
inline auto access(const std::string& fpath, const int mode = F_OK) {
  return ::access(fpath.c_str(), mode) != -1;
}

// Procedure: chmod
// Change the mode of a given file. 
// For example: -rw-r--r--  1 foo foo  272 Mar 17 08:22 test.txt
// User foo has read and write, group foo has read, and others has read.
inline void chmod(const std::string& fpath, const mode_t mode) {
  if(::chmod(fpath.c_str(), mode) == -1) {
    LOGE("Failed to chmod on ", fpath, " (", strerror(errno), ")"); 
  }
}

// Procedure: chdir
// Change the directory.
inline void chdir(const std::string& path) {
  if(::chdir(path.c_str()) == -1) {
    LOGE("Failed to chdir ", path, " (", strerror(errno), ")");
  }
}

// Procedure: rename
inline void rename(const std::string& from, const std::string& to) {
  if(::rename(from.c_str(), to.c_str()) == -1) {
    LOGE("Failed to rename ", from, " to ", to, " (", strerror(errno), ")");
  }
}

// Function: is_dir
// Check whether a given path is a directory
inline auto is_dir(const std::string& path) {
  struct stat s;
  if (::stat(path.c_str(), &s) < 0) {
    return false;
  }
  return S_ISDIR(s.st_mode);
}

// Function: is_file
// Check whether a given path is a regular file.
inline auto is_file(const std::string& path) {
  struct stat s;
  if (::stat(path.c_str(), &s) < 0) {
    return false;
  }
  return S_ISREG(s.st_mode);
}

// Function: is_fifo
// Check whether a given path is a fifo.
inline auto is_fifo(const std::string& path) {
  struct stat s;
  if (::stat(path.c_str(), &s) < 0) {
    return false;
  }
  return S_ISFIFO(s.st_mode);
}

// Function: ls
// Obtain the file list from a given path to a directory. In order to retrieve the file in
// the list, one can use the following, for example:
//
// for(const auto& file : ls("./")) {
//   ...
// }
//
inline auto ls(const std::string& path) {
  std::forward_list<std::string> res;
  DIR* dir = ::opendir(path.c_str());
  if (dir) {
    while(auto ptr = ::readdir(dir)) {
      if (::strcmp(ptr->d_name, ".") == 0 || ::strcmp(ptr->d_name, "..") == 0) {
        continue;
      }
      res.push_front(ptr->d_name);
    }
  }
  return res;
}

// Function: is_process_alive
// User kill to send a signal to the given process. If sig is 0, then no signal is sent, 
// but error checking is still performed; this can be used to check for the existence of a process
// ID or process group ID.
inline auto is_process_alive(const pid_t pid) {
  return (::kill(pid, 0) == 0 || errno == EPERM);
}

// Function: make_shm
// Create a shared memory for a given type. On Linux, shared memory objects are created in a 
// (tmpfs) virtual filesystem, normally mounted under /dev/shm.  
// The template will return a shared pointer to the newly-created space and the shared pointer
// is associated with a deletor which will automatically destroy the entire shared memory block.
template <typename T>
auto make_shm(const std::string& fpath , const size_t size = sizeof(T) ) {

  // Create a shared-memory block.
  auto fd = ::shm_open(fpath.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

  std::shared_ptr<T> ret;

  if(fd == -1) {
    LOGE("Failed to open a shared-memory block (", strerror(errno), ")");
    return ret;
  }


  if(::ftruncate(fd, size) == -1) {
    #ifndef __APPLE__
    LOGE("Failed to truncate a shared-memory block (", strerror(errno), ")");
    ::close(fd);
    return ret;
    #endif
  }


  auto ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if(ptr == MAP_FAILED) {
    LOGE("Failed to map a shared-memory block (", strerror(errno), ")");
    ::close(fd);
    return ret;
  }
  
  ret.reset(
    (T*)ptr, 
    [=] (T* ptr) {
      ::munmap(ptr, size); 
      ::close(fd);
      ::shm_unlink(fpath.c_str());
    }
  );

  return ret;
}


// Function: make_fifo_writer
//
// A FIFO special file (a named pipe) is similar to a pipe, except that it is accessed as part of 
// the filesystem. It can be opened by multiple processes for reading or writing. When processes 
// are exchanging data via the FIFO, the kernel passes all data internally without writing it to 
// the filesystem. Thus, the FIFO special file has no contents on the filesystem; the filesystem 
// entry merely serves as a reference point so that processes can access the pipe using a name in 
// the filesystem.
//
// The kernel maintains exactly one pipe object for each FIFO special file that is opened by at 
// least one process. The FIFO must be opened on both ends (reading and writing) before data can 
// be passed. Normally, opening the FIFO blocks until the other end is opened also.
//
// A process can open a FIFO in non-blocking mode. In this case, opening for read only will succeed
// even if none has opened on the write side yet; opening for write only will fail with ENXIO 
// (no such device or address) unless the other end has already been opened.
//
// Under Linux, opening a FIFO for read and write will succeed both in blocking and non-blocking 
// mode. POSIX leaves this behaviour undefined. This can be used to open a FIFO for writing while 
// there are no readers available. A process that uses both ends of the connection in order to 
// communicate with itself should be very careful to avoid deadlocks.  
//
inline int make_fifo_writer(const std::string& fpath) {

  // Create a fifo at fpath & the file shouldn't exist before creating (EEXIST)
  // By default, the permission is set to 664 (with umask 002). That is, owner and group has w/r
  // and others can read only.
  if((::mkfifo(fpath.c_str(), 0666) < 0) && errno != EEXIST) {   
    LOGE("Failed to make an FIFO writer file (", strerror(errno), ")");
    return -1;
  }   

  // Set to Write & Non-Blocking. Notice here reading data from this FIFO is undefined.
  auto fd = ::open(fpath.c_str(), O_WRONLY | O_NONBLOCK | O_CLOEXEC); 

  if(fd == -1) {   
    LOGE("Failed to open an FIFO writer file (", strerror(errno), ")");
  }   

  return fd;
}

// Function: make_fifo_reader
//
// A FIFO special file (a named pipe) is similar to a pipe, except that it is accessed as part of 
// the filesystem. It can be opened by multiple processes for reading or writing. When processes 
// are exchanging data via the FIFO, the kernel passes all data internally without writing it to 
// the filesystem. Thus, the FIFO special file has no contents on the filesystem; the filesystem 
// entry merely serves as a reference point so that processes can access the pipe using a name in 
// the filesystem.
//
// The kernel maintains exactly one pipe object for each FIFO special file that is opened by at 
// least one process. The FIFO must be opened on both ends (reading and writing) before data can 
// be passed. Normally, opening the FIFO blocks until the other end is opened also.
//
// A process can open a FIFO in non-blocking mode. In this case, opening for read only will succeed
// even if none has opened on the write side yet; opening for write only will fail with ENXIO 
// (no such device or address) unless the other end has already been opened.
//
// Under Linux, opening a FIFO for read and write will succeed both in blocking and non-blocking 
// mode. POSIX leaves this behaviour undefined. This can be used to open a FIFO for writing while 
// there are no readers available. A process that uses both ends of the connection in order to 
// communicate with itself should be very careful to avoid deadlocks.  
//
inline int make_fifo_reader(const std::string& fpath) {

  // Create a fifo at fpath & the file shouldn't exist before creating (EEXIST)
  if((::mkfifo(fpath.c_str(), 0666) < 0) && errno != EEXIST) {   
    LOGE("Failed to make an FIFO reader file (", strerror(errno), ")");
    return -1;
  }   

  // Set to Read Only & Non-Blocking
  int fd = ::open(fpath.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC); 
  if(fd == -1){   
    LOGE("Failed to open an FIFO reader file (", strerror(errno), ")");
  }   

  return fd;
} */


};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
