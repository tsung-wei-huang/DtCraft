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

#ifndef DTC_EVENT_EPOLL_HPP_
#define DTC_EVENT_EPOLL_HPP_

#include <dtc/event/event.hpp>
#include <sys/epoll.h>
#include <sys/timerfd.h>

namespace dtc {

// Class: EPOLL
//
// EPOLL allows a program to monitor multiple file descriptors, waiting until one or more of the
// file descriptors become "ready" for some class of IO operations. Below are data structures and 
// function signatures:
// 
// ** Data structures:
//
//	typedef union epoll_data {
//		void    *ptr;
//		int      fd;
//		uint32_t u32;
//		uint64_t u64;
//	} epoll_data_t;
//
//	struct epoll_event {
//		uint32_t     events;    /* Epoll event types: EPOLLIN, EPOLLOUT */
//		epoll_data_t data;      /* User data variable */
//	};
//
//
//  ** Function signatures:   
// 
//  int epoll_create(int size);  
//  * Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
// 
// 
//  int epoll_create1(int flags); 
//  * If flags is 0, then, other than the fact that the obsolete sizea argument is dropped,  
//    epoll_create1() is the same as epoll_create(). The flag can be EPOLL_CLOEXEC: Set the 
//    close-on-exec (FD_CLOEXEC) flag on the new file descriptor.
//  
//  
//  int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//  * Control interface for an epoll file descriptor. The op could be EPOLL_CTL_ADD, 
//    EPOLL_CTL_MOD, EPOLL_CTL_DEL.
//
//    
//	int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
//  * The epoll_wait() system call waits for events on the epoll
//    instance referred to by the  epfd.  The memory area pointed
//    to by events will contain the events that will be available
//    for the caller.  Up to maxevents are returned by epoll_wait().
//    The maxevents must be greater than zero.
//

// TODO: refactor
// 1. move error handling to constructor (timerfd creation, etc.)
// 2. Check max_fd 
// 3. Remove should recap?

class Epoll {

  friend class Reactor;

  ~Epoll();
  Epoll();

  int _max_fd {-1};
  int _epfd {-1};
  int _timerfd {-1};

  size_t _buf_sz {0};
  Event** _fd2ev[2] {nullptr, nullptr};
  epoll_event* _event_buf {nullptr};
  
  template <typename D, typename C>
  void _poll(D&&, C&&);

  void _insert(Event*);
  void _remove(Event*);
  void _prepare_epoll(struct timeval);
  void _recap(const int);
  void _clear();



  // In kernels before 2.6.37, a timeout value larger than approximately
  // LONG_MAX / HZ milliseconds is treated as -1 (i.e., infinity).  Thus,
  // for example, on a system where sizeof(long) is 4 and the kernel HZ
  // value is 1000, this means that timeouts greater than 35.79 minutes
  // are treated as infinity. 
  const int MAX_EPOLL_TIMEOUT_MSEC {2100000}; // 35*60*1000
};

// Procedure: poll
template <typename D, typename C>
void Epoll::_poll(D&& d, C&& on) {

  _prepare_epoll(duration_cast<struct timeval>(std::forward<D>(d)));

  // Wait for ready events.
  // If millisec = -1: block indefinitely. If millisec = 0: return immediately 
  auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  auto avail = ::epoll_wait(_epfd, _event_buf, _max_fd, timeout);

  if(avail == -1){
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), "Epoll failed");
  }

  // Iterate ready events 
  for(auto i=0;i<avail;++i){

    // Ignore timerfd event
    if(_event_buf[i].data.fd == _timerfd){
      continue;
    }

    auto efd = _event_buf[i].data.fd;
    if(_event_buf[i].events & (EPOLLHUP|EPOLLERR)){
      if(_fd2ev[0][efd]){
        on(_fd2ev[0][efd]);
      }
      if(_fd2ev[1][efd]){
        on(_fd2ev[1][efd]);
      }
    }
    else if(_event_buf[i].events & EPOLLIN){
      on(_fd2ev[0][efd]);
    }
    else if(_event_buf[i].events & EPOLLOUT){
      on(_fd2ev[1][efd]);
    }
    else{
      assert(false);
    }
  }
}



};  // End of namespace dtc. ---------------------------------------------------------------


#endif
