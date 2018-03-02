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


#include <dtc/event/epoll.hpp>

namespace dtc {

// Ctor
Epoll::Epoll() {

  if(_epfd = ::epoll_create1(EPOLL_CLOEXEC); _epfd == -1){
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll epfd create failed");
  }

  if(_timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC); _timerfd == -1){
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll timerfd create failed");
  }

  _recap(_timerfd);

  epoll_event ev;
  ev.data.fd = _timerfd;
  ev.events = EPOLLIN;

  // Add the timerfd event to epoll
  if(auto ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, _timerfd, &ev); ret == -1){
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll_ctl adds timerfd failed");
  }
}

// Destructor
Epoll::~Epoll() {
  // No need to use epoll_ctl + EPOLL_CTL_DEL to remove the event in epoll 
  // as the reactor destructor will close the notify fd first and the fd becomes invalid 
  ::free(_fd2ev[0]);
  ::free(_fd2ev[1]);
  ::free(_event_buf);
  ::close(_timerfd);
  ::close(_epfd);
}

void Epoll::_prepare_epoll(struct timeval tv){
  auto timeout = tv.tv_sec*1000 + tv.tv_usec/1000;
  if( timeout > MAX_EPOLL_TIMEOUT_MSEC ){
    timeout = MAX_EPOLL_TIMEOUT_MSEC;
  }
  else if(tv.tv_sec != 0 || tv.tv_usec != 0){
    struct itimerspec is;
    is.it_interval.tv_sec = tv.tv_sec;    // Interval for periodic timer
    is.it_interval.tv_nsec = tv.tv_usec * 1000;
    is.it_value.tv_sec = 0;       // Initial expiration
    is.it_value.tv_nsec = 0;

    if(::timerfd_settime(_timerfd, 0, &is, nullptr) < 0){
      throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll timerfd_settime failed");
    }
  }
}

void Epoll::_recap(const int fd){

  if(_max_fd < fd){
    _max_fd = fd;
  }

  if(size_t fd_num = _max_fd + 1; _buf_sz < fd_num){

    auto old_cap =_buf_sz;
    _buf_sz = _buf_sz == 0 ? 1 : _buf_sz;

    while(_buf_sz < fd_num){
      _buf_sz *= 2;
    }

    _event_buf = static_cast<epoll_event*>(std::realloc(_event_buf, _buf_sz*sizeof(epoll_event)));
    _fd2ev[0] = static_cast<Event**>(std::realloc(_fd2ev[0], _buf_sz*sizeof(Event*)));
    _fd2ev[1] = static_cast<Event**>(std::realloc(_fd2ev[1], _buf_sz*sizeof(Event*)));
    ::memset((uint8_t*)_fd2ev[0] + old_cap*sizeof(Event*), 0, (_buf_sz-old_cap)*sizeof(Event*));
    ::memset((uint8_t*)_fd2ev[1] + old_cap*sizeof(Event*), 0, (_buf_sz-old_cap)*sizeof(Event*));
  }
}

// Procedure: _insert
// Insert an event into the demux.
void Epoll::_insert(Event* event) {
 
  auto efd = event->device()->fd();

  _recap(efd);

  epoll_event ev;
  ev.data.fd = efd;

  switch(event->type){
    case Event::READ:
      ev.events = EPOLLIN;
      _fd2ev[0][efd] = event;
      break;
    case Event::WRITE:
      ev.events = EPOLLOUT;
      _fd2ev[1][efd] = event;
      break;
    default:
      assert(false);
      break;
  }


  // Try insert first to see whether the event exists or not
  if(_fd2ev[0][efd] != nullptr && _fd2ev[1][efd] != nullptr){
    // If the fd has been registered (A Read & Write event)
    ev.events = (EPOLLIN | EPOLLOUT);
    if(::epoll_ctl(_epfd, EPOLL_CTL_MOD, efd, &ev) == -1){
      throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll_ctl mod failed");
    }
  }
  else{
    // If this is the first time to insert that event 
    if(::epoll_ctl(_epfd, EPOLL_CTL_ADD, efd, &ev) == -1){
      throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll_ctl mod failed");
    }
  }
}

// Remove an event from the demux.
void Epoll::_remove(Event* event) {

  auto efd = event->device()->fd();

  //_recap(efd);
  // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required
  // a non-null pointer in event, even though this argument is ignored.
  // Since Linux 2.6.9, event can be specified as NULL when using
  // EPOLL_CTL_DEL.  Applications that need to be portable to kernels
  // before 2.6.9 should specify a non-null pointer in event.
  
  epoll_event ev;
  ev.data.fd = efd;


  if(_fd2ev[0][efd] != nullptr && _fd2ev[1][efd] != nullptr){
    // A Read & Write event 
    switch(event->type){
      case Event::READ:
        ev.events = EPOLLOUT;
        _fd2ev[0][efd] = nullptr;
      break;
      case Event::WRITE:
        ev.events = EPOLLIN;
        _fd2ev[1][efd] = nullptr;
      break;
      default:
        assert(false);
      break;
    }
    if(auto ret = epoll_ctl(_epfd, EPOLL_CTL_MOD, efd, &ev); ret == -1){
      assert(false);
      throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll_ctl mod failed");
    }
  }
  else{
    if(_fd2ev[0][efd] != nullptr || _fd2ev[1][efd] != nullptr){
      if(epoll_ctl(_epfd, EPOLL_CTL_DEL, efd, &ev) == -1){
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"epoll_ctl del failed");
      }
    }
    _fd2ev[0][efd] = nullptr;
    _fd2ev[1][efd] = nullptr;
  }
}

// TODO(clin99)
void Epoll::_clear(){
  epoll_event ev;  
  for(int i=0;i<_max_fd;++i){
    if(_fd2ev[0][i] != nullptr || _fd2ev[1][i] != nullptr){
      auto efd = _fd2ev[0][i] != nullptr ? _fd2ev[0][i]->device()->fd() : _fd2ev[1][i]->device()->fd();
      epoll_ctl(_epfd, EPOLL_CTL_DEL, efd, &ev);
    }
  }
  _max_fd = _timerfd;
}

};  // End of namespace dtc. ---------------------------------------------------------------



