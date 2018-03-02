/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin and Martin D. F. Wong,   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_PROTOBUF_BROKENIO_HPP_
#define DTC_PROTOBUF_BROKENIO_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::pb {

// BrokenIO.
struct BrokenIO {

  std::ios_base::openmode mode;
  std::error_code errc;
  
  BrokenIO() = default;
  BrokenIO(const BrokenIO&) = default;
  BrokenIO(BrokenIO&&) = default;
  ~BrokenIO() = default;

  BrokenIO& operator = (const BrokenIO&) = default;
  BrokenIO& operator = (BrokenIO&&) = default;
  
  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(mode, errc);
  }
};

};  // End of namespace dtc::pb. ------------------------------------------------------------

#endif




