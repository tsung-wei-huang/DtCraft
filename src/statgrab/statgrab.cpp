/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin and Martin D. F. Wong,   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/statgrab/statgrab.hpp>

namespace dtc {

// Function: num_cpus
uintmax_t Statgrab::num_cpus() const {
  if(auto N = ::sysconf(_SC_NPROCESSORS_ONLN); N == -1) {
    return 0;
  }
  else {
    return N;
  }
}

// Function: memory_limit_in_bytes
uintmax_t Statgrab::memory_limit_in_bytes() const {
  if(struct sysinfo info; ::sysinfo(&info) == -1) {
    return 0;
  }
  else {
    return ((uintmax_t)info.totalram * info.mem_unit);
  }
}

// Function: space_limit_in_bytes
uintmax_t Statgrab::space_limit_in_bytes() const {
  return std::filesystem::space("/").available;
}

// Function: cpu_load_avg
Statgrab::LoadInfo Statgrab::cpu_load_avg() const {
  LoadInfo val;
  if(::getloadavg(reinterpret_cast<double*>(&val), 3) == -1) {
    return {DBL_MAX, DBL_MAX, DBL_MAX};
  }
  return val;
}


};  // End of namespace. --------------------------------------------------------------------------




