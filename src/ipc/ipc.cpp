/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/ipc/ipc.hpp>

namespace dtc {

// operator
InputStream::operator bool () {
  return BinaryInputPackager(isbuf) == true;
}

// ------------------------------------------------------------------------------------------------

// Destructor
OutputStream::~OutputStream() {

  try {
    osbuf.flush();
  } 
  catch(...) {
  }
  
  if(auto rem = osbuf.out_avail(); rem > 0) {
    LOGW("ostream remain ", rem, " bytes uncleaned");
  }
  
}

// Procedure: _notify
bool OutputStream::_notify() {

  auto r = reactor();
  
  if(_notified || r == nullptr || osbuf._out_avail() == 0) return false;

  _notified = true;
  
  // Here we need promise to avoid deadlock.
  r->thaw(shared_from_this());

  return true;
}

// Function: _remove_on_flush
Event::Signal OutputStream::_remove_on_flush() {
        
  try { 
    osbuf.flush();
  }
  catch(std::system_error& se) {
    LOGW("Failed to flush the ostream on removal (", se.code().message(), ")");
    return Event::REMOVE;
  }

  //if(osbuf.flush() == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
  //  LOGW("Failed to flush the ostream (", strerror(errno), ")");
  //  return Event::REMOVE;
  //}


  std::scoped_lock lock(osbuf._mutex);
  _notified = false;

  //LOGD("out_avail=", osbuf._out_avail());

  return _notify() == false ? Event::REMOVE : Event::DEFAULT;
}

// Procedure: remove_on_flush
void OutputStream::remove_on_flush() { 

  if(auto r = reactor(); r == nullptr) {
    return; 
  }
  else {
    std::scoped_lock lock(osbuf._mutex);

    if(_disabled) return;

    _disabled = true; 

    if(osbuf._out_avail() > 0) {
      r->thaw(shared_from_this());
    }
    else {
      r->remove(shared_from_this());
    }
  }
}


};  // End of namespace dtc. ----------------------------------------------------------------------





