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

#ifndef DTC_PROTOBUF_RESOURCE_HPP_
#define DTC_PROTOBUF_RESOURCE_HPP_

#include <dtc/policy.hpp>
#include <dtc/statgrab/statgrab.hpp>

namespace dtc::pb {

// Struct: Resource
//
// - Memory:
//
// total = all memory in the system (4GB on this server)
// used = all memory currently in use/reserved by running processes and the OS
// free = total - used
// shared = memory being shared by multiple processes 
// buffers = memory reserved by the OS to alloc as buffers when process need them ('heap')
// cached = recently used files being stored in ram (THANK YOU LINUX!)
// 
// Using these definitions:
// 
// When thinking about 'how much memory is really being used' - I want to calculate:
// 'used' - ('buffers' + 'cached')
// 
// When thinking about 'how much memory is really free' - I want to calculate:
// 'free' + ('buffers' + 'cached')
//
struct Resource {
  
  std::string host {env::this_host()};

  uintmax_t num_cpus {0};
  uintmax_t memory_limit_in_bytes {0};
  uintmax_t space_limit_in_bytes {0};

  Resource() = default;
  Resource(const Resource&) = default;
  Resource(Resource&&) = default;
  ~Resource() = default;

  Resource& operator = (const Resource&) = default;
  Resource& operator = (Resource&&) = default;

  Resource& update();

  std::string to_string() const;

  bool operator >  (uintmax_t) const;
  bool operator >= (uintmax_t) const;
  bool operator <  (uintmax_t) const;
  bool operator <= (uintmax_t) const;
  bool operator >  (const Resource &) const;
  bool operator >= (const Resource &) const;
  bool operator <  (const Resource &) const;
  bool operator <= (const Resource &) const;
  bool operator == (const Resource &) const;
  bool operator != (const Resource &) const;

  Resource& operator += (const Resource&);
  Resource& operator -= (const Resource&);

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT&);
};

// Function: archive
template <typename ArchiverT>
std::streamsize Resource::archive(ArchiverT& ar) {
  return ar(
    host,
    num_cpus,
    memory_limit_in_bytes,
    space_limit_in_bytes
  );
}

// Operator: <<
std::ostream& operator << (std::ostream&, const Resource&);

};  // End of namespace dtc::pb. ------------------------------------------------------------


#endif

