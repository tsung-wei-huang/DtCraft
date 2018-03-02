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

#ifndef DTC_IPC_IPC_HPP_
#define DTC_IPC_IPC_HPP_

#include <dtc/archive/binary.hpp>
#include <dtc/ipc/streambuf.hpp>
#include <dtc/event/reactor.hpp>

namespace dtc {

// Class: InputStream
class InputStream : public ReadEvent {

  public:

    InputStreamBuffer isbuf;

    template <typename C>
    InputStream(std::shared_ptr<Device>, C&&);

    ~InputStream() = default;
    
    template <typename... T>
    std::streamsize operator()(T&&...);

    operator bool ();
};

// Constructor.
template <typename C>
InputStream::InputStream(std::shared_ptr<Device> device, C&& c) :
  ReadEvent {
    device,
    [this, c=std::forward<C>(c)] (Event& e) mutable {
      if constexpr(std::is_same_v<std::invoke_result_t<C, InputStream&>, Event::Signal>) {
        return c(*this);
      }
      else {
        c(*this);
        return Event::DEFAULT;
      }
    }
  },
  isbuf {device.get(), nullptr} {
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
    
    bool _disabled {false};
    bool _notified {false};
    bool _notify();

    Event::Signal _remove_on_flush();
    
  public:
    
    OutputStreamBuffer osbuf;

    template <typename C>
    OutputStream(std::shared_ptr<Device>, C&&);

    ~OutputStream();

    template <typename... T>
    std::streamsize operator()(T&&...);
    
    void remove_on_flush();
};

// Constructor.
template <typename C>
OutputStream::OutputStream(std::shared_ptr<Device> device, C&& c) :
  WriteEvent {
    device,
    [this, c=std::forward<C>(c)] (Event& e) mutable {

      if(!_disabled) {
      
        assert(_notified == true);

        if constexpr(std::is_same_v<std::invoke_result_t<C, OutputStream&>, Event::Signal>) {
          if(auto s = c(*this); s != Event::DEFAULT) {
            return s;
          }
        }
        else {
          c(*this);
        }

        // We have to unmark the flag at the very end.
        std::lock_guard lock(osbuf._mutex);
        _notified = false;
        _notify();

        return Event::DEFAULT;
      }
      else {
        return _remove_on_flush();
      }
    }
  },
  osbuf {device.get(), [this](){_notify();}} {
}

// Operator: ()
template <typename... T>
std::streamsize OutputStream::operator()(T&&... t) {
  return BinaryOutputPackager(osbuf)(std::forward<T>(t)...);
}


}  // End of namespace dtc. -----------------------------------------------------------------------

#endif









