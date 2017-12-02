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

#include <dtc/kernel/manager.hpp>

namespace dtc {

// Function: _insert_stderr_listener
std::shared_ptr<SocketListener> KernelBase::_insert_stderr_listener() {

  assert(is_owner());

  auto L = _insert_listener(Policy::get().STDERR_LISTENER_PORT())(
    [K=this] (std::shared_ptr<Socket>&& skt) {
      K->insert<ReadEvent>(
        skt->fd(),
        [K, skt, buffer=std::array<char, BUFSIZ>()] (Event& e) mutable {
          auto NR = ::read(skt->fd(), buffer.data(), BUFSIZ);
          if(NR <= 0 || ::write(STDERR_FILENO, buffer.data(), NR) != NR) {
            K->remove(e.shared_from_this());
          }
        }
      );
    }
  );

  Policy::get().STDERR_LISTENER_PORT(std::get<1>(L->get()->this_host()));

  return L;
}

// Function: _insert_stdout_listener
std::shared_ptr<SocketListener> KernelBase::_insert_stdout_listener() {

  assert(is_owner());
  
  auto L = _insert_listener(Policy::get().STDOUT_LISTENER_PORT())(
    [K=this] (std::shared_ptr<Socket>&& skt) {
      K->insert<ReadEvent>(
        skt->fd(),
        [K, skt, buffer=std::array<char, BUFSIZ>()] (Event& e) mutable {
          auto NR = ::read(skt->fd(), buffer.data(), BUFSIZ);
          if(NR <= 0 || ::write(STDOUT_FILENO, buffer.data(), NR) != NR) {
            K->remove(e.shared_from_this());
          }
        }
      );
    }
  );

  Policy::get().STDOUT_LISTENER_PORT(std::get<1>(L->get()->this_host()));

  return L;
}



};  // End of namespace dtc. ----------------------------------------------------------------------

