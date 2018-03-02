/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_HEADERDEF_HPP_
#define DTC_HEADERDEF_HPP_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <dirent.h>
#include <vector>
#include <cstring>
#include <string_view>
#include <memory>
#include <map>
#include <future>
#include <atomic>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <tuple>
#include <unordered_set>
#include <numeric>
#include <iterator>
#include <functional>
#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <sys/eventfd.h>
#include <sys/mount.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/sysctl.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <float.h>
#include <cassert>
#include <getopt.h>
#include <random>
#include <signal.h>
#include <mntent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex>
#include <variant>
#include <experimental/filesystem>
#include <optional>
#include <any>


// Platform-specific include.
#ifdef __APPLE__
#include <crt_externs.h>      
#elif !defined(__WINDOWS__)
extern char** environ;
#endif

// 3rd-party include
#include <dtc/Eigen/Core>
#include <dtc/json/json.hpp>

// Top header declaration.
#include <dtc/macrodef.hpp>
#include <dtc/traits.hpp>
#include <dtc/config.hpp>
#include <dtc/error.hpp>
#include <dtc/exit.hpp>

namespace dtc {

using key_type = int;
  
using namespace std::chrono_literals;
using namespace std::literals::string_literals;

using json = nlohmann::json;  


};  // End of namespace dtc. ----------------------------------------------------------------------


namespace std {
  namespace filesystem = experimental::filesystem;
};



#endif

