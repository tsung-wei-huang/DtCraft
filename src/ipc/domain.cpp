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

#include <dtc/ipc/domain.hpp>

namespace dtc {

// Constructor
Domain::Domain(const int fd) : Device{fd} {
  make_fd_nonblocking(fd);
  make_fd_close_on_exec(fd);
  make_socket_reuseable(fd);
  //keep_socket_alive(fd);
}

// Destructor
Domain::~Domain() {
  ::close(_fd);
}

/*// Function: pop
int Domain::pop() {
  
  auto fd = int {-1};

  std::lock_guard<SpinLock> lock(_rlock);
  
  if(!_rcmsgs.empty()) {
    const auto& cmsg = _rcmsgs.front();
    fd = *reinterpret_cast<int*>(CMSG_DATA(&cmsg));
    _rcmsgs.pop();
  }

  return fd;
}

// Procedure: attach
void Domain::attach(const int fd_to_send) {
  ControlMessage cmsg = {0};
  cmsg.header.cmsg_level = SOL_SOCKET;
  cmsg.header.cmsg_type = SCM_RIGHTS;
  cmsg.header.cmsg_len = CMSG_LEN(sizeof(fd_to_send));
  *reinterpret_cast<int*>(CMSG_DATA(&cmsg)) = fd_to_send;

  std::lock_guard<SpinLock> lock(_wlock);
  _wcmsgs.push(cmsg);
}*/

/*
// Function: _read
std::streamsize Domain::_read(void* buf, const size_t sz) {

  struct msghdr msg;
  struct iovec iov {buf, sz};
  ControlMessage icmsg = {0};

  msg.msg_name = nullptr;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_flags = 0;
  msg.msg_control= &icmsg;
  msg.msg_controllen = sizeof(ControlMessage);

  issue_read:
  auto ret = ::recvmsg(_fd, &msg, 0);

  // Extract the ancillary data. Here it might have zero return.
  bool has_cmsg {false};

  if(auto ptr = CMSG_FIRSTHDR(&msg)) {
    if(ptr->cmsg_len > CMSG_LEN(0)) {
      _rcmsgs.push(icmsg);
      has_cmsg = true;
    }
  }
  
  // Case 1: fail
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_read;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      LOGE("Failed to recvmsg (", strerror(errno), ")");
      insert_iostates(std::ios_base::failbit);
    }
  }
  // Case 2: eof
  else if(ret == 0 && !has_cmsg) {
    insert_iostates(std::ios_base::eofbit);
  }

  return ret;
}

// Function: _write
std::streamsize Domain::_write(const void* buf, const size_t sz) {
  
  std::lock_guard<SpinLock> lock(_wlock);

  // The <sys/socket.h> header shall define the msghdr structure that includes at least 
  // the following members:
  //
  // void          *msg_name        Optional address. 
  // socklen_t      msg_namelen     Size of address. 
  // struct iovec  *msg_iov         Scatter/gather array. 
  // int            msg_iovlen      Members in msg_iov. 
  // void          *msg_control     Ancillary data; see below. 
  // socklen_t      msg_controllen  Ancillary data buffer len. 
  // int            msg_flags       Flags on received message. 
  //

  struct msghdr msg;
  struct iovec iov;
  iov.iov_base = const_cast<void*>(buf);

  msg.msg_name = nullptr;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_flags = 0;
  
  if(!_wcmsgs.empty()) {
    assert(sz != 0);
    msg.msg_control = &_wcmsgs.front();
    msg.msg_controllen = sizeof(ControlMessage);
    iov.iov_len = 1;
  }
  else {
    msg.msg_control = nullptr;
    msg.msg_controllen = 0;
    iov.iov_len = sz;
  }

  issue_write:
  auto ret = ::sendmsg(_fd, &msg, 0);

  if(ret == -1) {

    if(errno == EINTR) {
      goto issue_write;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      LOGE("Failed to sendmsg (", strerror(errno), ")");
      insert_iostates(std::ios_base::failbit);
    }
  }
  else {
    if(!_wcmsgs.empty()) {
      if(ret != 1) {
        LOGF("Fatal error occurred in sending data with control message (ret=", ret, ")");
      }
      _wcmsgs.pop();
    }
  }
  
  return ret;
}

//-------------------------------------------------------------------------------------------------

// Constructor.
DomainReaderBase::DomainReaderBase(const std::shared_ptr<Domain>& device) :
  ReadEventBase {device->fd()},
  _isbuf {device},
  _istream {&_isbuf} {
}

// Operator: ()
void DomainReaderBase::operator()() {

  if(_isbuf.pubsync() == -1) {
    _istream.setstate(std::ios_base::failbit);
  }
  
  if(_isbuf.device()->eof()) {
    _istream.setstate(std::ios_base::eofbit); 
  }

  this->operator()(_istream);
}

//-------------------------------------------------------------------------------------------------

// Constructor.
DomainWriterBase::DomainWriterBase(const std::shared_ptr<Domain>& device) : 
  WriteEventBase {device->fd()},
  _osbuf {device},
  _ostream {&_osbuf} {
}

// Operator: ()
void DomainWriterBase::operator()() {
  
  if(_osbuf.pubsync() == -1) {
    _ostream.setstate(std::ios_base::failbit);
  }

  this->operator()(_ostream);

}
*/

};  // End of namespace dtc. ----------------------------------------------------------------------





