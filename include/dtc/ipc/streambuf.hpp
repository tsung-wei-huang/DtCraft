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

#ifndef DTC_IPC_STREAMBUF_HPP_
#define DTC_IPC_STREAMBUF_HPP_

#include <dtc/ipc/device.hpp>

namespace dtc {

// Small buffer optimization.
union LocalStreamBuffer {
  std::streamsize value;
  char alignment[32];
};

// OutputStreamBuffer.
class OutputStreamBuffer {

  friend class OutputStream;
  friend class InputStreamBuffer;
  friend class BinaryOutputArchiver;
  friend class BinaryOutputPackager;

  public:

    OutputStreamBuffer() = default;
    
    template <typename C>
    OutputStreamBuffer(const std::shared_ptr<Device>&, C&&);
    
    OutputStreamBuffer(const std::shared_ptr<Device>&) noexcept;

    ~OutputStreamBuffer();

    std::streamsize out_avail() const;
    std::streamsize flush();
    std::streamsize sync();
    std::streamsize write(const void*, std::streamsize);
    std::streamsize copy(void*, std::streamsize) const;

    inline const std::shared_ptr<Device>& device() const noexcept;

    template <typename D>
    void device(D&&) noexcept;

    inline std::string_view string_view() const;

  private:

    mutable std::recursive_mutex _mutex;
    
    std::shared_ptr<Device> _device;
    std::function<void()> _on_write;

    LocalStreamBuffer _size {sizeof(LocalStreamBuffer) - sizeof(char)};
    char* _data  {reinterpret_cast<char*>(&_size) + sizeof(char)};
    char* _pbase {_data};
    char* _pptr  {_data};
    char* _epptr {_data + _size.value};
    
    bool _is_local_data() const noexcept;

    std::streamsize _flush();
    std::streamsize _sync();
    std::streamsize _out_avail() const noexcept;
    std::streamsize _write(const void*, std::streamsize);
    std::streamsize _copy(void*, std::streamsize) const noexcept;
};

template <typename C>
OutputStreamBuffer::OutputStreamBuffer(const std::shared_ptr<Device>& device, C&& c) : 
  _device {device},
  _on_write {std::forward<C>(c)} {
}

inline const std::shared_ptr<Device>& OutputStreamBuffer::device() const noexcept {
  return _device;
}

template <typename D>
void OutputStreamBuffer::device(D&& d) noexcept {
  _device = std::forward<D>(d);
}

inline std::string_view OutputStreamBuffer::string_view() const {
  return {_pbase, static_cast<typename std::string_view::size_type>(_pptr - _pbase)};
}

//-------------------------------------------------------------------------------------------------

// InputStreamBuffer
class InputStreamBuffer {

  friend class InputStream;
  friend class OutputStreamBuffer;
  friend class BinaryInputArchiver;
  friend class BinaryInputPackager;
  
  public:

    InputStreamBuffer() = default;
    InputStreamBuffer(const std::shared_ptr<Device>&) noexcept;
    InputStreamBuffer(const OutputStreamBuffer&);
    InputStreamBuffer(OutputStreamBuffer&&);
    
    template <typename C>
    InputStreamBuffer(const std::shared_ptr<Device>&, C&&);

    ~InputStreamBuffer();

    std::streamsize in_avail() const;
    std::streamsize sync();
    std::streamsize read(void*, std::streamsize);
    std::streamsize copy(void*, std::streamsize) const;
    std::streamsize drop(std::streamsize);

    inline const std::shared_ptr<Device>& device() const noexcept;

    template <typename D>
    void device(D&&) noexcept;

    inline std::string_view string_view() const;
    
  private:  
    
    mutable std::recursive_mutex _mutex;
    
    std::shared_ptr<Device> _device;
    std::function<void()> _on_read;

    LocalStreamBuffer _size {sizeof(LocalStreamBuffer) - sizeof(char)};
    char* _data  {reinterpret_cast<char*>(&_size) + sizeof(char)};
    char* _eback {_data};
    char* _gptr  {_data};
    char* _egptr {_data};
    
    bool _is_local_data() const noexcept;
    
    std::streamsize _in_avail() const noexcept;
    std::streamsize _sync();
    std::streamsize _drop(std::streamsize);
    std::streamsize _read(void*, std::streamsize);
    std::streamsize _copy(void*, std::streamsize) const noexcept;
};
    
template <typename C>
InputStreamBuffer::InputStreamBuffer(const std::shared_ptr<Device>& device, C&& c) :
  _device {device}, _on_read {std::forward<C>(c)} {
}

inline const std::shared_ptr<Device>& InputStreamBuffer::device() const noexcept {
  return _device;
}

template <typename D>
void InputStreamBuffer::device(D&& d) noexcept {
  _device = std::forward<D>(d);
}

inline std::string_view InputStreamBuffer::string_view() const {
  return {_gptr, static_cast<typename std::string_view::size_type>(_egptr - _gptr)};
}

//-------------------------------------------------------------------------------------------------

//// Class: StreamState
//class StreamState {
//
//  private:
//
//    std::ios_base::iostate _state {std::ios_base::goodbit};
//
//  public:
//
//    inline void clear(std::ios_base::iostate s = std::ios_base::goodbit) { _state = s; }
//    inline void setstate(std::ios_base::iostate s) { _state |= s; }
//    inline std::ios_base::iostate rdstate() const { return _state; }
//    inline bool good() const { return _state == std::ios_base::goodbit; }
//    inline bool fail() const { return (_state & (std::ios_base::badbit | std::ios_base::failbit)) != 0; }
//    inline bool eof() const { return (_state & std::ios_base::eofbit) != 0; }
//    inline bool bad() const { return (_state & std::ios_base::badbit) != 0; }
//};

/*
//-------------------------------------------------------------------------------------------------
// The version without leveldb
//-------------------------------------------------------------------------------------------------

// Class: OutputStreamBuffer
// Stream buffer based on OutputStreamBuffer. The streambuffer is used on conjunction with the c++
// std::ostream which support formatted streaming.
template <typename DeviceT>
class OutputStreamBuffer : public std::basic_streambuf<char> {

  private:
    
    static_assert(sizeof(size_t) > sizeof(char_type), "Stack buffer size should be at least 1");
    
    // Small buffer optimization.
    size_t _size {sizeof(size_t) - sizeof(char_type)};
    char_type* _data {reinterpret_cast<char_type*>(&_size) + 1};

    std::shared_ptr<DeviceT> _device;

  public:

    OutputStreamBuffer(const std::shared_ptr<DeviceT>&);
    ~OutputStreamBuffer();

    inline const std::shared_ptr<DeviceT>& device() const;
    
    inline size_t size() const;

  protected:

    inline bool _is_heap_data() const;

    // Function: overflow
    //
    // Virtual function called by other member functions to put a character into the controlled 
    // output sequence without changing the current position.
    // It is called by public member functions such as sputc to write a character when there are 
    // no writing positions available at the put pointer (pptr).
    //
    // return: in case of success, the character put is returned, converted to a value of type 
    //         int_type using member traits_type::to_int_type.
    //         Otherwise, it returns the end-of-file value (EOF) either if called with this value 
    //         as argument c or to signal a failure (some implementations may throw an exception 
    //         instead).
    // 
    int_type overflow(int_type = traits_type::eof()) override final;
    
    // Function: sync
    //
    // Virtual function called by the public member function pubsync to synchronize the contents 
    // in the buffer with those of the associated character sequence.
    // Its default behavior in streambuf is to do nothing and return zero (indicating success), 
    // but derived classes that use intermediate buffers shall override this behavior to properly 
    // synchronize them: filebuf overrides this virtual member function (see filebuf::sync).
    //
    // @return: zero, which indicates success, -1 on failure.
    //
    int sync() override final;   

    // Function: xsputn
    //
    // Writes count characters to the output sequence from the character array whose first element 
    // is pointed to by the first arguemnt. The characters are written as if by repeated calls to 
    // sputc(). Writing stops when either count characters are written or a call to sputc() would 
    // have returned Traits::eof().
    //
    // @param1: pointer to the address
    // @param2: size of the address
    // @return: number of characters successfully written
    //
    std::streamsize xsputn(const char_type*, std::streamsize) override final;
  
};

// Constructor.
template <typename DeviceT>
OutputStreamBuffer<DeviceT>::OutputStreamBuffer(const std::shared_ptr<DeviceT>& device) :
  _device {device} {
  setp(_data, _data + size());
}

// Destructor.
template <typename DeviceT>
OutputStreamBuffer<DeviceT>::~OutputStreamBuffer() {
  while(pptr() != pbase() && sync() != -1);
  if(_is_heap_data()) {
    std::free(_data);
  }
}

// Function: _is_heap_data
template <typename DeviceT>
inline bool OutputStreamBuffer<DeviceT>::_is_heap_data() const {
  return _data != (reinterpret_cast<const char_type*>(&_size) + 1);
}

// Function: device
template <typename DeviceT>
inline const std::shared_ptr<DeviceT>& OutputStreamBuffer<DeviceT>::device() const {
  return _device;
}

// Function: size
template <typename DeviceT>
inline size_t OutputStreamBuffer<DeviceT>::size() const {
  return _is_heap_data() ? _size : static_cast<char_type>(_size);
}

// Function: sync
// Flush the output buffer into the underlying file descriptor.
//
// pbase ------------- pptr -------------- epptr
// [     available      ]
//
template <typename DeviceT>
int OutputStreamBuffer<DeviceT>::sync() {

  auto success = int {0};
  auto remain = pptr() - pbase(); 

  while(remain) {

    auto ret = _device->write(pptr() - remain, remain);

    if(ret <= 0) {
      if(_device->fail()) {
        success = -1;
      }
      break;    
    }

    remain -= ret;
  }

  setp(pptr() - remain, epptr());
  pbump(remain);
  
  return success;
}

// Function: overflow
// Write the given character into the last position (placeholder in the buffer) and advance
// the put pointer. Synchronize with the underlying file descriptor. If no error occurs, a 
// vector-like heuristic is used to enlarge the array by two times.
template <typename DeviceT>
typename OutputStreamBuffer<DeviceT>::int_type OutputStreamBuffer<DeviceT>::overflow(int_type c) {

  if (traits_type::eq_int_type(c, traits_type::eof())) {
    return traits_type::eof();
  }

  assert(pptr() == epptr());

  size_t empty = pbase() - _data;
  size_t avail = pptr() - pbase();
  size_t csize = size();
  
  // Reuse the empty region.
  if (empty > csize / 2) {
    std::memmove(_data, pbase(), avail*sizeof(char_type)); 
  }
  // Double the array size.
  else {
    csize = csize * 2;
    auto chunk = static_cast<char_type*>(std::malloc(csize * sizeof(char_type)));
    std::memcpy(chunk, pbase(), avail*sizeof(char_type));

    if(_is_heap_data()) {
      std::free(_data);
    }

    _data = chunk;
    _size = csize;
  }
    
  setp(_data, _data + csize);
  pbump(avail);
  
  assert(pptr() != epptr());
  
  // write the overflow character
  *pptr() = traits_type::to_char_type(c);
  pbump(1);
  
  // speculative synchronization
  return sync() == -1 ? traits_type::eof() : traits_type::not_eof(c);
}

// Function: xsputn
template <typename DeviceT>
std::streamsize OutputStreamBuffer<DeviceT>::xsputn(const char_type* s, std::streamsize count) {

  // Immediate space is not enough. We have to either readjust the buffer or enlarge
  // the buffer to hold the data.
  if(epptr() - pptr() < count) {

    size_t avail = pptr() - pbase();
    size_t csize = size();

    // Case 1: Empty space is enough.
    if(static_cast<std::streamsize>(csize - avail) >= count) {
      std::memmove(_data, pbase(), avail*sizeof(char_type));
    }
    // Case 2: Empty space is not enough
    else {
      while(static_cast<std::streamsize>(csize - avail) < count) {
        csize = csize * 2;
      }
      auto chunk = static_cast<char_type*>(std::malloc(csize*sizeof(char_type)));
      std::memcpy(chunk, pbase(), avail*sizeof(char_type));

      if(_is_heap_data()) {
        std::free(_data);
      }
      _data = chunk;
      _size = csize;
    }
    setp(_data, _data + csize);
    pbump(avail);
  }
    
  std::memcpy(pptr(), s, count*sizeof(char_type));
  pbump(count);

  return count;
}

//-------------------------------------------------------------------------------------------------

// Class: InputStreamBuffer
// Input streambuffer based on std::basic_streambuf<char>. The streambuffer is used in conjunction
// with the c++ std::istream which supports formatted extraction.
template <typename DeviceT>
class InputStreamBuffer : public std::basic_streambuf<char> {

  private:
    
    static_assert(sizeof(size_t) > sizeof(char_type), "Stack buffer size should be at least 1");
    
    size_t _size {sizeof(size_t) - sizeof(char_type)};
    char_type* _data {reinterpret_cast<char_type*>(&_size) + 1};

    std::shared_ptr<DeviceT> _device;

  public:

    InputStreamBuffer(const std::shared_ptr<DeviceT>&);
    ~InputStreamBuffer();

    inline const std::shared_ptr<DeviceT>& device() const;

    inline size_t size() const;

  protected:
    
    inline bool _is_heap_data() const;

    // Function: sync
    //
    // Virtual function called by the public member function pubsync to synchronize the contents 
    // in the buffer with those of the associated character sequence.
    // Its default behavior in streambuf is to do nothing and return zero (indicating success), 
    // but derived classes that use intermediate buffers shall override this behavior to properly 
    // synchronize them: filebuf overrides this virtual member function (see filebuf::sync).
    //
    // @return: zero, which indicates success, -1 on failure.
    //
    int sync() override final;
    
    // Function: underflow
    //
    // Ensures that at least one character is available in the input area by updating the pointers 
    // to the input area (if needed) and reading more data in from the input sequence (if 
    // applicable). Returns the value of that character (converted to int_type with 
    // Traits::to_int_type(c)) on success or Traits::eof() on failure.
    //
    // @return: the first character pointed by gptr if any or eof otherwise
    //
    int_type underflow() override final;
    
    // Function: xsgetn
    //
    // Reads count characters from the input sequence and stores them into a character array 
    // pointed to by s. The characters are read as if by repeated calls to sbumpc(). 
    //
    // @param1: pointer to the address
    // @param1: size of the address
    // @return: number of bytes successfully read.
    //
    std::streamsize xsgetn(char_type*, std::streamsize) override final;
};

// Constructor
template <typename DeviceT>
InputStreamBuffer<DeviceT>::InputStreamBuffer(const std::shared_ptr<DeviceT>& device) :
  _device {device} {
  setg(_data, _data, _data);
}
  
// Destructor
template <typename DeviceT>
InputStreamBuffer<DeviceT>::~InputStreamBuffer() {  
  if(_is_heap_data()) {
    std::free(_data);
  }
}

// Function: _is_heap_data
template <typename DeviceT>
inline bool InputStreamBuffer<DeviceT>::_is_heap_data() const {
  return _data != (reinterpret_cast<const char_type*>(&_size) + 1);
}

// Function: size
template <typename DeviceT>
inline size_t InputStreamBuffer<DeviceT>::size() const {
  return _is_heap_data() ? _size : static_cast<char_type>(_size);
}

// Function: device
template <typename DeviceT>
inline const std::shared_ptr<DeviceT>& InputStreamBuffer<DeviceT>::device() const {
  return _device;
}

// Function: sync
// Synchronize with the underlying device by reading data to the buffer. The buffer is 
// automatically increased by two times if necessary.
//
// eback ---------- gptr ---------- egptr
//                   |    available   |
//
template <typename DeviceT>
int InputStreamBuffer<DeviceT>::sync() {

  auto success = int {0};
  
  while(1) {
      
    size_t csize = size();

    if (egptr() == eback() + csize) {
    
      size_t empty = gptr() - eback();
      size_t avail = egptr() - gptr();

      // Reuse the empty region.
      if (empty > csize / 2) {
        std::memmove(eback(), gptr(), avail*sizeof(char_type));
      }
      // Double the array size.
      else {
        csize = csize * 2;
        auto chunk = static_cast<char_type*>(std::malloc(csize*sizeof(char_type)));
        std::memcpy(chunk, gptr(), avail);

        if(_is_heap_data()) {
          std::free(_data);
        } 

        _data = chunk;
        _size = csize;
      }
      setg(_data, _data, _data + avail);
    }

    // Read the data.
    auto ret = _device->read(egptr(), csize - (egptr() - eback()));

    if(ret <= 0) {
      if(_device->fail()) {
        success = -1;
      }
      break;
    }

    setg(eback(), gptr(), egptr() + ret);
  }

  return success;
}

// Function: underflow
template <typename DeviceT>
typename InputStreamBuffer<DeviceT>::int_type InputStreamBuffer<DeviceT>::underflow() {
  if(sync() == -1 || gptr() == egptr()) return traits_type::eof();
  return *(gptr());
}

// Function: xsgetn
template <typename DeviceT>
std::streamsize InputStreamBuffer<DeviceT>::xsgetn(char_type* s, std::streamsize count) {
  auto num_read = std::min(count, egptr() - gptr());
  std::memcpy(s, gptr(), num_read*sizeof(char_type));
  setg(eback(), gptr() + num_read, egptr());
  return num_read;
}
*/

//-------------------------------------------------------------------------------------------------
// The version with leveldb
//-------------------------------------------------------------------------------------------------

/*// Class: OutputStreamBuffer
// Stream buffer based on OutputStreamBuffer. The streambuffer is used on conjunction with the c++
// std::ostream which support formatted streaming.
template <typename DeviceT>
class OutputStreamBuffer : public std::basic_streambuf<char> {

  private:
    
    constexpr static size_t _STACK_BUFSZ = 16;

    static_assert(_STACK_BUFSZ >= 1, "Stack buffer size should be at least 1");
    
    char_type  _stk [_STACK_BUFSZ];
    char_type* _buf {_stk};

    size_t _size {_STACK_BUFSZ};

    std::shared_ptr<DeviceT> _device;
    
    leveldb::DB* _iosdb {nullptr};
    
    const std::string _kbase;

    size_t _kcursor {0};
    size_t _num_kvs {0};

  public:

    OutputStreamBuffer(const std::shared_ptr<DeviceT>&);
    ~OutputStreamBuffer();

    inline const std::shared_ptr<DeviceT>& device() const;

  protected:

    // Function: overflow
    //
    // Virtual function called by other member functions to put a character into the controlled 
    // output sequence without changing the current position.
    // It is called by public member functions such as sputc to write a character when there are 
    // no writing positions available at the put pointer (pptr).
    //
    // return: in case of success, the character put is returned, converted to a value of type 
    //         int_type using member traits_type::to_int_type.
    //         Otherwise, it returns the end-of-file value (EOF) either if called with this value 
    //         as argument c or to signal a failure (some implementations may throw an exception 
    //         instead).
    // 
    int_type overflow(int_type = traits_type::eof()) override;
    
    // Function: sync
    //
    // Virtual function called by the public member function pubsync to synchronize the contents 
    // in the buffer with those of the associated character sequence.
    // Its default behavior in streambuf is to do nothing and return zero (indicating success), 
    // but derived classes that use intermediate buffers shall override this behavior to properly 
    // synchronize them: filebuf overrides this virtual member function (see filebuf::sync).
    //
    // @return: zero, which indicates success, -1 on failure.
    //
    int sync() override;   

    // Function: xsputn
    //
    // Writes count characters to the output sequence from the character array whose first element 
    // is pointed to by the first arguemnt. The characters are written as if by repeated calls to 
    // sputc(). Writing stops when either count characters are written or a call to sputc() would 
    // have returned Traits::eof().
    //
    // @param1: pointer to the address
    // @param2: size of the address
    // @return: number of characters successfully written
    //
    std::streamsize xsputn(const char_type*, std::streamsize) override;
  
};

// Constructor.
template <typename DeviceT>
OutputStreamBuffer<DeviceT>::OutputStreamBuffer(const std::shared_ptr<DeviceT>& device) :
  _device {device},
  _iosdb {Policy::get().iosdb()},
  _kbase {Policy::get().next_iosdb_kbase()}{
  setp(_buf, _buf + _size);
}

// Destructor.
template <typename DeviceT>
OutputStreamBuffer<DeviceT>::~OutputStreamBuffer() {
  while(pptr() != pbase() && sync() != -1);
  if(_buf != _stk) {
    std::free(_buf);
  }
}

// Function: device
template <typename DeviceT>
inline const std::shared_ptr<DeviceT>& OutputStreamBuffer<DeviceT>::device() const {
  return _device;
}

// Function: sync
// Flush the output buffer into the underlying file descriptor.
//
// pbase ------------- pptr -------------- epptr
// [     available      ]
//
template <typename DeviceT>
int OutputStreamBuffer<DeviceT>::sync() {
  
  auto success = int {0};
  auto remain = pptr() - pbase(); 

  while (remain) {
    auto ret = _device->write(pptr() - remain, remain);
    if(ret <= 0) {
      if(_device->fail()) {
        success = -1;
      }
      break;    
    }
    remain -= ret;
  };

  setp(pptr() - remain, _buf + _size );
  pbump(remain);

  // Now we are going to synchronize more data from the leveldb to the device, as long
  // as the streambuffer (remain=0) is empty.
  if( success != -1 && remain == 0 && _num_kvs > 0 ){
     setp(_buf, _buf + _size);
     auto stop {false};

     while(!stop && _num_kvs > 0 ){

       leveldb::Iterator *it = _iosdb->NewIterator(leveldb::ReadOptions());
       it->Seek(_kbase + std::to_string(_kcursor-_num_kvs));
       
       auto v_cursor = size_t{0};

       assert(it->value().size() <= _size);
        
       // For a key
       while( v_cursor != it->value().size() ){
         auto v_remain = it->value().size() - v_cursor;
         auto ret = _device->write(&(it->value().data()[v_cursor]), v_remain);

         if(ret <= 0) {
           if(_device->fail()) {
             success = -1;
           }
           
           // copy the partial data to the end of the streambuffer. 
           setp( _buf + _size - v_remain, _buf + _size );
           memcpy( pptr() , &(it->value().data()[v_cursor]) , v_remain );
           pbump(v_remain);

           stop = true;
           break;    
         }

         v_cursor += ret;
       }

       --_num_kvs;
       _iosdb->Delete( leveldb::WriteOptions(), it->key() );
       delete it;
     }
  }
  
  return success;

}

// Function: overflow
// Write the given character into the last position (placeholder in the buffer) and advance
// the put pointer. Synchronize with the underlying file descriptor. If no error occurs, a 
// vector-like heuristic is used to enlarge the array by two times.
template <typename DeviceT>
typename OutputStreamBuffer<DeviceT>::int_type OutputStreamBuffer<DeviceT>::overflow(int_type c) {
  
  if (traits_type::eq_int_type(c, traits_type::eof())) {
    return traits_type::eof();
  }

  assert(pptr() == epptr());

  size_t avail = pptr() - pbase();

  // Enlarge linear array if the size is less than Policy::get().DTC_MAX_STREAMBUF_SIZE()
  if( _size < Policy::get().DTC_MAX_STREAMBUF_SIZE() ){
    _size = _size * 2;
    auto chunk = static_cast<char_type*>(std::malloc(_size * sizeof(char_type)));
    std::memcpy(chunk, pbase(), avail*sizeof(char_type));
    if (_buf != _stk) std::free(_buf);
    _buf = chunk;

    setp(_buf, _buf + _size);
    pbump(avail);
    assert(pptr() != epptr());
    // Write the overflow character.
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
  }
  else{
    char_type ct = traits_type::to_char_type(c);

    _iosdb->Put( 
      leveldb::WriteOptions(), 
      _kbase + std::to_string(_kcursor), 
      leveldb::Slice( &ct, sizeof(char_type))
    );

    ++_kcursor;
    ++_num_kvs;
  }

  
  // Look-ahead synchronization
  return sync() == -1 ? traits_type::eof() : traits_type::not_eof(c);
}

// Function: xsputn
// Similar to write system call, xsputn allows caller to write a chunk of data into the streambuffer.
template <typename DeviceT>
std::streamsize OutputStreamBuffer<DeviceT>::xsputn(const char_type* s, std::streamsize count) {
  
  // Leveldb has no data and the space from pptr of streambuffer can hold the data.
  if( epptr() - pptr() >= count && _num_kvs == 0 ){
    std::memcpy(pptr(), s, count*sizeof(char_type));
    pbump(count);
  }
  // Leveldb has no data and the total space of streambuffer can hold the data.
  else if( _size - (pptr()-pbase()) >= static_cast<size_t>(count)  && _num_kvs == 0  && pbase() != _buf  ){
    size_t avail = pptr() - pbase();
    std::memmove(_buf, pbase(), avail*sizeof(char_type));
    setp(_buf, _buf + _size);
    pbump(avail);

    std::memcpy(pptr(), s, count*sizeof(char_type));
    pbump(count);
  }
  // Space is not enough 
  else{
    if( _size < Policy::get().DTC_MAX_STREAMBUF_SIZE() ){
      size_t avail = pptr() - pbase();
      // Case 1: Empty space is enough.
      if(static_cast<std::streamsize>(_size - avail) >= count) {
        std::memmove(_buf, pbase(), avail*sizeof(char_type));
      }
      // Case 2: Empty space is not enough
      else {
        while(static_cast<std::streamsize>(_size - avail) < count) _size = _size * 2;
        auto chunk = static_cast<char_type*>(std::malloc(_size*sizeof(char_type)));
        std::memcpy(chunk, pbase(), avail*sizeof(char_type));
        if(_buf != _stk) std::free(_buf);
        _buf = chunk;
      }
      setp(_buf, _buf + _size);
      pbump(avail);

      std::memcpy(pptr(), s, count*sizeof(char_type));
      pbump(count);
    }
    else{
      for(std::streamsize i = 0 ; i < count ; i+=Policy::get().DTC_MAX_STREAMBUF_SIZE()){
        _iosdb->Put( 
          leveldb::WriteOptions(), 
          _kbase + std::to_string(_kcursor), 
          leveldb::Slice(
            &(s[i]), 
            i + Policy::get().DTC_MAX_STREAMBUF_SIZE() < static_cast<size_t>(count) ? 
            Policy::get().DTC_MAX_STREAMBUF_SIZE() : count - i 
          ) 
        );
        ++_kcursor;
        ++_num_kvs;
      }

      // pbase ------------- pptr -------------- epptr
      // [     available      ]
      // Align linear array to right (pptr = epptr)
      if( pptr() != epptr() ){
        auto avail = pptr() - pbase();
        std::memmove( epptr() - avail , pbase(), avail );
        setp( epptr() - avail , epptr() );
        pbump( avail );
      }

    }
  }
  return count;

}

//-------------------------------------------------------------------------------------------------

// Class: InputStreamBuffer
// Input streambuffer based on std::basic_streambuf<char>. The streambuffer is used in conjunction
// with the c++ std::istream which supports formatted extraction.
template <typename DeviceT>
class InputStreamBuffer : public std::basic_streambuf<char> {

  private:
    
    constexpr static size_t _STACK_BUFSZ = 16;
    
    static_assert(_STACK_BUFSZ >= 1, "Stack buffer size should be at least 1");
    
    char_type  _stk [_STACK_BUFSZ];
    char_type* _buf {_stk};

    size_t _size {_STACK_BUFSZ};

    std::shared_ptr<DeviceT> _device;

  public:

    InputStreamBuffer(const std::shared_ptr<DeviceT>&);
    ~InputStreamBuffer();
    
    inline const std::shared_ptr<DeviceT>& device() const;

  protected:

    // Function: sync
    //
    // Virtual function called by the public member function pubsync to synchronize the contents 
    // in the buffer with those of the associated character sequence.
    // Its default behavior in streambuf is to do nothing and return zero (indicating success), 
    // but derived classes that use intermediate buffers shall override this behavior to properly 
    // synchronize them: filebuf overrides this virtual member function (see filebuf::sync).
    //
    // @return: zero, which indicates success, -1 on failure.
    //
    int sync() override;
    
    // Function: underflow
    //
    // Ensures that at least one character is available in the input area by updating the pointers 
    // to the input area (if needed) and reading more data in from the input sequence (if 
    // applicable). Returns the value of that character (converted to int_type with 
    // Traits::to_int_type(c)) on success or Traits::eof() on failure.
    //
    // @return: the first character pointed by gptr if any or eof otherwise
    //
    int_type underflow() override;
    
    // Function: xsgetn
    //
    // Reads count characters from the input sequence and stores them into a character array 
    // pointed to by s. The characters are read as if by repeated calls to sbumpc(). 
    //
    // @param1: pointer to the address
    // @param1: size of the address
    // @return: number of bytes successfully read.
    //
    std::streamsize xsgetn(char_type*, std::streamsize) override;
};

// Constructor
template <typename DeviceT>
InputStreamBuffer<DeviceT>::InputStreamBuffer(const std::shared_ptr<DeviceT>& device) :
  _device {device} {
  setg(_buf, _buf, _buf);
}
  
// Destructor
template <typename DeviceT>
InputStreamBuffer<DeviceT>::~InputStreamBuffer() {  
  if(_buf != _stk) {
    std::free(_buf);
  }
}

// Function: device
template <typename DeviceT>
inline const std::shared_ptr<DeviceT>& InputStreamBuffer<DeviceT>::device() const {
  return _device;
}

// Function: sync
// Synchronize with the underlying device by reading data to the buffer. The buffer is 
// automatically increased by two times if necessary.
//
// eback ---------- gptr ---------- egptr
//                   |    available   |
//
template <typename DeviceT>
int InputStreamBuffer<DeviceT>::sync() {

  auto success = int {0};
  auto stop = bool {false};

  while(!stop) {
    
    // no more data can be appended to the end.
    if (egptr() == eback() + _size) {
    
      size_t empty = gptr() - eback();
      size_t avail = egptr() - gptr();

      // Reuse the empty region.
      if (empty > _size / 2) {
        std::memmove(eback(), gptr(), avail*sizeof(char_type));
      }
      // Double the array size.
      else if(_size < Policy::get().DTC_MAX_STREAMBUF_SIZE()) {
        _size = _size * 2;
        auto chunk = static_cast<char_type*>(std::malloc(_size*sizeof(char_type)));
        std::memcpy(chunk, gptr(), avail);
        if (_buf != _stk) std::free(_buf);
        _buf = chunk;
      }
			else {
				stop = true;
      }

			if( !stop ) {
        setg(_buf, _buf, _buf + avail);
      }
    }

    // Read the data.
    auto ret = _device->read(egptr(), _size - (egptr() - eback()));

    if(ret <= 0) {
      if(_device->fail()) {
        success = -1;
      }
      break;
    }

    setg(eback(), gptr(), egptr() + ret);
  }

  return success;
}

// Function: underflow
template <typename DeviceT>
typename InputStreamBuffer<DeviceT>::int_type InputStreamBuffer<DeviceT>::underflow() {
  if(sync() == -1 || gptr() == egptr()) return traits_type::eof();
  return *(gptr());
}

// Function: xsgetn
template <typename DeviceT>
std::streamsize InputStreamBuffer<DeviceT>::xsgetn(char_type* s, std::streamsize count) {
  auto num_read = std::min(count, egptr() - gptr());
  std::memcpy(s, gptr(), num_read*sizeof(char_type));
  setg(eback(), gptr() + num_read, egptr());
  return num_read;
} */


};  // End of namespace dtc. --------------------------------------------------------------

#endif












