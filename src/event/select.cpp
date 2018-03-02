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

#include <dtc/event/select.hpp>

namespace dtc {

// Destructor
Select::~Select() {
  std::free(_R[0]);
  std::free(_R[1]);
  std::free(_W[0]);
  std::free(_W[1]);
  std::free(_fd2ev[0]);
  std::free(_fd2ev[1]);
}

// Procedure: _make_pollee
// Apply the bitwise AND on the mask M and input fd mask, and store the result to the 
// output fd mask.
void Select::_make_pollee(uint8_t* dst, const uint8_t* in, const size_t size) {
  auto *e = dst + size;
  while (dst < e) {
    *dst++ = *in++;
  }
}

// Procedure: _clear
void Select::_clear() {
  std::memset(_R[0], 0, _cap[0]);
  std::memset(_W[0], 0, _cap[0]);
  std::memset(_fd2ev[0], 0, _cap[0]*8*sizeof(Event*));
  std::memset(_fd2ev[1], 0, _cap[0]*8*sizeof(Event*));
  _max_fd = -1;
}

// Procedure: _recap
// Adjust the capacity to accommodate the file descriptor fd.
void Select::_recap(const int fd) {
  
  // No need for recap
  if(_max_fd >= fd) return;

  _max_fd = fd;

  size_t tgt_cap = num_masks(fd + 1) * sizeof(fd_mask);

  // Nothing has to be done if the capacity can accommodate the file descriptor.
  if(_cap[0] >= tgt_cap) return;
  
  // Adjust the cap to the next highest power of 2 larger than num_fds
  size_t new_cap = (_cap[0] == 0) ? sizeof(fd_mask) : _cap[0];
  while(new_cap < tgt_cap) new_cap *= 2;
  
  // Adjust and reset the memory chunk.
  _R[0] = static_cast<fd_set*>(std::realloc(_R[0], new_cap));
  _W[0] = static_cast<fd_set*>(std::realloc(_W[0], new_cap));
  _fd2ev[0] = static_cast<Event**>(std::realloc(_fd2ev[0], new_cap*8*sizeof(Event*)));
  _fd2ev[1] = static_cast<Event**>(std::realloc(_fd2ev[1], new_cap*8*sizeof(Event*)));

  ::memset((uint8_t*)_R[0] + _cap[0], 0, new_cap - _cap[0]);
  ::memset((uint8_t*)_W[0] + _cap[0], 0, new_cap - _cap[0]);
  ::memset((uint8_t*)_fd2ev[0] + _cap[0]*8*sizeof(Event*), 0, (new_cap - _cap[0])*8*sizeof(Event*));
  ::memset((uint8_t*)_fd2ev[1] + _cap[0]*8*sizeof(Event*), 0, (new_cap - _cap[0])*8*sizeof(Event*));

  _cap[0] = new_cap;
}

// Procedure: _prepare_select
void Select::_prepare_select() {

  // fd  | bytes
  // -1 => 0
  // 0  => 1
  // 1  => 1
  // ...
  // 7  => 1
  // 8  => 2
  // 9  => 2
  // ...
  // 15 => 2
  // 16 => 3 
  size_t tgt_cap = (_max_fd == -1) ? 0 : (_max_fd) / 8 + 1;

  if(_cap[1] < _cap[0]) {
    _cap[1] = _cap[0];
    _R[1] = static_cast<fd_set*>(std::realloc(_R[1], _cap[1]));
    _W[1] = static_cast<fd_set*>(std::realloc(_W[1], _cap[1]));
  }

  _make_pollee((uint8_t*)_R[1], (const uint8_t*)_R[0], tgt_cap);
  _make_pollee((uint8_t*)_W[1], (const uint8_t*)_W[0], tgt_cap);
}

// Procedure: _insert
// Insert an event into the demux.
void Select::_insert(Event* event) {
 
  // Adjust the capacity in according to the input file descriptor.
  auto efd = event->device()->fd();
  _recap(efd);
  
  // Set the read file descriptor.
  switch(event->type) {

    case Event::READ:
      FD_SET(efd, _R[0]);
      _fd2ev[0][efd] = event;
    break;
  
    case Event::WRITE:
      FD_SET(efd, _W[0]);
      _fd2ev[1][efd] = event;
    break;

    default:
    break;
  };
}

// Procedure: _remove
// Remove an event from the demux.
void Select::_remove(Event* event) {
  
  // Adjust the capacity in according to the input file descriptor.
  auto efd = event->device()->fd();
  _recap(efd);

  // Clear the read file descriptor.
  switch(event->type) {

    case Event::READ:
      FD_CLR(efd, _R[0]);
      _fd2ev[0][efd] = nullptr;
    break;
    
    case Event::WRITE:
      FD_CLR(efd, _W[0]);
      _fd2ev[1][efd] = nullptr;
    break;

    default:
    break;
  };

  // Adjust the max_fd
  while(_max_fd >= 0 && !FD_ISSET(_max_fd, _R[0]) && !FD_ISSET(_max_fd, _W[0])) {
    --_max_fd;
  }
}

/*// Destructor
Select::~Select() {
  std::free(_R[0]);
  std::free(_R[1]);
  std::free(_W[0]);
  std::free(_W[1]);
  std::free(_M[0]);
  std::free(_M[1]);
  std::free(_fd2ev[0]);
  std::free(_fd2ev[1]);
}

// Procedure: _make_pollee
// Apply the bitwise AND on the mask M and input fd mask, and store the result to the 
// output fd mask.
void Select::_make_pollee(uint8_t* dst, const uint8_t* in, const uint8_t* mask, const size_t size) {
  auto *e = dst + size;
  while (dst < e) {
    *dst++ = *in++ & *mask++;
  }
}

// Procedure: _clear
void Select::_clear() {
  std::memset(_R[0], 0, _cap[0]);
  std::memset(_W[0], 0, _cap[0]);
  std::memset(_M[0], 1, _cap[0]);
  std::memset(_M[1], 1, _cap[0]);
  std::memset(_fd2ev[0], 0, _cap[0]*8*sizeof(Event*));
  std::memset(_fd2ev[1], 0, _cap[0]*8*sizeof(Event*));
  _max_fd = -1;
}

// Procedure: _recap
// Adjust the capacity to accommodate the file descriptor fd.
void Select::_recap(const int fd) {
  
  // No need for recap
  if(_max_fd >= fd) return;

  _max_fd = fd;

  size_t tgt_cap = num_masks(fd + 1) * sizeof(fd_mask);

  // Nothing has to be done if the capacity can accommodate the file descriptor.
  if(_cap[0] >= tgt_cap) return;
  
  // Adjust the cap to the next highest power of 2 larger than num_fds
  size_t new_cap = (_cap[0] == 0) ? sizeof(fd_mask) : _cap[0];
  while(new_cap < tgt_cap) new_cap *= 2;
  
  // Adjust and reset the memory chunk.
  _R[0] = static_cast<fd_set*>(std::realloc(_R[0], new_cap));
  _W[0] = static_cast<fd_set*>(std::realloc(_W[0], new_cap));
  _M[0] = static_cast<fd_set*>(std::realloc(_M[0], new_cap));
  _M[1] = static_cast<fd_set*>(std::realloc(_M[1], new_cap));
  _fd2ev[0] = static_cast<Event**>(std::realloc(_fd2ev[0], new_cap*8*sizeof(Event*)));
  _fd2ev[1] = static_cast<Event**>(std::realloc(_fd2ev[1], new_cap*8*sizeof(Event*)));

  ::memset((uint8_t*)_R[0] + _cap[0], 0, new_cap - _cap[0]);
  ::memset((uint8_t*)_W[0] + _cap[0], 0, new_cap - _cap[0]);
  ::memset((uint8_t*)_M[0] + _cap[0], 1, new_cap - _cap[0]);
  ::memset((uint8_t*)_M[1] + _cap[0], 1, new_cap - _cap[0]);
  ::memset((uint8_t*)_fd2ev[0] + _cap[0]*8*sizeof(Event*), 0, (new_cap - _cap[0])*8*sizeof(Event*));
  ::memset((uint8_t*)_fd2ev[1] + _cap[0]*8*sizeof(Event*), 0, (new_cap - _cap[0])*8*sizeof(Event*));

  _cap[0] = new_cap;
}

// Procedure: _prepare_select
void Select::_prepare_select() {

  // fd  | bytes
  // -1 => 0
  // 0  => 1
  // 1  => 1
  // ...
  // 7  => 1
  // 8  => 2
  // 9  => 2
  // ...
  // 15 => 2
  // 16 => 3 
  size_t tgt_cap = (_max_fd == -1) ? 0 : (_max_fd) / 8 + 1;

  if(_cap[1] < _cap[0]) {
    _cap[1] = _cap[0];
    _R[1] = static_cast<fd_set*>(std::realloc(_R[1], _cap[1]));
    _W[1] = static_cast<fd_set*>(std::realloc(_W[1], _cap[1]));
  }

  _make_pollee((uint8_t*)_R[1], (const uint8_t*)_R[0], (const uint8_t*)_M[0], tgt_cap);
  _make_pollee((uint8_t*)_W[1], (const uint8_t*)_W[0], (const uint8_t*)_M[1], tgt_cap);
}

// Procedure: _insert
// Insert an event into the demux.
void Select::_insert(Event* event) {
 
  // Adjust the capacity in according to the input file descriptor.
  _recap(event->descriptor());
  
  // Set the read file descriptor.
  switch(event->type) {

    case Event::READ:
      FD_SET(event->descriptor(), _R[0]);
      FD_SET(event->descriptor(), _M[0]);
      _fd2ev[0][event->descriptor()] = event;
    break;
  
    case Event::WRITE:
      FD_SET(event->descriptor(), _W[0]);
      FD_SET(event->descriptor(), _M[1]);
      _fd2ev[1][event->descriptor()] = event;
    break;

    default:
    break;
  };
}

// Procedure: _remove
// Remove an event from the demux.
void Select::_remove(Event* event) {
  
  // Adjust the capacity in according to the input file descriptor.
  _recap(event->descriptor());

  // Clear the read file descriptor.
  switch(event->type) {

    case Event::READ:
      FD_CLR(event->descriptor(), _R[0]);
      FD_CLR(event->descriptor(), _M[0]);
      _fd2ev[0][event->descriptor()] = nullptr;
    break;
    
    case Event::WRITE:
      FD_CLR(event->descriptor(), _W[0]);
      FD_CLR(event->descriptor(), _M[1]);
      _fd2ev[1][event->descriptor()] = nullptr;
    break;

    default:
    break;
  };

  // Adjust the max_fd
  while(_max_fd >= 0 && !FD_ISSET(_max_fd, _R[0]) && !FD_ISSET(_max_fd, _W[0])) {
    --_max_fd;
  }
}

// Procedure: _freeze
void Select::_freeze(Event* event) {
  
  // Adjust the capacity in according to the input file descriptor.
  _recap(event->descriptor());
  
  // Clear the read file descriptor.
  switch(event->type) {
    case Event::READ:
      FD_CLR(event->descriptor(), _M[0]);
    break;
    
    case Event::WRITE:
      FD_CLR(event->descriptor(), _M[1]);
    break;

    default:
    break;
  };
}

// Procedure: _thaw
void Select::_thaw(Event* event) {
  
  // Adjust the capacity in according to the input file descriptor.
  _recap(event->descriptor());
  
  // Clear the read file descriptor.
  switch(event->type) {
    case Event::READ:
      FD_SET(event->descriptor(), _M[0]);
    break;
    
    case Event::WRITE:
      FD_SET(event->descriptor(), _M[1]);
    break;

    default:
    break;
  };
}*/

//-------------------------------------------------------------------------------------------------

// Function: select_on_write
bool select_on_write(int fd, struct timeval&& tv) {

  if(fd < 0) return false;

  fd_set write_fds;
  fd_set except_fds;

  issue_select:

  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(fd, &write_fds);
  FD_SET(fd, &except_fds);

  if(::select(fd + 1, nullptr, &write_fds, &except_fds, &tv) == -1) {
    if(errno == EINTR) {
      goto issue_select;
    }
    return false;
  }
            
  return FD_ISSET(fd, &write_fds);
} 

// Function: select_on_write 
bool select_on_write(int fd, int timeout, std::error_code& errc) {

  fd_set write_fds;
  fd_set except_fds;

  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  errc.clear();

  issue_select:

  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(fd, &write_fds);
  FD_SET(fd, &except_fds);

  auto ret = ::select(fd + 1, nullptr, &write_fds, &except_fds, &tv);
    
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_select;
    }
    errc = make_posix_error_code(errno);
  }
            
  return FD_ISSET(fd, &write_fds);
}



};  // End of namespace dtc. ---------------------------------------------------------------


