/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_KERNEL_MANAGER_HPP_
#define DTC_KERNEL_MANAGER_HPP_

#include <dtc/ipc/shm.hpp>
#include <dtc/ipc/socket.hpp>
#include <dtc/ipc/ipc.hpp>
#include <dtc/archive/binary.hpp>
#include <dtc/kernel/container.hpp>
#include <dtc/protobuf/protobuf.hpp>
#include <dtc/webui/webui.hpp>

namespace dtc {

// Class: KernelBase
class KernelBase : public Reactor {

  public:

  struct ActorBase {
    std::shared_ptr<InputStream> istream;
    std::shared_ptr<OutputStream> ostream;
  };


    template <typename... T>
    KernelBase(T&&... t);

    inline auto insert_stream(
      std::shared_ptr<Device>&&, 
      std::ios_base::openmode = std::ios_base::in | std::ios_base::out
    );

  protected:

    template <typename A, typename... T>
    auto _insert_actor(const std::shared_ptr<Device>&, T&&...);

    inline auto _insert_stream(
      const std::shared_ptr<Device>&, 
      std::ios_base::openmode = std::ios_base::in | std::ios_base::out
    );

    inline auto _insert_listener(std::string_view);

    std::shared_ptr<SocketListener> _insert_stdout_listener();
    std::shared_ptr<SocketListener> _insert_stderr_listener();
};

// Constructor
template <typename... T>
KernelBase::KernelBase(T&&... args) : Reactor{std::forward<T>(args)...} {
}
    
// Function: _insert_listener    
auto KernelBase::_insert_listener(std::string_view P) {
  return [K=this, P] (auto&& f) mutable {
    return K->_insert<SocketListener>(
      make_socket_server(P),
      [f=std::forward<decltype(f)>(f)] (const std::shared_ptr<Socket>& listener) {
        try {
          f(listener->accept());
        }
        catch(const std::system_error& s) {
          LOGE("Failed to accept a new connection: ", s.what());
        }
      }
    );
  };
}

// Function: _insert_actor    
template <typename A, typename... T>
auto KernelBase::_insert_actor(const std::shared_ptr<Device>& d, T&&... args) {
  return [K=this, d, actor=A(std::forward<T>(args)...)] (auto&&... f) mutable {
    std::tie(actor.istream, actor.ostream) = K->_insert_stream(d)(std::forward<decltype(f)>(f)...);
    return std::move(actor);
  };
}

// Function: insert_stream
auto KernelBase::insert_stream(std::shared_ptr<Device>&& d, std::ios_base::openmode m) {
  return [K=this, d=std::move(d), m] (auto&&... f) mutable {
    return K->promise(
      [K, d=std::move(d), m, f=Functors{std::forward<decltype(f)>(f)...}] () {
        return K->_insert_stream(d, m)(f);
      }
    );
  };
}

// Function: _insert_stream 
auto KernelBase::_insert_stream(const std::shared_ptr<Device>& d, std::ios_base::openmode m) {

  return [K=this, d, m] (auto&&... f) {
    
    auto functors = Functors{std::forward<decltype(f)>(f)...};    
    
    static_assert(std::is_invocable_v<decltype(functors), std::error_code>);

    auto R = (m & std::ios_base::in) ? K->_insert<InputStream>(
      d,
      [=] (InputStream& istream) mutable {
        
        // Synchronized with the underlying data.
        try {
          istream.isbuf.sync();
        }
        catch (const std::system_error& e) {
          functors(e.code());
          return;
        }
        catch (const std::exception& e) {
          LOGF("Unknown istream exception: ", e.what());
        }
        
        // Regular stream
        if constexpr(std::is_invocable_v<decltype(functors), InputStream&>) {
          functors(istream);
        }
        else {
          pb::Protobuf pb;
          while(istream(pb) != -1) {
            std::visit(Functors{[](auto&&){ assert(false); }, functors}, pb);
          }
        }
      }
    ) : nullptr;

    auto W = (m & std::ios_base::out) ? K->_insert<OutputStream>(
      d,
      [=] (OutputStream& ostream) mutable {
        try {
          ostream.osbuf.sync();
        }
        catch (const std::system_error& e) {
          functors(e.code());
          return;
        }
        catch (const std::exception& e) {
          LOGF("Unknown ostream exception: ", e.what());
        }
        
        // Regular stream
        if constexpr(std::is_invocable_v<decltype(functors), OutputStream&>) {
          functors(ostream);
        }
      }
    ) : nullptr;

    return std::make_pair(std::move(R), std::move(W));

  };
}

}; // End of namespace dtc::kernel. ---------------------------------------------------------------

#endif


