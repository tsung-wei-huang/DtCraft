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

#include <dtc/ipc/ipc.hpp>

namespace dtc {

// ------------------------------------------------------------------------------------------------

// Procedure: _notify
bool OutputStream::_notify() {

  auto r = reactor();
  
  if(_notified || r == nullptr || osbuf._out_avail() == 0) return false;

  _notified = true;
  r->thaw(shared_from_this());

  return true;
}


};  // End of namespace dtc. ----------------------------------------------------------------------





