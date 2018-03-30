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

#ifndef DTC_PROTOBUF_LOADINFO_HPP_
#define DTC_PROTOBUF_LOADINFO_HPP_

#include <headerdef.hpp>
#include <dtc/statgrab/statgrab.hpp>

namespace dtc::pb {

// Struct: LoadInfo
struct LoadInfo {

  float cpu_load {.0f};

  LoadInfo() = default;
  LoadInfo(LoadInfo&&) = default;
  LoadInfo(const LoadInfo&) = default;

  LoadInfo& operator = (const LoadInfo&) = default;
  LoadInfo& operator = (LoadInfo&&) = default;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(cpu_load);
  }

};




};  // End of namespace dtc::pb. ------------------------------------------------------------------

#endif
