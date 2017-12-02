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

#include <dtc/ipc/socket.hpp>

namespace dtc {
   
// Constructor
Socket::Socket(const int fd) :
  Device{fd} {
  //make_socket_reuseable(fd);
  //make_socket_keepalive(fd);
}

// Destructor
Socket::~Socket() {
}

// Function: is_connected
bool Socket::is_connected() const {
  // Special case for FreeBSD7, for which send() does not generate SIGPIPE.
  //#if defined(__FreeBSD__) || defined(BSD)
  struct sockaddr junk;
  socklen_t length = sizeof(junk);
  ::memset(&junk, 0, sizeof(junk));
  return ::getpeername(_fd, &junk, &length) == 0;
  //#endif
}

// Function: is_listener
bool Socket::is_listener() const {
  int val;
  socklen_t len = sizeof(val);
  if (::getsockopt(_fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1) {
    return false;
  }
  else if(val) {
    return true;
  }
  else {
    return false;
  }
}

// Function: peer_host
std::pair<std::string, std::string> Socket::peer_host() const {

  socklen_t len;
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];
  int port = -1;
  
  len = sizeof(addr);
  if(::getpeername(_fd, (struct sockaddr*)&addr, &len) == -1) {
    return {"", ""};
  }
  
  // IP4
  if (addr.ss_family == AF_INET) {
    auto s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
  } 
  // IP6
  else {
    auto s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
  } 

  return {std::string(ipstr), std::to_string(port)};
}


// Function: this_host
std::pair<std::string, std::string> Socket::this_host() const {

  socklen_t len;
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];
  int port = -1;
  
  len = sizeof(addr);
  if(::getsockname(_fd, (struct sockaddr*)&addr, &len) == -1) {
    return {"", ""};
  }
  
  // IP4
  if (addr.ss_family == AF_INET) {
    auto s = (struct sockaddr_in *)&addr;
		port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
  } 
  // IP6
  else {
    auto s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
  } 

  return {std::string(ipstr), std::to_string(port)};
}

// Function: accept
std::shared_ptr<Socket> Socket::accept() const {

  struct sockaddr addr;
  socklen_t addrlen = sizeof(addr);

  if(auto cfd = ::accept4(_fd, &addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC); cfd == -1) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), "Accept failed");
  }
  else {
	  return std::make_shared<Socket>(cfd);
  }
}

// Function: read
std::streamsize Socket::read(void* buf, std::streamsize sz) {

  issue_read:
  auto ret = ::recv(_fd, buf, sz, 0);

  // case 1: fail or in-progress
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_read;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      throw std::system_error(make_posix_error_code(errno), "Socket read failed");
    }
  }
  // case 2: eof
  else if(ret == 0 && sz != 0) {
    throw std::system_error(make_posix_error_code(EPIPE), "Socket read failed");
  }

  return ret;
}

// Function: write
std::streamsize Socket::write(const void* buf, std::streamsize sz) {

  issue_write:
  auto ret = ::send(_fd, buf, sz, MSG_NOSIGNAL);
  
  // Case 1: error
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_write;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      throw std::system_error(make_posix_error_code(errno), "Socket write failed");
    }
  }
  
  return ret;
}

//-------------------------------------------------------------------------------------------------

// Function: to_host
std::tuple<std::string, std::string> to_host(struct sockaddr& sa, size_t len) noexcept {
    
  char host[1024];
  char port[32];

  if(sa.sa_family == AF_INET || sa.sa_family == AF_INET6) {
    auto err = ::getnameinfo(
      &sa, len, 
      host, sizeof(host), 
      port, sizeof(port), 
      NI_NUMERICHOST | NI_NUMERICSERV
    );

    if(err) goto fail;
    return {host, port};
  }
  else if(sa.sa_family == AF_UNIX) {
    return {reinterpret_cast<struct sockaddr_un*>(&sa)->sun_path, ""}; 
  }

  fail:
  return {"", ""};
}

// Function: make_domain_pair
std::tuple<std::shared_ptr<Socket>, std::shared_ptr<Socket>> make_domain_pair() {
  if(int fd[2]; ::socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, fd) == -1) {
    throw std::system_error(make_posix_error_code(errno), "Failed to create socketpair");
	}
	else {
    return {std::make_shared<dtc::Socket>(fd[0]), std::make_shared<dtc::Socket>(fd[1])};
  }
}


// Function: make_socket_server_fd
int make_socket_server_fd(std::string_view port, std::error_code& errc) noexcept {

  errc.clear();

  // Memo:
  // struct addrinfo {
  //   int             ai_flags;       // AI_PASSIVE, AI_CANONNAME, etc.
  //   int             ai_family;      // AF_INET, AF_INET6, AF_UNSPEC
  //   int             ai_socktype;    // SOCK_STREAM, SOCK_DGRAM
  //   int             ai_protocol;    // use 0 for "any"
  //   size_t          ai_addrlen;     // size of ai_addr in bytes
  //   struct sockaddr *ai_addr;       // struct sockaddr_in or _in6
  //   char            *ai_canonname;  // full canonical hostname
  //   struct addrinfo *ai_next;       // linked list, next node
  // }
  // 
  // struct sockaddr {
  //   unsigned short si_family;       // address family, AF_XXX
  //   char           sa_data[14];     // 14 bytes of protocol address.
  // }

  int fd {-1};

  struct addrinfo hints;
  struct addrinfo* res {nullptr};
  struct addrinfo* ptr {nullptr};

  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;         // let it fill the host for us.
  
  int one {1};
  int ret;  

  if((ret = ::getaddrinfo(nullptr, port.data(), &hints, &res)) != 0) {
    errc = make_posix_error_code(ret);
    return -1;
  }

  // Try to connect to the first one that is available.
  for(ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
    
    // Ignore undefined ip type.
    if(ptr->ai_family != AF_INET && ptr->ai_family != AF_INET6) {
      goto try_next;
    }
    
    if((fd = ::socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, ptr->ai_protocol)) == -1) {
      errc = make_posix_error_code(errno);
      goto try_next;
    }
    
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if(::bind(fd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      errc = make_posix_error_code(errno);
      goto try_next;
    }

    if(::listen(fd, 128) == -1) {
      errc = make_posix_error_code(errno);
      goto try_next;
    }
    else {
      break;
    }

    try_next:
    
    if(fd != -1) {
      ::close(fd);
      fd = -1;  
    }

  }
  
  ::freeaddrinfo(res);

  // Assign the socket to the underlying event native handle.
  return fd;
}

// Function: make_socket_server_fd
int make_socket_server_fd(std::string_view P) {
  std::error_code errc;
  if(auto fd = make_socket_server_fd(P, errc); errc) {
    throw std::system_error(errc, "Failed to listen to port "s + P.data());
  }
  else return fd;
}

// Function: make_socket_server
std::shared_ptr<Socket> make_socket_server(std::string_view port) {
  return std::make_shared<Socket>(make_socket_server_fd(port));
}

// Function: make_socket_client_fd
int make_socket_client_fd(std::string_view H, std::string_view P, std::error_code& errc) {

  errc.clear();

  struct addrinfo hints;
  struct addrinfo* res {nullptr};
  
  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  
  int ret;
  int fd {-1};
  int tries;
  
  // TODO: change the error code
  if((ret = ::getaddrinfo(H.data(), P.data(), &hints, &res)) != 0) {
    errc = make_posix_error_code(ret);
    return -1;
  }
  
  // Try each internet entry.
  for(auto ptr = res; ptr != nullptr; ptr = ptr->ai_next) {

    // Ignore undefined ip type.
    if(ptr->ai_family != AF_INET && ptr->ai_family != AF_INET6) {
      goto try_next;
    }
    
    if((fd = ::socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, ptr->ai_protocol)) == -1) {
      errc = make_posix_error_code(errno);
      goto try_next;
    }

    tries = 3;

    issue_connect:
    ret = ::connect(fd, ptr->ai_addr, ptr->ai_addrlen);
    
    if(ret == -1) {
      if(errno == EINTR) {
        goto issue_connect;
      }
      else if(errno == EAGAIN && tries--) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        goto issue_connect;
      }
      else if(errno != EINPROGRESS) {
        goto try_next;
      }
      errc = make_posix_error_code(errno);
    }
    
    // Poll the socket. Note that writable return doesn't mean it is connected to the other end.
    if(select_on_write(fd, 5, errc) && !errc) {
      int optval = -1;
      socklen_t optlen = sizeof(optval);
      if(::getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        errc = make_posix_error_code(errno);
        goto try_next;
      }
      if(optval != 0) {
        errc = make_posix_error_code(optval);
        goto try_next;
      }
      break;
    }

    try_next:

    if(fd != -1) {
      ::close(fd);
      fd = -1;  
    }
  }
  
  ::freeaddrinfo(res);

  return fd;
}

// Function: make_socket_client_fd
int make_socket_client_fd(std::string_view H, std::string_view P) {
  std::error_code errc;
  if(auto fd = make_socket_client_fd(H, P, errc); errc) {
    throw std::system_error(errc, "Failed to connect to "s + H.data() + ":" + P.data());
  }
  else return fd;
}

// Function: make_socket_client
std::shared_ptr<Socket> make_socket_client(std::string_view H, std::string_view P) {
  return std::make_shared<Socket>(make_socket_client_fd(H, P));
}

// Function: redirect_to_socket
void redirect_to_socket(
  int tgt, 
  std::string_view H, 
  std::string_view P, 
  std::error_code& errc
) {
  auto fd = make_socket_client_fd(H, P, errc);
  if(fd != -1) {
    if(::dup2(fd, tgt) == -1 || ::close(fd) == -1) {
      errc = std::make_error_code(static_cast<std::errc>(errno));
    }
  }
}

// Function: redirect_to_socket
void redirect_to_socket(int tgt, std::string_view H, std::string_view P) {
  std::error_code errc;
  redirect_to_socket(tgt, H, P, errc);
  if(errc) {
    throw std::system_error(errc, "Failed to redirect to"s + H.data() + ":" + P.data());
  }
}



};  // End of namespace Socket. -------------------------------------------------------------------






