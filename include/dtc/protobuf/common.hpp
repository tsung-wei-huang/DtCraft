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

#ifndef DTC_PROTOBUF_COMMON_HPP_
#define DTC_PROTOBUF_COMMON_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility/utility.hpp>

namespace dtc {

// Struct: TaskID
struct TaskID {

  key_type graph {-1};
  key_type topology {-1};
  
  TaskID() = default;
  TaskID(const TaskID&) = default;
  TaskID(TaskID&&) = default;

  TaskID& operator = (const TaskID&) = default;
  TaskID& operator = (TaskID&&) = default;

  std::string to_string() const;

  inline bool operator == (const TaskID&) const;
  inline bool operator <  (const TaskID&) const;
  inline bool operator >  (const TaskID&) const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT& ar) {
    return ar(graph, topology);
  }
};

// Operator: ==  
inline bool TaskID::operator == (const TaskID& rhs) const {
  return graph == rhs.graph && topology == rhs.topology;
}

// Operator: <
inline bool TaskID::operator < (const TaskID& rhs) const {
  return graph < rhs.graph && topology < rhs.topology;
}

// Operator: >
inline bool TaskID::operator > (const TaskID& rhs) const {
  return graph > rhs.graph && topology > rhs.topology;
}

// Outputstream
std::ostream& operator<<(std::ostream&, const TaskID&);

};  // End of namespace dtc. ----------------------------------------------------------------------

namespace std {
  
template <>
struct hash<dtc::TaskID> {
  size_t operator()(const dtc::TaskID& t) const {
    size_t seed {0};
    dtc::hash_combine(seed, t.graph);
    dtc::hash_combine(seed, t.topology);
    return seed;
  }
};

}  // End of namespace std. -----------------------------------------------------------------------

#endif
