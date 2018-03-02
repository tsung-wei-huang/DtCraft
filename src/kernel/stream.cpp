/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/kernel/stream.hpp>
#include <dtc/kernel/vertex.hpp>

namespace dtc {

//-------------------------------------------------------------------------------------------------
// Stream
//-------------------------------------------------------------------------------------------------

// Constructor
Stream::Stream(key_type k, Vertex* t, Vertex* h) : key {k}, _tail {t}, _head {h} {
}

// Function: is_inter_stream
bool Stream::is_inter_stream() const {
  return (_tail && !_head) || (!_tail && _head);
}

// Function: is_intra_stream
bool Stream::is_intra_stream() const {
  return _tail && _head;
}

// Function: is_inter_stream    
bool Stream::is_inter_stream(std::ios_base::openmode m) const {
  switch(m) {
    case std::ios_base::out:
      return _tail && !_head;
    break;

    case std::ios_base::in:
      return !_tail && _head;
    break;

    default:
      return false;
    break;
  }
}

Event::Signal Stream::operator()(InputStream& is) const {
  if(_on_istream) return _on_istream((*_head)(), is);
  else return Event::DEFAULT;
}

Event::Signal Stream::operator()(OutputStream& os) const {
  if(_on_ostream) return _on_ostream((*_tail)(), os);
  else return Event::DEFAULT;
}

// Function: ostream
std::shared_ptr<OutputStream> Stream::ostream() const {
  if(auto ptr = std::get_if<std::weak_ptr<OutputStream>>(&_writer)) {
    return ptr->lock();
  }
  else return nullptr;
}

// Function: istream
std::shared_ptr<InputStream> Stream::istream() const {
  if(auto ptr = std::get_if<std::weak_ptr<InputStream>>(&_reader)) {
    return ptr->lock();
  }
  else return nullptr;
}

// Function: obridge
std::shared_ptr<Device> Stream::obridge() const {
  if(auto ptr = std::get_if<std::shared_ptr<Device>>(&_writer)) {
    return *ptr;
  }
  else return nullptr;
}

// Function: ibridge
std::shared_ptr<Device> Stream::ibridge() const {
  if(auto ptr = std::get_if<std::shared_ptr<Device>>(&_reader)) {
    return *ptr;
  }
  else return nullptr;
}

// Function: _extract_obridge
std::shared_ptr<Device> Stream::extract_obridge() {
  if(auto ptr = std::get_if<std::shared_ptr<Device>>(&_writer)) {
    return std::move(*ptr);
  }
  return nullptr;
}

// Function: _extract_ibridge
std::shared_ptr<Device> Stream::extract_ibridge() {
  if(auto ptr = std::get_if<std::shared_ptr<Device>>(&_reader)) {
    return std::move(*ptr);
  }
  else return nullptr;
}

// ------------------------------------------------------------------------------------------------

// Constructor
PlaceHolder::PlaceHolder(std::optional<key_type> t, std::optional<key_type> h) : tail {t}, head {h} {
}

};  // end of namespace dtc. ----------------------------------------------------------------------



