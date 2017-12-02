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

#include <dtc/ipc/streambuf.hpp>

namespace dtc {

// Constructor
OutputStreamBuffer::OutputStreamBuffer(const std::shared_ptr<Device>& device) noexcept :
  _device {device} {
}

// Destructor
OutputStreamBuffer::~OutputStreamBuffer() {

  try {
    _flush();
  } 
  catch(...) {
    LOGW("Flush failed on osbuf destructor");
  }
  
  if(!_is_local_data()) {
    std::free(_data);
  }
}

// Function: flush
std::streamsize OutputStreamBuffer::flush() {
  std::lock_guard lock(_mutex);
  return _flush();
}

// Function: _flush
std::streamsize OutputStreamBuffer::_flush() {
  auto sz = std::streamsize {0};
  while(_out_avail() > 0) {
    auto ret = _sync();
    if(ret > 0) {
      sz += ret;
    }
    else if(ret == -1) return -1;
  }
  return sz;
}

// Function: _is_local_data
// Check whether the buffer sits in the local buffer 
bool OutputStreamBuffer::_is_local_data() const noexcept {
  return _data == (reinterpret_cast<const char*>(&_size) + sizeof(char));
}

// Function: out_avail
// Return the amount of data in the buffer. The call is thread-safe.
std::streamsize OutputStreamBuffer::out_avail() const {
  std::lock_guard lock(_mutex);
  return _out_avail();
}

// Function: _out_avail
std::streamsize OutputStreamBuffer::_out_avail() const noexcept {
  return _pptr - _pbase;
}

// Function: copy
std::streamsize OutputStreamBuffer::copy(void* data, std::streamsize count) const {
  std::lock_guard lock(_mutex);
  return _copy(data, count);
}

// Function: _copy
std::streamsize OutputStreamBuffer::_copy(void* data, std::streamsize count) const noexcept {
  auto num_bytes = std::min(_pptr - _pbase, count);
  std::memcpy(data, _pbase, num_bytes);
  return num_bytes;
}

// Function: write
// Add data into the buffer. The call is thread-safe.
std::streamsize OutputStreamBuffer::write(const void* s, std::streamsize count) {
  std::lock_guard lock(_mutex);
  return _write(s, count);
}

// Function: _write
// Add data into the buffer. The call is thread-safe.
std::streamsize OutputStreamBuffer::_write(const void* s, std::streamsize count) {

  // Immediate space is not enought where the buffer has to be either enlarged or adjusted.
  if(_epptr < _pptr + count) {

    size_t avail = _pptr - _pbase;
    size_t csize = _is_local_data() ? static_cast<char>(_size.value) : _size.value;
    
    // Case 1: Empty space is enough.
    if(csize >= avail + count) {
      std::memmove(_data, _pbase, avail*sizeof(char));
    }
    // Case 2: Empty space is not enough
    else {
      while(csize < avail + count) {
        csize = csize * 2;
      }
      auto chunk = static_cast<char*>(std::malloc(csize*sizeof(char)));
      assert(chunk != nullptr);

      std::memcpy(chunk, _pbase, avail*sizeof(char));

      if(!_is_local_data()) {
        std::free(_data);
      }
      _data = chunk;
      _size.value = csize;
    }
    _pbase = _data;
    _epptr = _data + csize;
    _pptr = _data + avail;
  }

  std::memcpy(_pptr, s, count*sizeof(char));
  _pptr += count;
  
  // Invoke the callback.
  if(count > 0 && _on_write) {
    _on_write();
  }
  
  // Here we assume write operation should always succeed (unless the memory error).
  return count;
}

// Function: sync
// Flush the output buffer into the underlying file descriptor.
//
// pbase ------------- pptr -------------- epptr
// [     available      ]
//
std::streamsize OutputStreamBuffer::_sync() {
  if(!_device) return -1;
  auto ret = _device->write(_pbase, _pptr - _pbase);
  if(ret > 0) {
    _pbase += ret;
  }
  return ret;
}

// Function: sync
std::streamsize OutputStreamBuffer::sync() {
  std::lock_guard lock(_mutex);
  return _sync();
}

//-------------------------------------------------------------------------------------------------

// Constructor.
InputStreamBuffer::InputStreamBuffer(const std::shared_ptr<Device>& device) noexcept : 
  _device {device} {
}

// Constructor.
InputStreamBuffer::InputStreamBuffer(const OutputStreamBuffer& osbuf) {

  std::lock_guard lock(osbuf._mutex);

  auto count = osbuf._pptr - osbuf._pbase;
  
  // Push to the local buffer
  if(count < _size.value) {
    std::memcpy(_data, osbuf._pbase, count);
    _egptr += count;
  }
  // Accommodate the data through malloc.
  else {
    _size.value = count;
    _data = static_cast<char*>(std::malloc(count*sizeof(char)));
    std::memcpy(_data, osbuf._pbase, count);
    _eback = _data;
    _gptr = _data;
    _egptr = _data + count;
  }
}

// Constructor.
InputStreamBuffer::InputStreamBuffer(OutputStreamBuffer&& osbuf) {
  
  std::lock_guard lock(osbuf._mutex);

  auto count = osbuf._pptr - osbuf._pbase;
  
  if(osbuf._is_local_data()) {
    std::memcpy(_data, osbuf._pbase, count);
    _egptr += count;
  }
  else {
    std::memmove(osbuf._data, osbuf._pbase, count*sizeof(char));
    _size.value = osbuf._size.value;
    _data = osbuf._data;
    _eback = _data;
    _gptr = _data;
    _egptr = _data + count;
  }

  // Reset the pointer of ostream buffer.
  osbuf._size = {sizeof(LocalStreamBuffer) - sizeof(char)};
  osbuf._data  = reinterpret_cast<char*>(&osbuf._size) + sizeof(char);
  osbuf._pbase = osbuf._data;
  osbuf._pptr  = osbuf._data;
  osbuf._epptr = osbuf._data + osbuf._size.value;
}

// Destructor.
InputStreamBuffer::~InputStreamBuffer() {
  if(!_is_local_data()) {
    std::free(_data);
  }
}

bool InputStreamBuffer::_is_local_data() const noexcept {
  return _data == (reinterpret_cast<const char*>(&_size) + sizeof(char));
}

// Function: in_avail
// Return the amount of data in the buffer. The call is thread-safe.
std::streamsize InputStreamBuffer::in_avail() const {
  std::lock_guard lock(_mutex);
  return _in_avail();
}

// Function: _in_avail
std::streamsize InputStreamBuffer::_in_avail() const noexcept {
  return _egptr - _gptr;
}

// Function: copy
std::streamsize InputStreamBuffer::copy(void* data, std::streamsize count) const {
  std::lock_guard lock(_mutex);
  return _copy(data, count);
}

// Function: _copy
std::streamsize InputStreamBuffer::_copy(void* data, std::streamsize count) const noexcept {
  auto num_bytes = std::min(_egptr - _gptr, count);
  std::memcpy(data, _gptr, num_bytes);
  return num_bytes;
}

// Function: sync
std::streamsize InputStreamBuffer::sync() {
  std::lock_guard lock(_mutex);
  return _sync();
}

// Function: _sync
// Synchronize the buffer with the underlying device.
std::streamsize InputStreamBuffer::_sync() {

  if(!_device) return -1;

  size_t csize = _is_local_data() ? static_cast<char>(_size.value) : _size.value;

  // The buffer is already full. We must guarantee at least one slot before calling 
  // device read or otherwise we will run into problem in the zero return value.
  if(_egptr == _eback + csize) {
    
    size_t empty = _gptr - _eback;
    size_t avail = _egptr - _gptr;

    // Reuse the empty region.
    if (empty > csize / 2) {
      std::memmove(_eback, _gptr, avail*sizeof(char));
    }
    // Double the array size.
    else {
      csize = csize * 2;
      auto chunk = static_cast<char*>(std::malloc(csize*sizeof(char)));
      assert(chunk != nullptr);

      std::memcpy(chunk, _gptr, avail);

      if(!_is_local_data()) {
        std::free(_data);
      } 
      _data = chunk;
      _size.value = csize;
    }
    _eback = _data;
    _gptr = _data;
    _egptr = _data + avail;
  }

  // Read the data.
  auto ret = _device->read(_egptr, csize - (_egptr - _eback));

  if(ret > 0) {
    _egptr += ret;
  }

  return ret;
}

// Function: read
// Get "count" data from the buffer into user space.
std::streamsize InputStreamBuffer::read(void* s, std::streamsize count) {
  std::lock_guard lock(_mutex);
  return _read(s, count);
}

// Function: _read
std::streamsize InputStreamBuffer::_read(void* s, std::streamsize count) {
  auto num_read = std::min(count, _egptr - _gptr);
  std::memcpy(s, _gptr, num_read*sizeof(char));
  _gptr += num_read;
  if(num_read > 0 && _on_read) {
    _on_read();
  }
  return num_read;
}

// Function: drop
std::streamsize InputStreamBuffer::drop(std::streamsize count) {
  std::lock_guard lock(_mutex);
  return _drop(count);
}

// Function: _drop
std::streamsize InputStreamBuffer::_drop(std::streamsize count) {
  auto n = std::min(count, _egptr - _gptr);
  _gptr += n;
  return n;
}


};  // End of dtc namespace .----------------------------------------------------------------------


