/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
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

// Unittest: Statgrab.CPU
TEST_CASE("Statgrab.CPU") {
  
  REQUIRE(dtc::Statgrab::get().num_cpus() > 0);
  
  auto [L1, L5, L15] = dtc::Statgrab::get().cpu_load_avg();

  REQUIRE(L1 >= .0);
  REQUIRE(L5 >= .0);
  REQUIRE(L15 >= .0);
}

//-------------------------------------------------------------------------------------------------

// Unittest: Statgrab.Memory
TEST_CASE("Statgrab.Memory") {
  REQUIRE(dtc::Statgrab::get().memory_limit_in_bytes() > 0); 
}

//-------------------------------------------------------------------------------------------------

// Unittest: Statgrab.Space
TEST_CASE("Statgrab.Space") {
  REQUIRE(dtc::Statgrab::get().space_limit_in_bytes() > 0); 
}

