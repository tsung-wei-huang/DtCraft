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

#ifndef DTC_IPC_SOCKET_HPP_
#define DTC_IPC_SOCKET_HPP_

#include <dtc/ipc/device.hpp>
#include <dtc/event/select.hpp>

//// Sockaddr
//struct sockaddr {
//    unsigned short    sa_family;    // address family, AF_xxx
//    char              sa_data[14];  // 14 bytes of protocol address
//};
//
//
//// IPv4 AF_INET sockets:
//struct sockaddr_in {
//    short            sin_family;   // e.g. AF_INET, AF_INET6
//    unsigned short   sin_port;     // e.g. htons(3490)
//    struct in_addr   sin_addr;     // see struct in_addr, below
//    char             sin_zero[8];  // zero this if you want to
//};
//
//struct in_addr {
//    unsigned long s_addr;          // load with inet_pton()
//};
//
//
//// IPv6 AF_INET6 sockets:
//
//struct sockaddr_in6 {
//    u_int16_t       sin6_family;   // address family, AF_INET6
//    u_int16_t       sin6_port;     // port number, Network Byte Order
//    u_int32_t       sin6_flowinfo; // IPv6 flow information
//    struct in6_addr sin6_addr;     // IPv6 address
//    u_int32_t       sin6_scope_id; // Scope ID
//};
//
//struct in6_addr {
//    unsigned char   s6_addr[16];   // load with inet_pton()
//};
//
//
//// General socket address holding structure, big enough to hold either
//// struct sockaddr_in or struct sockaddr_in6 data:
//
//struct sockaddr_storage {
//    sa_family_t  ss_family;     // address family
//
//    // all this is padding, implementation specific, ignore it:
//    char      __ss_pad1[_SS_PAD1SIZE];
//    int64_t   __ss_align;
//    char      __ss_pad2[_SS_PAD2SIZE];
//};
//
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

//-------------------------------------------------------------------------------------------------

namespace dtc {

// Class: Socket
// Basic wrapper for a socket device. A socket is a device that support ::read/::write through the
// network. By default the socket uses TCP stream to perform message passing.
class Socket : public Device {

  public:

    Socket(const int);
    Socket(Socket&&) = delete;
    Socket(const Socket&) = delete;
    
    Socket& operator = (Socket&&) = delete;
    Socket& operator = (const Socket&) = delete;
  
    ~Socket();
    
    bool is_connected() const;
    bool is_listener() const;

    std::shared_ptr<Socket> accept() const;

    std::pair<std::string, std::string> peer_host() const;
    std::pair<std::string, std::string> this_host() const;

    std::streamsize read(void*, std::streamsize) override final;
    std::streamsize write(const void*, std::streamsize) override final;
};

std::tuple<std::string, std::string> to_host(struct sockaddr&, size_t) noexcept;

int make_socket_server_fd(std::string_view, std::error_code&) noexcept;
int make_socket_server_fd(std::string_view);
int make_socket_client_fd(std::string_view, std::string_view, std::error_code&);
int make_socket_client_fd(std::string_view, std::string_view);

std::shared_ptr<Socket> make_socket_server(std::string_view);
std::shared_ptr<Socket> make_socket_client(std::string_view, std::string_view);

std::tuple<std::shared_ptr<Socket>, std::shared_ptr<Socket>> make_domain_pair();

void redirect_to_socket(int, std::string_view, std::string_view, std::error_code&);
void redirect_to_socket(int, std::string_view, std::string_view);

//-------------------------------------------------------------------------------------------------

// Class: SocketListener
// Base class for listener. Notice that the listener can be only registered as READ.
class SocketListener : public ReadEvent {

  private:

    std::shared_ptr<Socket> _listener;
  
  public:
    
		template <typename C>
    SocketListener(std::shared_ptr<Socket>&&, C&&);

    inline const std::shared_ptr<Socket>& get() const;
};

template <typename C>
SocketListener::SocketListener(std::shared_ptr<Socket>&& listener, C&& c) : 
  ReadEvent {
    listener->fd(),
    [&, c=std::forward<C>(c)] (Event& e) {
      c(_listener);
    }
  },
  _listener {std::move(listener)} {
}

// Function: get
inline const std::shared_ptr<Socket>& SocketListener::get() const {
  return _listener;
}


};  // End of namespace Socket. -------------------------------------------------------------------



#endif



