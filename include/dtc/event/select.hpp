/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_EVENT_SELECT_HPP_
#define DTC_EVENT_SELECT_HPP_

// Class: Select
//
// Select allows a program to monitor multiple file descriptors, waiting until one or more of the
// file descriptors become "ready" for some class of IO operations. Below are function signarues:
//
//   The time structures involved are defined in <sys/time.h> and look like
//
//   struct timeval {
//     long    tv_sec;         // seconds
//     long    tv_usec;        // microseconds
//   };
//
// int select(int nfds, fd_set *rfds, fd_set *wfds, fd_set *exceptfds, struct timeval *timeout);
// void FD_CLR(int fd, fd_set *set);
// int  FD_ISSET(int fd, fd_set *set);
// void FD_SET(int fd, fd_set *set);
// void FD_ZERO(fd_set *set);
//

#include <dtc/event/event.hpp>

namespace dtc {

// Class: Select
class Select {

  friend class Reactor;

  ~Select();

  inline size_t num_fds_per_mask() const;
  inline size_t num_masks(const size_t) const;
  
  int _max_fd       {-1};
  size_t _cap[2]    {0, 0};               // in/out
  fd_set* _R[2]     {nullptr, nullptr};   // in/out
  fd_set* _W[2]     {nullptr, nullptr};   // in/out
  Event** _fd2ev[2] {nullptr, nullptr};   // read/write
  
  template <typename D, typename C>
  void _poll(D&&, C&&);

  void _prepare_select();
  void _recap(const int);
  void _make_pollee(uint8_t*, const uint8_t*, const size_t);
  void _insert(Event*);
  void _remove(Event*);
  void _clear();
};

// Function: num_fds_per_mask
// Each byte can accommodate eight bits (eight file descriptors).
inline size_t Select::num_fds_per_mask() const {
  return sizeof(fd_mask)*8;
}

// Function: num_masks
// Perform the ceil operation to find the number of required masks for a given
// number of file descriptors.
inline size_t Select::num_masks(const size_t num_fds) const {
  return (num_fds + num_fds_per_mask() - 1) / num_fds_per_mask();
}

// Procedure: poll
template <typename D, typename C>
void Select::_poll(D&& d, C&& on) {
  
  if(_max_fd == -1) return;

  _prepare_select();
  
  // Privatize the storage that is going to be used by the select system call.
  const auto pmax_fd = _max_fd;

  auto tv = duration_cast<struct timeval>(std::forward<D>(d));
  
  // Here the caller goes to the sleep.
  auto ret = ::select(pmax_fd + 1, _R[1], _W[1], nullptr, &tv);

  // Let's move to the next dispatch cycle.
  if(ret == -1) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), "Select failed");
  }

  // Invoke the handler for every active read/write event.
  for(int fd=pmax_fd; fd >= 0; --fd) {
    if(FD_ISSET(fd, _R[1]) && _fd2ev[0][fd]) {
      on(_fd2ev[0][fd]);
    }
    if(FD_ISSET(fd, _W[1]) && _fd2ev[1][fd]) {
      on(_fd2ev[1][fd]);
    }
  }

}
//-------------------------------------------------------------------------------------------------

bool select_on_write(int, struct timeval&&);

template <typename T>
bool select_on_write(int fd, T&& d) {
  return select_on_write(fd, duration_cast<struct timeval>(std::forward<T>(d))); 
}

bool select_on_write(int, int, std::error_code&);

};  // End of namespace dtc. ---------------------------------------------------------------


#endif
