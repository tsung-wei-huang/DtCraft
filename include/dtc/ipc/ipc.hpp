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

#ifndef DTC_IPC_IPC_HPP_
#define DTC_IPC_IPC_HPP_

#include <dtc/ipc/ios.hpp>
#include <dtc/event/reactor.hpp>

namespace dtc {

// Class: InputStream
class InputStream : public ReadEvent {
 
  public:

    InputStreamBuffer isbuf;

    template <typename C>
    InputStream(const std::shared_ptr<Device>&, C&&);

    ~InputStream() = default;
    
    template <typename... T>
    std::streamsize operator()(T&&...);
};

// Constructor.
template <typename C>
InputStream::InputStream(const std::shared_ptr<Device>& device, C&& c) :
  ReadEvent {
    device->fd(),
    [&, c=std::forward<C>(c)] (Event& e) mutable {
      c(*this);
    }
  },
  isbuf {device, nullptr} {
}

// Operator
template <typename... T>
std::streamsize InputStream::operator()(T&&... t) {
  return BinaryInputPackager(isbuf)(std::forward<T>(t)...);
}

//-------------------------------------------------------------------------------------------------

// Class: OutputStream 
class OutputStream : public WriteEvent {

  private:

    bool _notified {false};
    bool _notify();
    
  public:
    
    OutputStreamBuffer osbuf;

    template <typename C>
    OutputStream(const std::shared_ptr<Device>&, C&&);

    ~OutputStream() = default;

    template <typename... T>
    std::streamsize operator()(T&&...);
};

// Constructor.
template <typename C>
OutputStream::OutputStream(const std::shared_ptr<Device>& device, C&& c) :
  WriteEvent {
    device->fd(),
    [&, c=std::forward<C>(c)] (Event& e) mutable {

      assert(_notified == true);

      c(*this);

      // We have to unmark the flag at the very end.
      std::lock_guard lock(osbuf._mutex);
      _notified = false;
      _notify();
    }
  },
  osbuf {device, [&](){_notify();}} {
}

// Operator: ()
template <typename... T>
std::streamsize OutputStream::operator()(T&&... t) {
  return BinaryOutputPackager(osbuf)(std::forward<T>(t)...);
}

// ------------------------------------------------------------------------------------------------

// Function: write_at_once
// The function forces a write of all data in the object item via the given device's underlying 
// synchronization method.
template <typename T>
std::streamsize write_at_once(const std::shared_ptr<Device>& device, T&& item) {
  OutputStreamBuffer osbuf { device, nullptr };  
  BinaryOutputPackager pkger(osbuf);
  pkger(std::forward<T>(item));
  return osbuf.flush();
}

}  // End of namespace dtc. -----------------------------------------------------------------------

#endif









