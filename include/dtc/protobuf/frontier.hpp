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

#ifndef DTC_PROTOBUF_FRONTIER_HPP_
#define DTC_PROTOBUF_FRONTIER_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/protobuf/topology.hpp>
#include <dtc/ipc/socket.hpp>

namespace dtc::pb {

// Struct: Frontier
struct Frontier {

  key_type graph {-1};   
  key_type topology {-1};
  key_type stream {-1}; 

  std::shared_ptr<Socket> socket;

  Frontier() = default;
  Frontier(const Frontier&) = default;
  Frontier(Frontier&&) = default;
  ~Frontier() = default;

  Frontier& operator = (const Frontier&) = default;
  Frontier& operator = (Frontier&&) = default;

  std::string to_kvp() const;

  inline auto task_id() const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(graph, topology, stream);
  }

};

// Function: task_id
inline auto Frontier::task_id() const {
  return TaskID{graph, topology};
}


};  // End of namespace dtc::pb. ------------------------------------------------------------


#endif



