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

#include <dtc/ipc/pipe.hpp>

namespace dtc {

// Function: make_pipe
std::tuple<std::shared_ptr<Pipe>, std::shared_ptr<Pipe>> make_pipe() {
  if(int fd[2]; ::pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "Failed to create pipe"
    );
	}
	else {
    return {std::make_shared<dtc::Pipe>(fd[0]), std::make_shared<dtc::Pipe>(fd[1])};
  }
}

// Function: make_sync_pipe
std::tuple<std::shared_ptr<Pipe>, std::shared_ptr<Pipe>> make_sync_pipe() {
  if(int fd[2]; ::pipe2(fd, O_CLOEXEC | O_DIRECT) == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "Failed to create sync pipe"
    );
	}
	else {
    return {std::make_shared<dtc::Pipe>(fd[0]), std::make_shared<dtc::Pipe>(fd[1])};
  }
}

};  // end of namespace dtc. ----------------------------------------------------------------------
