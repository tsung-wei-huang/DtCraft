/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/utility/os.hpp>

namespace dtc {

// Function: environment_variables
std::unordered_map<std::string, std::string> environment_variables() {
   
  char** envp =
  #ifdef __APPLE__
    *_NSGetEnviron();
  #else
    environ;
  #endif

  std::unordered_map<std::string, std::string> res;
  for(size_t idx = 0; envp[idx] != nullptr; ++idx) {
    std::string_view entry(envp[idx]);
    if(auto pos = entry.find_first_of('='); pos != std::string_view::npos) {
      res.try_emplace(std::string(entry.substr(0, pos)), entry.substr(pos+1));
    }
  }
  return res;
}

// Function: open
int open(const char* fpath, int flags) {
  using namespace std::literals::string_literals;
  if(auto fd = ::open(fpath, flags); fd == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "Failed to open "s + fpath
    ); 
  }
  else return fd;
}

// Function: open
int open(const std::filesystem::path& fpath, int flags) {
  return open(fpath.c_str(), flags);
}

// Function: is_fd_valid
bool is_fd_valid(const int fd) noexcept {
  return ::fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

// Function: get_fd_flags
//int get_fd_flags(const int fd, std::error_code& errc) noexcept {
//  if(auto ret = ::fcntl(fd, F_GETFD, nullptr); ret == -1) {
//    errc = std::make_error_code(static_cast<std::errc>(errno));
//    return -1;
//  } 
//  else {
//    errc.clear();
//    return ret;
//  }
//}
//
//// Function: get_fd_flags
//int get_fd_flags(const int fd) {
//  if(auto ret = ::fcntl(fd, F_GETFD, nullptr); ret == -1) {
//    throw std::system_error(
//      std::make_error_code(static_cast<std::errc>(errno)), 
//      "Failed to get descriptor flags"
//    );
//  } 
//  else {
//    return ret;
//  }
//}
//
//// Function: get_fs_flags
//int get_fs_flags(const int fd, std::error_code& errc) noexcept {
//  if(auto ret = ::fcntl(fd, F_GETFL, nullptr); ret == -1) {
//    errc = std::make_error_code(static_cast<std::errc>(errno));
//    return -1;
//  } 
//  else {
//    errc.clear();
//    return ret;
//  }
//}
//
//// Function: get_fs_flags
//int get_fs_flags(const int fd) {
//  if(auto ret = ::fcntl(fd, F_GETFL, nullptr); ret == -1) {
//    throw std::system_error(
//      std::make_error_code(static_cast<std::errc>(errno)), 
//      "Failed to get file status flags"
//    );
//  } 
//  else {
//    return ret;
//  }
//}

//// Function: make_fd_nonblocking
//void make_fd_nonblocking(const int fd, std::error_code& errc) noexcept {
//  if(auto flags = get_fs_flags(fd, errc); flags != -1) {
//    if(auto ret = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK); ret == -1) {
//      errc = std::make_error_code(static_cast<std::errc>(errno));
//    }
//  }
//}

// Function: make_fd_nonblocking
void make_fd_nonblocking(const int fd) {

  if(auto flags = ::fcntl(fd, F_GETFL, nullptr); flags != -1) {
    if(::fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1) {
      return;
    }
  }
      
  throw std::system_error(
    std::make_error_code(static_cast<std::errc>(errno)),
    "Failed to make fd non-blocking"
  );
}

// Function: is_fd_nonblocking
bool is_fd_nonblocking(const int fd) noexcept {
  if(auto ret = ::fcntl(fd, F_GETFL, nullptr); ret != -1) {
    return (ret & O_NONBLOCK);
  }
  return false;
}

// Procedure: make_fd_blocking
//void make_fd_blocking(const int fd, std::error_code& errc) noexcept {
//  if(auto flags = get_fs_flags(fd, errc); flags != -1) {
//    if(auto ret = ::fcntl(fd, F_SETFL, flags & (~O_NONBLOCK)); ret == -1) {
//      errc = std::make_error_code(static_cast<std::errc>(errno));
//    }
//  }
//}

// Procedure: make_fd_blocking
void make_fd_blocking(const int fd) {

  if(auto flags = ::fcntl(fd, F_GETFL, nullptr); flags != -1) {
    if(::fcntl(fd, F_SETFL, flags & ~(O_NONBLOCK)) != -1) {
      return;
    }
  }
      
  throw std::system_error(
    std::make_error_code(static_cast<std::errc>(errno)),
    "Failed to make fd blocking"
  );
}

// Function: make_fd_close_on_exec
//void make_fd_close_on_exec(const int fd, std::error_code& errc) noexcept {
//  if(auto flags = get_fd_flags(fd, errc); flags != -1) {
//    if(auto ret = ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC); ret == -1) {
//      errc = std::make_error_code(static_cast<std::errc>(errno));
//    }
//  }
//}

// Function: is_fd_blocking
bool is_fd_blocking(const int fd) noexcept {
  if(auto ret = ::fcntl(fd, F_GETFL, nullptr); ret != -1) {
    return !(ret & O_NONBLOCK);
  }
  return false;
}

// Function: make_fd_close_on_exec
void make_fd_close_on_exec(const int fd) {
  if(auto flags = ::fcntl(fd, F_GETFD, nullptr); flags != -1) {
    if(::fcntl(fd, F_SETFD, flags | FD_CLOEXEC) != -1) {
      return;
    }
  }

  throw std::system_error(
    std::make_error_code(static_cast<std::errc>(errno)),
    "Failed to make fd close on exec"
  );
}

// Function: is_fd_close_on_exec
bool is_fd_close_on_exec(const int fd) noexcept {
  if(auto ret = ::fcntl(fd, F_GETFD, nullptr); ret != -1) {
    return (ret & FD_CLOEXEC);
  }
  return false;
}

// Function: make_fd_open_on_exec
//void make_fd_open_on_exec(const int fd, std::error_code& errc) noexcept {
//  if(auto flags = get_fd_flags(fd, errc); flags != -1) {
//    if(auto ret = ::fcntl(fd, F_SETFD, flags & (~FD_CLOEXEC)); ret == -1) {
//      errc = std::make_error_code(static_cast<std::errc>(errno));
//    }
//  }
//}

// Function: make_fd_open_on_exec
void make_fd_open_on_exec(const int fd) {

  if(auto flags = ::fcntl(fd, F_GETFD, nullptr); flags != -1) {
    if(::fcntl(fd, F_SETFD, flags & (~FD_CLOEXEC)) != -1) {
      return;
    }
  }

  throw std::system_error(
    std::make_error_code(static_cast<std::errc>(errno)),
    "Failed to make fd open on exec"
  );
}

// Function: is_fd_open_on_exec
bool is_fd_open_on_exec(const int fd) noexcept {
  if(auto ret = ::fcntl(fd, F_GETFD, nullptr); ret != -1) {
    return !(ret & FD_CLOEXEC);
  }
  return false;
}

// Procedure: make_socket_reuseable
//void make_socket_reuseable(const int sfd, std::error_code& errc) noexcept {
//  if(int one = 1; setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1) {
//    errc = std::make_error_code(static_cast<std::errc>(errno));
//  }
//  else errc.clear();
//}

// Procedure: make_socket_reuseable
void make_socket_reuseable(const int sfd) {
  if(int one = 1; setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)),
      "Failed to make socket reuseable"
    );
  }
}


// Procedure: make_socket_keep_alive
void make_socket_keep_alive(const int sfd) {
  if(int one = 1; setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one)) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)),
      "Failed to keep socket alive"
    );
  }
}

// Function: duplicate_fd
int duplicate_fd(int fd) {
  if(auto newfd = ::dup(fd); newfd == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)),
      "Failed on 'dup'"
    );
  }
  else return newfd;
}

// Procedure: duplicate_fd
void duplicate_fd(int old_fd, int new_fd) {
  if(::dup2(old_fd, new_fd) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "Failed on 'dup2'"
    );
  }
}

// Procedure: redirect_fd
void redirect_fd(int from, int to) {
  using namespace std::literals::string_literals;
  if(::dup2(to, from) == -1 || ::close(to) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)),
      "Failed to redirect "s + std::to_string(from) + "->" +std::to_string(to)
    );
  }
}

// Function: write_all
std::streamsize write_all(int fd, const void* B, std::streamsize N) {
  
  std::streamsize n = 0;
  
  issue_write:

  auto ret = ::write(fd, static_cast<const char*>(B) + n, N - n);

  if(ret > 0) {
    n += ret;
    goto issue_write;
  }
  else if (ret == -1) {
    if(errno == EINTR) {
      goto issue_write;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      throw std::system_error(
        std::make_error_code(static_cast<std::errc>(errno)), "Failed to write all"
      );
    }
  }
  return n;
}
    
// Function: write_all
std::streamsize write_all(int fd, const std::filesystem::path& file) {

  std::streamsize byte_sent {0};

  auto src = ::open(file.c_str(), O_RDONLY | O_CLOEXEC); 
  auto fsz = std::filesystem::file_size(file);

  for(off_t offset = 0; fsz > 0;) {
    if(auto ret = ::sendfile(fd, src, &offset, fsz); ret > 0) {
      fsz -= ret;
      byte_sent += ret;
    }
    else if (ret == -1) {
      if(errno != EAGAIN && errno != EWOULDBLOCK) {
        ::close(src);
        throw std::system_error(
          std::make_error_code(static_cast<std::errc>(errno)), "Failed to sendfile"
        ); 
      }
    }
  }
  ::close(src);

  return byte_sent;
}

//-------------------------------------------------------------------------------------------------

// Procedure: unshare_user_namespace
void unshare_user_namespace() {
  
  using namespace std::literals::string_literals;

  auto euid = ::geteuid();
  auto egid = ::getegid();
  auto pid  = ::getpid();

  if(::unshare(CLONE_NEWUSER) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "unshare failed"
    );
  }

  std::filesystem::path proc("/proc");

  // UID mapping.
  auto uid_map = std::filesystem::path("/proc") / std::to_string(pid) / "uid_map";
  if(std::ofstream ofs(uid_map); ofs) {
    ofs << "0 "s + std::to_string(euid) + " 1";
  }
  else throw std::runtime_error("Failed to open "s + uid_map.c_str());
  
  // GID mapping
  auto setgroups = std::filesystem::path("/proc") / std::to_string(pid) / "setgroups";
  if(std::ofstream ofs(setgroups); ofs) {
    ofs << "deny";
  }
  else throw std::runtime_error("Failed to open "s + setgroups.c_str());

  auto gid_map = std::filesystem::path("/proc") / std::to_string(pid) / "gid_map";
  if(std::ofstream ofs(gid_map); ofs) {
    ofs << "0 "s + std::to_string(egid) + " 1";
  }
  else throw std::runtime_error("Failed to open "s + gid_map.c_str());

  if(::geteuid() != 0 || ::getegid() != 0) {
    throw std::runtime_error("Failed to set uid_map/gid_map");
  }
}

//-------------------------------------------------------------------------------------------------

// Function: cpu_load_average
// The system imposes a maximum of 3 samples, representing
// averages over the last 1, 5, and 15 minutes, respectively.
std::tuple<double, double, double> cpu_load_average(std::error_code& errc) noexcept {
  double val[3];
  if(::getloadavg(val, 3) == -1) {
    errc = std::make_error_code(static_cast<std::errc>(errno));
  }
  else {
    errc.clear();
  }
  return std::make_tuple(val[0], val[1], val[2]);
}

// Function: cpu_load_average
std::tuple<double, double, double> cpu_load_average() {
  double val[3];
  if(::getloadavg(val, 3) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)),
      "Failed to get cpu load"
    );
  }
  return std::make_tuple(val[0], val[1], val[2]);
}

};  // End of namespace dtc. ----------------------------------------------------------------------




