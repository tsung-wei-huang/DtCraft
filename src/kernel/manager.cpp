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

#include <dtc/kernel/manager.hpp>

namespace dtc {

// Function: insert_stderr_listener
std::shared_ptr<ReadEvent> KernelBase::insert_stderr_listener() {

  assert(is_owner());

  auto L = insert_listener(env::stderr_listener_port())(
    [this] (std::shared_ptr<Socket>&& skt) {
      insert<ReadEvent>(
        std::move(skt),
        [buffer=std::array<char, BUFSIZ>()] (Event& e) mutable {
          auto NR = ::read(e.device()->fd(), buffer.data(), BUFSIZ);
          if(NR <= 0 || ::write(STDERR_FILENO, buffer.data(), NR) != NR) {
            return Event::REMOVE;
          }
          return Event::DEFAULT;
        }
      ).get();
    }
  );

  env::stderr_listener_port(std::get<1>(std::static_pointer_cast<Socket>(L->device())->this_host()));

  return L;
}

// Function: insert_stdout_listener
std::shared_ptr<ReadEvent> KernelBase::insert_stdout_listener() {

  assert(is_owner());
  
  auto L = insert_listener(env::stdout_listener_port())(
    [this] (std::shared_ptr<Socket>&& skt) {
      insert<ReadEvent>(
        std::move(skt),
        [buffer=std::array<char, BUFSIZ>()] (Event& e) mutable {
          auto NR = ::read(e.device()->fd(), buffer.data(), BUFSIZ);
          if(NR <= 0 || ::write(STDOUT_FILENO, buffer.data(), NR) != NR) {
            return Event::REMOVE;
          }
          return Event::DEFAULT;
        }
      ).get();
    }
  );

  env::stdout_listener_port(std::get<1>(std::static_pointer_cast<Socket>(L->device())->this_host()));

  return L;
}



};  // End of namespace dtc. ----------------------------------------------------------------------

