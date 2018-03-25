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

#include <dtc/device.hpp>

namespace dtc {

// Constructor.
Device::Device(int fd) : _fd {fd} {
  //assert(is_fd_nonblocking(fd) && is_fd_close_on_exec(fd));
}

// Destructor
Device::~Device() {
  if(_fd != -1 && ::close(_fd) == -1) {
    LOGW("Failed to close device fd=", _fd, ": ", strerror(errno));
  }
}

// Function: flush
std::streamsize Device::flush(const void* B, std::streamsize N) const {

  std::streamsize n = 0;
  
  while(n < N) {
    if(auto ret = write(static_cast<const char*>(B) + n, N - n); ret > 0) {
      n += ret;
    }
  }

  return n;
}

// Function: purge
std::streamsize Device::purge(void* B, std::streamsize N) const {
  
  std::streamsize n = 0;

  while(n < N) {
    if(auto ret = read(static_cast<char*>(B) + n, N - n); ret > 0) {
      n += ret;
    }
  }
  
  return n;
}

// Function: read
std::streamsize Device::read(void* buf, std::streamsize sz) const {

  assert(sz != 0);

  issue_read:
  auto ret = ::read(_fd, buf, sz);

  // case 1: fail or in-progress
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_read;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      throw std::system_error(
        std::make_error_code(static_cast<std::errc>(errno)), "Device read failed"
      );
    }
  }
  // case 2: eof
  else if(ret == 0 && sz != 0) {
    //errno = EPIPE;
    //return -1;
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(EPIPE)), "Device read failed"
    );
    return -1;
  }

  return ret;
}

// Function: write
std::streamsize Device::write(const void* buf, std::streamsize sz) const {

  issue_write:
  auto ret = ::write(_fd, buf, sz);
  
  // Case 1: error
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_write;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      throw std::system_error(
        std::make_error_code(static_cast<std::errc>(errno)), "Device write failed"
      );
    }
  }
  
  return ret;
}

Device& Device::blocking(bool flag) {
  flag ? make_fd_blocking(_fd) : make_fd_nonblocking(_fd);
  return *this;
}

// Procedure: open_on_exec
Device& Device::open_on_exec(bool flag) {
  flag ? make_fd_open_on_exec(_fd) : make_fd_close_on_exec(_fd);
  return *this;
}

// ------------------------------------------------------------------------------------------------
    
ScopedOpenOnExec::ScopedOpenOnExec(std::shared_ptr<Device> dev) : _device{std::move(dev)} {
  if(_device) {
    _device->open_on_exec(true);
  }
}

ScopedOpenOnExec::ScopedOpenOnExec(ScopedOpenOnExec&& rhs) : _device {std::move(rhs._device)} {
}

ScopedOpenOnExec::~ScopedOpenOnExec() {
  if(_device) {
    _device->open_on_exec(false);
  }
}
    
// ------------------------------------------------------------------------------------------------

ScopedDeviceRestorer::ScopedDeviceRestorer(std::shared_ptr<Device> dev) : _device{std::move(dev)} {
  if(_device) {
    (*_device).open_on_exec(true).blocking(true);
  }
}

ScopedDeviceRestorer::ScopedDeviceRestorer(ScopedDeviceRestorer&& rhs) : _device {std::move(rhs._device)} {
}

ScopedDeviceRestorer::~ScopedDeviceRestorer() {
  if(_device) {
    (*_device).open_on_exec(false).blocking(false);
  }
}
    

};  // End of namespace dtc. ----------------------------------------------------------------------





