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

#ifndef DTC_KERNEL_MANAGER_HPP_
#define DTC_KERNEL_MANAGER_HPP_

#include <dtc/ipc/shm.hpp>
#include <dtc/ipc/socket.hpp>
#include <dtc/ipc/pipe.hpp>
#include <dtc/ipc/ipc.hpp>
#include <dtc/ipc/notifier.hpp>
#include <dtc/ipc/block_file.hpp>
#include <dtc/concurrent/unique_guard.hpp>
#include <dtc/concurrent/shared_guard.hpp>
#include <dtc/archive/binary.hpp>
#include <dtc/kernel/container.hpp>
#include <dtc/protobuf/protobuf.hpp>
#include <dtc/webui/webui.hpp>
#include <dtc/kernel/cgroup.hpp>

namespace dtc {

// Class: KernelBase
class KernelBase : public Reactor {

  public:

  static constexpr auto default_channel_mode = std::ios_base::in | std::ios_base::out;

    template <typename... T>
    KernelBase(T&&... t);
    
    inline auto insert_channel(std::shared_ptr<Device>, std::ios_base::openmode = default_channel_mode);
    inline auto insert_listener(std::string_view);

    std::shared_ptr<ReadEvent> insert_stdout_listener();
    std::shared_ptr<ReadEvent> insert_stderr_listener();
};

// Constructor
template <typename... T>
KernelBase::KernelBase(T&&... args) : Reactor{std::forward<T>(args)...} {
}
    
// Function: insert_listener    
inline auto KernelBase::insert_listener(std::string_view P) {
  return [this, P] (auto&& f) mutable {
    return insert<ReadEvent>(
      make_socket_server(P),
      [f=std::forward<decltype(f)>(f)] (Event& event) {
        try {
          f(std::static_pointer_cast<Socket>(event.device())->accept());
        }
        catch(const std::system_error& s) {
          LOGE("Failed to accept a new connection: ", s.what());
        }
      }
    ).get();
  };
}

// Function: insert_channel
auto KernelBase::insert_channel(std::shared_ptr<Device> d, std::ios_base::openmode m) {

  return [this, d=std::move(d), m] (auto&&... f) {
    
    auto functors = Functors{std::forward<decltype(f)>(f)...};    
    
    auto R = (m & std::ios_base::in) ? insert<InputStream>(
      d,
      [=, pb=pb::Protobuf()] (InputStream& istream) mutable {

        // Synchronized with the underlying data.
        try {
          istream.isbuf.sync();
        }
        catch (const std::system_error& e) {
          pb = pb::BrokenIO {std::ios_base::in, e.code()};
          functors(std::get<pb::BrokenIO>(pb));
          return;
        }
        //catch (const std::exception& e) {
        //  pb = pb::BrokenIO {std::ios_base::in, make_posix_error_code(EIO), e.what()};
        //  functors(std::get<pb::BrokenIO>(pb));
        //  return;
        //}
        
        //if(istream.isbuf.sync() == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        //  pb = pb::BrokenIO {std::ios_base::in, make_posix_error_code(errno)};
        //  functors(std::get<pb::BrokenIO>(pb));
        //  return;
        //}

        
        // Regular stream
        if constexpr(std::is_invocable_v<decltype(functors), InputStream&>) {
          functors(istream);
        }
        else {
          while(istream(pb) != -1) {
            std::visit(Functors{[](auto&&){ assert(false); }, functors}, pb);
          }
        }
      }
    ).get() : nullptr;

    auto W = (m & std::ios_base::out) ? insert<OutputStream>(
      d,
      [=] (OutputStream& ostream) mutable {

        try {
          ostream.osbuf.sync();
        }
        catch (const std::system_error& e) {
          pb::BrokenIO b{std::ios_base::out, e.code()};
          functors(b);
          return;
        }
        //catch (const std::exception& e) {
        //  pb::BrokenIO b{std::ios_base::out, make_posix_error_code(EIO), e.what()};
        //  functors(b);
        //  return;
        //}

        //if(ostream.osbuf.sync() == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        //  pb::BrokenIO b{std::ios_base::out, make_posix_error_code(errno)};
        //  functors(b);
        //  return;
        //}

        
        // Regular stream
        if constexpr(std::is_invocable_v<decltype(functors), OutputStream&>) {
          functors(ostream);
        }
      }
    ).get() : nullptr;

    return std::make_pair(std::move(R), std::move(W));

  };
}

}; // End of namespace dtc::kernel. ---------------------------------------------------------------

#endif


