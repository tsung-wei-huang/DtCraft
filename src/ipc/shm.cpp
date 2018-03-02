/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/ipc/shm.hpp>

namespace dtc {

// Constructor
//SharedMemory::SharedMemory(int fd) : Device{fd} {
//  //make_fd_nonblocking(fd);
//  //make_fd_close_on_exec(fd);
//}
//
//// Destructor
//SharedMemory::~SharedMemory() {
//  //::close(_fd);
//}

// Function: _allocate
//std::unique_ptr<SharedMemory::fifo_type, std::function<void(SharedMemory::fifo_type*)>> 
//SharedMemory::_allocate(){
//
//  static std::mutex mtx;
//  static constexpr int sz = 16; 
//
//  static std::array<fifo_type, sz> pool;
//  static std::array<size_t, sz> released;
//  
//  static size_t p {0};
//  static size_t r {0};
//
//  std::lock_guard guard(mtx);
//
//  if( p < sz ){
//    size_t _p = p++;
//    return std::unique_ptr<fifo_type, std::function<void(fifo_type*)>>( 
//      &(pool[_p]), 
//      [&, _p] (fifo_type* ptr) {
//        std::lock_guard guard(mtx);
//        released[r++] = _p;
//      } 
//    );
//  }
//  else if( r > 0 ){
//    size_t _r = --r;
//    return std::unique_ptr<fifo_type, std::function<void(fifo_type*)>>( 
//      &(pool[released[_r]]),
//      [&, idx=released[_r]] (fifo_type* ptr) {
//        std::lock_guard guard(mtx);
//        released[r++] = idx;
//      } 
//    );
//  }
//  else{
//    return std::unique_ptr<fifo_type, std::function<void(fifo_type*)>>( 
//      new fifo_type(), [](fifo_type* ptr) { delete ptr; } 
//    );
//  }
//}

/* 
std::streamsize SharedMemory::read(void* given, std::streamsize sz) {

  const std::streamsize fence = _w_ptr;
  const std::streamsize bufsz = _fifo->size();
  
  // The buffer is empty (similar to EAGAIN and EWOULDBLOCK)
  if ((_r_ptr + 1) % bufsz == fence) {
    return -1;
  }

  // Clear notify
  static uint64_t c;
  assert(::read(_fd, &c, sizeof(c)) == sizeof(c) || errno == EAGAIN);

  std::streamsize num_reads {0};

  if (fence < _r_ptr) {
    if (_r_ptr != bufsz - 1) {
      num_reads = _bufcpy(given, &((*_fifo)[_r_ptr + 1]), std::min(sz, bufsz - _r_ptr - 1));
      _r_ptr += num_reads;
    }
    if (num_reads < sz && fence != 0) {
      _r_ptr = _bufcpy(&(static_cast<char*>(given)[num_reads]), _fifo->data(), std::min(fence, sz - num_reads)) - 1;
      num_reads += _r_ptr + 1;
    }
  } else {
    num_reads = _bufcpy(given, &((*_fifo)[_r_ptr + 1]), std::min(fence - _r_ptr - 1, sz));
    _r_ptr += num_reads;
  }
  
  // Notify again.
  if((_w_ptr - _r_ptr + 1 + bufsz) % bufsz > 0) {
    assert(_notify() == true); 
  }

  return num_reads;

} */

/*// Function: write
std::streamsize SharedMemory::write(const void* given, std::streamsize sz) {

  const std::streamsize fence = _r_ptr;
  const std::streamsize bufsz = _fifo->size();

  // The buffer is full (similar to EAGAIN and EWOULDBLOCK)
  if ((_w_ptr + 1) % bufsz == fence) {
    return -1;
  }

  std::streamsize num_writes = 0;

  if (fence < _w_ptr) {
    if (fence == 0) {
      num_writes = _bufcpy(&((*_fifo)[_w_ptr]), given, std::min(sz, bufsz - _w_ptr - 1));
      _w_ptr += num_writes;
    } 
    else {
      num_writes = _bufcpy(&((*_fifo)[_w_ptr]), given, std::min(sz, bufsz - _w_ptr));
      _w_ptr = (_w_ptr + num_writes) % bufsz;
      if (num_writes < sz) {
        auto given_ptr = static_cast<const char*>(given);
        _w_ptr = _bufcpy(_fifo->data(), &(given_ptr[num_writes]), std::min(fence - 1, sz - num_writes));
        num_writes += _w_ptr;
      }
    }
  } else {
    num_writes = _bufcpy(&((*_fifo)[_w_ptr]), given, std::min(fence - _w_ptr - 1, sz));
    _w_ptr += num_writes;
  }

  if(num_writes > 0) {
    assert(_notify() == true);
  }
  
  return num_writes;
} */

// Function: _notify
//bool SharedMemory::_notify() {
//  static uint64_t c = 1;
//  auto r = ::write(_fd, &c, sizeof(c));
//  return (r == sizeof(c) || errno == EAGAIN);
//}
//
//// Function: read
//std::streamsize SharedMemory::read(void* given, std::streamsize sz) {
//
//  // The buffer is empty (similar to EAGAIN and EWOULDBLOCK)
//  if (_fifo.empty()) {
//    return -1;
//  }
//
//  // Clear notify
//  static uint64_t c;
//  assert(::read(_fd, &c, sizeof(c)) == sizeof(c) || errno == EAGAIN);
//
//  std::streamsize num_reads {0};
//  while(sz > num_reads && _fifo.pop((static_cast<char*>(given)[num_reads]))){
//    ++num_reads;
//  }
//  
//  // Notify again.
//  if(_fifo.size() > 0) {
//    assert(_notify() == true); 
//  }
//
//  return num_reads;
//}
//
//// Function: write
//std::streamsize SharedMemory::write(const void* given, std::streamsize sz) {
//  
//  // The buffer is full (similar to EAGAIN and EWOULDBLOCK)
//  if (_fifo.capacity() == 0) {
//    return -1;
//  }
//
//  std::streamsize num_writes = 0;
//  while(sz > num_writes && _fifo.push((static_cast<const char*>(given)[num_writes]))){
//    ++num_writes;
//  }
//  
//  if(num_writes > 0) {
//    assert(_notify() == true);
//  }
//  return num_writes;
//}
//
////-------------------------------------------------------------------------------------------------
//
//// Function: make_shared_memory
//std::shared_ptr<SharedMemory> make_shared_memory() {
//  if(auto fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK); fd == -1) {
//    throw std::system_error(
//      std::make_error_code(static_cast<std::errc>(errno)), 
//      "Failed to create an event notification for shared memory"
//    );
//  }
//  else {
//    return std::make_shared<SharedMemory>(fd);
//  }
//}



};  // End of namespace dtc. --------------------------------------------------------------









