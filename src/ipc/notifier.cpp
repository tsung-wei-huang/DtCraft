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

#include <dtc/ipc/notifier.hpp>

namespace dtc {

std::shared_ptr<Notifier> make_notifier(unsigned int init) {
  
  int fd = ::eventfd(init, EFD_CLOEXEC | EFD_NONBLOCK);

  if(fd == -1) {
    throw std::system_error(
      std::make_error_code(static_cast<std::errc>(errno)), "Failed to create notifier"
    );
  }

  return std::make_shared<Notifier>(fd);
}

};  // end of namespace dtc. ----------------------------------------------------------------------
