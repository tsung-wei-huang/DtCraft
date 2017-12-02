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

#include <dtc/ipc/fifo.hpp>

namespace dtc {
/*
// Constructor.
FIFO::FIFO() {
}

// Destructor
FIFO::~FIFO() {
  close();
}

// Function: is_open
bool FIFO::is_open() const {
  return _fd != -1;
}

// Procedure: close
void FIFO::close() {
  if(is_open()) {
    ::close(_fd);
    _fd = -1;
  }
}

// Procedure: open
void FIFO::open(const std::string& fpath, const std::ios_base::openmode mode) {
  
  // Read mode.
  if(mode == std::ios_base::in) {
    if((::mkfifo(fpath.c_str(), 0666) < 0) && errno != EEXIST) {
      LOGE("Failed to make an FIFO reader file (", strerror(errno), ")");
      return;
    }
    _fd = ::open(fpath.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC); 
    if(_fd == -1) {
      LOGE("Failed to open an FIFO reader file (", strerror(errno), ")");
      return;
    }
  }
  // Write mode. Open an FIFO for write might fail.
  else if(mode == std::ios_base::out) {
    if((::mkfifo(fpath.c_str(), 0666) < 0) && errno != EEXIST) {
      //LOGE("Failed to make an FIFO writer file (", strerror(errno), ")");
      return;
    }
    _fd = ::open(fpath.c_str(), O_WRONLY | O_NONBLOCK | O_CLOEXEC); 
  }
}

// Function: _read
std::streamsize FIFO::_read(void* buf, const size_t sz) {

  issue_read:
  auto ret = ::read(_fd, buf, sz);
  
  // case 1: fail
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_read;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      insert_iostates(std::ios_base::failbit);
    }
  }
  // case 2: eof
  else if(ret == 0) {
    insert_iostates(std::ios_base::eofbit);
  }

  return ret;
}

// Function: _write
std::streamsize FIFO::_write(const void* buf, const size_t sz) {
    
  issue_write:
  auto ret = ::write(_fd, buf, sz);

  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_write;
    }
    else if(errno != EAGAIN && errno != EWOULDBLOCK) {
      insert_iostates(std::ios_base::failbit);
    }
  }
  
  return ret;
}

//-------------------------------------------------------------------------------------------------

// Constructor.
FIFOConnectorBase::FIFOConnectorBase(const std::string& fpath, const std::ios_base::openmode mode) :
  AsyncEventBase {
    [&]() {
      if(_future.valid()) {
        return _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
      }
      else {
        return false;
      }
    }
  }
{

  // Create a promise and use it as the async predicate.
  std::promise<std::shared_ptr<FIFO>> pm;
  _future = pm.get_future();
  
  std::thread(
    [fpath=fpath, mode=mode, pm=std::move(pm)] () mutable {
      auto fifo = std::make_shared<FIFO>();
      auto tle = now() + Policy::get().DTC_FIFO_CONNECTION_TIMEOUT();
      while(now() < tle) {
        fifo->open(fpath, mode);
        if(fifo->is_open()) break;
        else std::this_thread::yield();
      }
      pm.set_value(fifo->is_open() ? std::move(fifo) : nullptr);
    }
  ).detach();
}

// Operator
void FIFOConnectorBase::operator()() {
  this->operator()(_future.valid() ? _future.get() : nullptr);
}


//-------------------------------------------------------------------------------------------------

// Constructor.
FIFOReaderBase::FIFOReaderBase(const std::shared_ptr<FIFO>& device) :
  ReadEventBase {device->fd()},
  _isbuf {device},
  _istream {&_isbuf} {
}

// Operator: ()
void FIFOReaderBase::operator()() {

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
FIFOWriterBase::FIFOWriterBase(const std::shared_ptr<FIFO>& device) : 
  WriteEventBase {device->fd()},
  _osbuf {device},
  _ostream {&_osbuf} {
}

// Operator: ()
void FIFOWriterBase::operator()() {

  if(_osbuf.pubsync() == -1) {
    _ostream.setstate(std::ios_base::failbit);
  }

  this->operator()(_ostream);
}
*/


};  // End of namespace dtc. --------------------------------------------------------------






