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

#include <dtc/protobuf/resource.hpp>

namespace dtc::pb {

// Procedure: update
Resource& Resource::update() {
  num_cpus = Statgrab::get().num_cpus();
  memory_limit_in_bytes = Statgrab::get().memory_limit_in_bytes();
  space_limit_in_bytes = Statgrab::get().space_limit_in_bytes();
  return *this;
}

// Operator: >
bool Resource::operator > (uintmax_t rhs) const {
  return num_cpus > rhs &&
         memory_limit_in_bytes > rhs &&
         space_limit_in_bytes > rhs; 
}

// Operator: >=
bool Resource::operator >= (uintmax_t rhs) const {
  return num_cpus >= rhs &&
         memory_limit_in_bytes >= rhs &&
         space_limit_in_bytes >= rhs; 
}

// Operator: <
bool Resource::operator < (uintmax_t rhs) const {
  return num_cpus < rhs &&
         memory_limit_in_bytes < rhs &&
         space_limit_in_bytes < rhs;
}

// Operator: <=
bool Resource::operator <= (uintmax_t rhs) const {
  return num_cpus <= rhs &&
         memory_limit_in_bytes <= rhs &&
         space_limit_in_bytes <= rhs;
}

// Operator: <
bool Resource::operator < (const Resource& rhs) const {
  return num_cpus < rhs.num_cpus &&
         memory_limit_in_bytes < rhs.memory_limit_in_bytes &&
         space_limit_in_bytes < rhs.space_limit_in_bytes;
}

// Operator: <=
bool Resource::operator <= (const Resource& rhs) const {
  return num_cpus <= rhs.num_cpus &&
         memory_limit_in_bytes <= rhs.memory_limit_in_bytes &&
         space_limit_in_bytes <= rhs.space_limit_in_bytes;
}

// Operator: >
bool Resource::operator > (const Resource& rhs) const {
  return num_cpus > rhs.num_cpus &&
         memory_limit_in_bytes > rhs.memory_limit_in_bytes && 
         space_limit_in_bytes > rhs.space_limit_in_bytes; 
}

// Operator: >=
bool Resource::operator >= (const Resource& rhs) const {
  return num_cpus >= rhs.num_cpus &&
         memory_limit_in_bytes >= rhs.memory_limit_in_bytes &&
         space_limit_in_bytes >= rhs.space_limit_in_bytes;
}

// Operator: ==
bool Resource::operator == (const Resource& rhs) const {
  return num_cpus == rhs.num_cpus &&
         memory_limit_in_bytes == rhs.memory_limit_in_bytes &&
         space_limit_in_bytes == rhs.space_limit_in_bytes;
}

// Operator: !=
bool Resource::operator != (const Resource &rhs) const {
  return !(*this == rhs);
}

// Operator: +=
Resource& Resource::operator += (const Resource& rhs) {
  num_cpus += rhs.num_cpus;
  memory_limit_in_bytes += rhs.memory_limit_in_bytes;
  space_limit_in_bytes += rhs.space_limit_in_bytes;
  return *this;
}

// Operator: -=
Resource& Resource::operator -= (const Resource& rhs) {

  if(!(*this >= rhs)) {
    DTC_THROW("Resource substraction encountered underflow");
  }
  
  num_cpus -= rhs.num_cpus;
  memory_limit_in_bytes -= rhs.memory_limit_in_bytes;
  space_limit_in_bytes -= rhs.space_limit_in_bytes;

  return *this;
}

// Function: to_string
std::string Resource::to_string() const {
  std::ostringstream oss;
  oss << "@" << host << " "
      << "[cpu:" << num_cpus
      << "|mem:" << memory_limit_in_bytes 
      << "|disk:" << space_limit_in_bytes << "]";
  return oss.str();
}

// Operator: <<
std::ostream& operator<<(std::ostream& os , const Resource& rhs) {
  os << rhs.to_string();
  return os;
}

};  // End of namespace dtc::pb::resource. --------------------------------------------------


