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

#include <dtc/ipc/device.hpp>

namespace dtc {

// Constructor.
Device::Device(int fd) : _fd {fd} {
  if(is_fd_blocking(fd) || is_fd_open_on_exec(fd)) {
    THROW("Device (fd=", fd, ") must be non-blocking and close-on-exec");
  }
}

// Destructor
Device::~Device() {
  ::close(_fd);
}

// Move constructor
Device::Device(Device&& rhs) : _fd {rhs._fd} {
  rhs._fd = -1;
}

// Move assignment
Device& Device::operator = (Device&& rhs) {

  // lhs
  _fd = rhs._fd;

  // rhs
  rhs._fd = -1;

  return *this;
}

void Device::blocking(bool flag) {
  flag ? make_fd_blocking(_fd) : make_fd_nonblocking(_fd);
}

// Procedure: open_on_exec
void Device::open_on_exec(bool flag) {
  flag ? make_fd_open_on_exec(_fd) : make_fd_close_on_exec(_fd);
}

};  // End of namespace dtc. ----------------------------------------------------------------------





