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

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

// ---- Utility.uuid ------------------------------------------------------------------------------

// Test case: Utility.uuid
TEST_CASE("Utility.uuid") {

  dtc::UUID u1, u2, u3, u4; 

  // Comparator.
  REQUIRE(u1 == u1);

  // Copy
  u2 = u1; 
  REQUIRE(u1 == u2);

  // Move
  u3 = std::move(u1); 
  REQUIRE(u2 == u3);

  // Copy constructor
  dtc::UUID u5(u4);
  REQUIRE(u5 == u4);

  // Move constructor.
  dtc::UUID u6(std::move(u4));
  REQUIRE(u5 == u6);

	// Uniqueness
  std::vector<dtc::UUID> uuids(65536);
  std::sort(uuids.begin(), uuids.end());
  std::unique(uuids.begin(), uuids.end());
  REQUIRE(uuids.size() == 65536);
}

//-------------------------------------------------------------------------------------------------

// Operator == 
bool operator == (const struct timeval& lhs, const struct timeval& rhs) {
  return lhs.tv_sec == rhs.tv_sec && lhs.tv_usec == rhs.tv_usec;
}

// Unittest: Utility.os.chrono
TEST_CASE("Utility.os.chrono") {
  // Conversion between chrono and timeval
  for(auto s=1; s<=1024; s<<=1) {
    for(auto u=1; u<=1024; u<<=1) {
      const struct timeval tv {s, u};
      const auto N = std::chrono::nanoseconds((uint64_t)s*1000000000 + u*1000);
      const auto B = std::chrono::duration_cast<std::chrono::microseconds>(N);
      const auto M = std::chrono::duration_cast<std::chrono::milliseconds>(N);
      const auto S = std::chrono::duration_cast<std::chrono::seconds>(N);
      REQUIRE(dtc::duration_cast<std::chrono::nanoseconds>(tv) == N);
      REQUIRE(dtc::duration_cast<std::chrono::microseconds>(tv) == B);
      REQUIRE(dtc::duration_cast<std::chrono::milliseconds>(tv) == M);
      REQUIRE(dtc::duration_cast<std::chrono::seconds>(tv) == S);
      REQUIRE(dtc::duration_cast<struct timeval>(N) == tv);
    }
  }

}

//-------------------------------------------------------------------------------------------------

// Unittest: Literals
TEST_CASE("Utility.literals") {
  
  using namespace dtc::literals;
  
  // Byte as baseline.
  static_assert(1_KB == 1000);
  static_assert(1_MB == 1000000);
  static_assert(1_GB == 1000000000);

  // KB as baseline
  static_assert(1000 == 1_KB);
  static_assert(1_MB == 1000_KB);
  static_assert(1_GB == 1000000_KB);

  // MB as baseline
  static_assert(1000000 == 1_MB);
  static_assert(1_GB == 1000_MB);

}

//-------------------------------------------------------------------------------------------------

// Unittest: Utility.os.environment
TEST_CASE("Utility.os.environment") {

  const auto key = "dtc";
  const auto val = "dummy";

  auto env = dtc::environment_variables();
  REQUIRE(env.size() > 0);

  REQUIRE(::unsetenv(key) != -1);
  env = dtc::environment_variables();
  REQUIRE(env.find(key) == env.end());

  REQUIRE(::setenv(key, val, 1) != -1) ;
  env = dtc::environment_variables();
  REQUIRE((env.find(key) != env.end() && env.find(key)->second == val));
}

//-------------------------------------------------------------------------------------------------

// Unit test: Utility.os.descriptor
TEST_CASE("Utility.os.descriptor") {

  int fd {-1};

  // Invalid fd
  REQUIRE_FALSE(dtc::is_fd_valid(fd));
  REQUIRE_FALSE(dtc::is_fd_blocking(fd));
  REQUIRE_FALSE(dtc::is_fd_nonblocking(fd));
  REQUIRE_FALSE(dtc::is_fd_open_on_exec(fd));
  REQUIRE_FALSE(dtc::is_fd_close_on_exec(fd));

  // Check the file descriptor flag
  // Basic operations
  REQUIRE_THROWS_AS(dtc::make_fd_blocking(fd), std::system_error);
  REQUIRE_THROWS_AS(dtc::make_fd_nonblocking(fd), std::system_error);
  REQUIRE_THROWS_AS(dtc::make_fd_open_on_exec(fd), std::system_error);
  REQUIRE_THROWS_AS(dtc::make_fd_close_on_exec(fd), std::system_error);

  // Make a valid fd
  fd = STDIN_FILENO;
  REQUIRE(dtc::is_fd_valid(fd));

  // Close or open on exec operations.
  REQUIRE(dtc::is_fd_open_on_exec(fd));
  REQUIRE_FALSE(dtc::is_fd_close_on_exec(fd));
  REQUIRE_NOTHROW(dtc::make_fd_close_on_exec(fd));
  REQUIRE(dtc::is_fd_close_on_exec(fd));
  REQUIRE_FALSE(dtc::is_fd_open_on_exec(fd));
  REQUIRE_NOTHROW(dtc::make_fd_open_on_exec(fd));
  REQUIRE(dtc::is_fd_open_on_exec(fd));
  REQUIRE_FALSE(dtc::is_fd_close_on_exec(fd));
  REQUIRE_NOTHROW(dtc::make_fd_close_on_exec(fd));
  REQUIRE_FALSE(dtc::is_fd_open_on_exec(fd));
  REQUIRE(dtc::is_fd_close_on_exec(fd));
  
  // Non-blocking and blocking operations.
  REQUIRE_NOTHROW(dtc::make_fd_blocking(fd));
  REQUIRE(dtc::is_fd_blocking(fd));
  REQUIRE_FALSE(dtc::is_fd_nonblocking(fd));
  REQUIRE_NOTHROW(dtc::make_fd_nonblocking(fd));
  REQUIRE(dtc::is_fd_nonblocking(fd));
  REQUIRE_FALSE(dtc::is_fd_blocking(fd));
  REQUIRE_NOTHROW(dtc::make_fd_blocking(fd));
  REQUIRE(dtc::is_fd_blocking(fd));
  REQUIRE_FALSE(dtc::is_fd_nonblocking(fd));
  REQUIRE_NOTHROW(dtc::make_fd_nonblocking(fd));
  REQUIRE_FALSE(dtc::is_fd_blocking(fd));
  REQUIRE(dtc::is_fd_nonblocking(fd));
}



