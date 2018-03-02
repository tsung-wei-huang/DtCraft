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
#include <dtc/traits.hpp>

// ---- Traits.STD --------------------------------------------------------------------------------

// Test case: Traits.STD.BasicString
TEST_CASE("TraitsTest.STD.BasicString") {
  static_assert(dtc::is_std_basic_string_v<std::string>);
  static_assert(dtc::is_std_basic_string_v<std::wstring>);
  static_assert(dtc::is_std_basic_string_v<std::u16string>);
  static_assert(dtc::is_std_basic_string_v<std::u32string>);
  
  static_assert(!dtc::is_std_basic_string_v<int>);
  static_assert(!dtc::is_std_basic_string_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_basic_string_v<std::vector<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::deque<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::list<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::map<int, int>>);
  static_assert(!dtc::is_std_basic_string_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_basic_string_v<std::set<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_basic_string_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_basic_string_v<std::error_code>);
}

// Test case: Traits.STD.Array
TEST_CASE("TraitsTest.STD.Array") {
  static_assert(dtc::is_std_array_v<std::array<int, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::array<int, 1024>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::vector<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::deque<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::list<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::forward_list<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::map<int, int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::unordered_map<int, int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::set<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::unordered_set<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::variant<int, float>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::unique_ptr<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::shared_ptr<int>, 1024>>);
  static_assert(dtc::is_std_array_v<std::array<std::error_code, 1024>>);
  
  static_assert(!dtc::is_std_array_v<int>);
  static_assert(!dtc::is_std_array_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_array_v<std::vector<int>>);
  static_assert(!dtc::is_std_array_v<std::deque<int>>);
  static_assert(!dtc::is_std_array_v<std::list<int>>);
  static_assert(!dtc::is_std_array_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_array_v<std::map<int, int>>);
  static_assert(!dtc::is_std_array_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_array_v<std::set<int>>);
  static_assert(!dtc::is_std_array_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_array_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_array_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_array_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_array_v<std::error_code>);
}

// Test case: Traits.STD.Vector
TEST_CASE("TraitsTest.STD.Vector") {
  static_assert(dtc::is_std_vector_v<std::vector<int>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::array<int, 1024>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::vector<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::deque<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::list<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::forward_list<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::map<int, int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::set<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::unordered_set<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::variant<int, float>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_vector_v<std::vector<std::error_code>>);
  
  static_assert(!dtc::is_std_vector_v<int>);
  static_assert(!dtc::is_std_vector_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_vector_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_vector_v<std::deque<int>>);
  static_assert(!dtc::is_std_vector_v<std::list<int>>);
  static_assert(!dtc::is_std_vector_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_vector_v<std::map<int, int>>);
  static_assert(!dtc::is_std_vector_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_vector_v<std::set<int>>);
  static_assert(!dtc::is_std_vector_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_vector_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_vector_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_vector_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_vector_v<std::error_code>);
}

// Test case: Traits.STD.Deque
TEST_CASE("TraitsTest.STD.Deque") {
  static_assert(dtc::is_std_deque_v<std::deque<int>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::array<int, 1024>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::vector<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::deque<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::list<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::forward_list<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::map<int, int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::set<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::unordered_set<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::variant<int, float>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_deque_v<std::deque<std::error_code>>);
  
  static_assert(!dtc::is_std_deque_v<int>);
  static_assert(!dtc::is_std_deque_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_deque_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_deque_v<std::vector<int>>);
  static_assert(!dtc::is_std_deque_v<std::list<int>>);
  static_assert(!dtc::is_std_deque_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_deque_v<std::map<int, int>>);
  static_assert(!dtc::is_std_deque_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_deque_v<std::set<int>>);
  static_assert(!dtc::is_std_deque_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_deque_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_deque_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_deque_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_deque_v<std::error_code>);
}

// Test case: Traits.STD.List
TEST_CASE("TraitsTest.STD.List") {
  static_assert(dtc::is_std_list_v<std::list<int>>);
  static_assert(dtc::is_std_list_v<std::list<std::array<int, 1024>>>);
  static_assert(dtc::is_std_list_v<std::list<std::vector<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::deque<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::list<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::forward_list<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::map<int, int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::set<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::unordered_set<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::variant<int, float>>>);
  static_assert(dtc::is_std_list_v<std::list<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_list_v<std::list<std::error_code>>);
  
  static_assert(!dtc::is_std_list_v<int>);
  static_assert(!dtc::is_std_list_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_list_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_list_v<std::vector<int>>);
  static_assert(!dtc::is_std_list_v<std::deque<int>>);
  static_assert(!dtc::is_std_list_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_list_v<std::map<int, int>>);
  static_assert(!dtc::is_std_list_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_list_v<std::set<int>>);
  static_assert(!dtc::is_std_list_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_list_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_list_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_list_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_list_v<std::error_code>);
}

// Test case: Traits.STD.ForwardList
TEST_CASE("TraitsTest.STD.ForwardList") {
  static_assert(dtc::is_std_forward_list_v<std::forward_list<int>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::array<int, 1024>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::vector<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::deque<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::list<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::forward_list<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::map<int, int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::set<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::unordered_set<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::variant<int, float>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_forward_list_v<std::forward_list<std::error_code>>);
  
  static_assert(!dtc::is_std_forward_list_v<int>);
  static_assert(!dtc::is_std_forward_list_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_forward_list_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_forward_list_v<std::vector<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::deque<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::list<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::map<int, int>>);
  static_assert(!dtc::is_std_forward_list_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_forward_list_v<std::set<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_forward_list_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_forward_list_v<std::error_code>);
}

// Test case: Traits.STD.Map
TEST_CASE("TraitsTest.STD.Map") {
  static_assert(dtc::is_std_map_v<std::map<int, int>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::array<int, 1024>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::vector<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::deque<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::list<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::forward_list<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::map<int, int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::set<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::unordered_set<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::variant<int, float>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::unique_ptr<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::shared_ptr<int>>>);
  static_assert(dtc::is_std_map_v<std::map<int, std::error_code>>);
  
  static_assert(!dtc::is_std_map_v<int>);
  static_assert(!dtc::is_std_map_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_map_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_map_v<std::vector<int>>);
  static_assert(!dtc::is_std_map_v<std::deque<int>>);
  static_assert(!dtc::is_std_map_v<std::list<int>>);
  static_assert(!dtc::is_std_map_v<std::set<int, int>>);
  static_assert(!dtc::is_std_map_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_map_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_map_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_map_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_map_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_map_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_map_v<std::error_code>);
}

// Test case: Traits.STD.UnorderedMap
TEST_CASE("TraitsTest.STD.UnorderedMap") {
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, int>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::array<int, 1024>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::vector<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::deque<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::list<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::forward_list<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::map<int, int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::set<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::unordered_set<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::variant<int, float>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::unique_ptr<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::shared_ptr<int>>>);
  static_assert(dtc::is_std_unordered_map_v<std::unordered_map<int, std::error_code>>);
  
  static_assert(!dtc::is_std_unordered_map_v<int>);
  static_assert(!dtc::is_std_unordered_map_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_unordered_map_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_unordered_map_v<std::vector<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::deque<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::list<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::set<int, int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::map<int, int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_unordered_map_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_unordered_map_v<std::error_code>);
}

// Test case: Traits.STD.Set
TEST_CASE("TraitsTest.STD.Set") {
  static_assert(dtc::is_std_set_v<std::set<int>>);
  static_assert(dtc::is_std_set_v<std::set<std::array<int, 1024>>>);
  static_assert(dtc::is_std_set_v<std::set<std::vector<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::deque<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::list<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::forward_list<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::map<int, int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::set<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::unordered_set<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::variant<int, float>>>);
  static_assert(dtc::is_std_set_v<std::set<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_set_v<std::set<std::error_code>>);
  
  static_assert(!dtc::is_std_set_v<int>);
  static_assert(!dtc::is_std_set_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_set_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_set_v<std::vector<int>>);
  static_assert(!dtc::is_std_set_v<std::deque<int>>);
  static_assert(!dtc::is_std_set_v<std::list<int>>);
  static_assert(!dtc::is_std_set_v<std::map<int, int>>);
  static_assert(!dtc::is_std_set_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_set_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_set_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_set_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_set_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_set_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_set_v<std::error_code>);
}

// Test case: Traits.STD.UnorderedSet
TEST_CASE("TraitsTest.STD.UnorderedSet") {
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<int>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::array<int, 1024>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::vector<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::deque<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::list<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::forward_list<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::map<int, int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::set<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::unordered_set<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::variant<int, float>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_unordered_set_v<std::unordered_set<std::error_code>>);
  
  static_assert(!dtc::is_std_unordered_set_v<int>);
  static_assert(!dtc::is_std_unordered_set_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_unordered_set_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_unordered_set_v<std::vector<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::deque<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::list<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::map<int, int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::set<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_unordered_set_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_unordered_set_v<std::error_code>);
}

// Test case: Traits.STD.UniquePointer
TEST_CASE("TraitsTest.STD.UniquePointer") {
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<int>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::array<int, 1024>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::vector<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::deque<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::list<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::forward_list<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::map<int, int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::set<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::unordered_set<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::variant<int, float>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_unique_ptr_v<std::unique_ptr<std::error_code>>);
  
  static_assert(!dtc::is_std_unique_ptr_v<int>);
  static_assert(!dtc::is_std_unique_ptr_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::vector<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::deque<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::list<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::map<int, int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::set<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_unique_ptr_v<std::error_code>);
}

// Test case: Traits.STD.SharedPointer
TEST_CASE("TraitsTest.STD.SharedPointer") {
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<int>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::array<int, 1024>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::vector<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::deque<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::list<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::forward_list<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::map<int, int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::unordered_map<int, int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::set<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::unordered_set<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::variant<int, float>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::unique_ptr<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::shared_ptr<int>>>);
  static_assert(dtc::is_std_shared_ptr_v<std::shared_ptr<std::error_code>>);
  
  static_assert(!dtc::is_std_shared_ptr_v<int>);
  static_assert(!dtc::is_std_shared_ptr_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::vector<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::deque<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::list<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::map<int, int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::set<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_shared_ptr_v<std::error_code>);
}

// Test case: Traits.STD.ErrorCode
TEST_CASE("TraitsTest.STD.ErrorCode") {
  static_assert(dtc::is_std_error_code_v<std::error_code>);
  
  static_assert(!dtc::is_std_error_code_v<int>);
  static_assert(!dtc::is_std_error_code_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_error_code_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_error_code_v<std::vector<int>>);
  static_assert(!dtc::is_std_error_code_v<std::deque<int>>);
  static_assert(!dtc::is_std_error_code_v<std::list<int>>);
  static_assert(!dtc::is_std_error_code_v<std::map<int, int>>);
  static_assert(!dtc::is_std_error_code_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_error_code_v<std::set<int>>);
  static_assert(!dtc::is_std_error_code_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_error_code_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_error_code_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_error_code_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_error_code_v<std::shared_ptr<int>>);
}

// Test case: Traits.STD.Duration
TEST_CASE("TraitsTest.STD.Duration") {
  static_assert(dtc::is_std_duration_v<std::chrono::seconds>);
  static_assert(dtc::is_std_duration_v<std::chrono::nanoseconds>);
  static_assert(dtc::is_std_duration_v<std::chrono::milliseconds>);
  static_assert(dtc::is_std_duration_v<std::chrono::minutes>);
  static_assert(dtc::is_std_duration_v<std::chrono::hours>);
  
  static_assert(!dtc::is_std_duration_v<int>);
  static_assert(!dtc::is_std_duration_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_duration_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_duration_v<std::vector<int>>);
  static_assert(!dtc::is_std_duration_v<std::deque<int>>);
  static_assert(!dtc::is_std_duration_v<std::list<int>>);
  static_assert(!dtc::is_std_duration_v<std::map<int, int>>);
  static_assert(!dtc::is_std_duration_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_duration_v<std::set<int>>);
  static_assert(!dtc::is_std_duration_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_duration_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_duration_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_duration_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_duration_v<std::shared_ptr<int>>);
}

// Test case: Traits.STD.Optional
TEST_CASE("TraitsTest.STD.Optional") {
  static_assert(dtc::is_std_optional_v<std::optional<int>>);
  static_assert(dtc::is_std_optional_v<std::optional<std::optional<int>>>);
  
  static_assert(!dtc::is_std_optional_v<int>);
  static_assert(!dtc::is_std_optional_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_optional_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_optional_v<std::vector<int>>);
  static_assert(!dtc::is_std_optional_v<std::deque<int>>);
  static_assert(!dtc::is_std_optional_v<std::list<int>>);
  static_assert(!dtc::is_std_optional_v<std::map<int, int>>);
  static_assert(!dtc::is_std_optional_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_optional_v<std::set<int>>);
  static_assert(!dtc::is_std_optional_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_optional_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_optional_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_optional_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_optional_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_optional_v<std::tuple<std::optional<int>, int>>);
}

// Test case: Traits.STD.tuple
TEST_CASE("TraitsTest.STD.tuple") {
  static_assert(dtc::is_std_tuple_v<std::tuple<>>);
  static_assert(dtc::is_std_tuple_v<std::tuple<int>>);
  static_assert(dtc::is_std_tuple_v<std::tuple<std::tuple<int>>>);
  
  static_assert(!dtc::is_std_tuple_v<int>);
  static_assert(!dtc::is_std_tuple_v<std::basic_string<char>>);
  static_assert(!dtc::is_std_tuple_v<std::array<int, 1024>>);
  static_assert(!dtc::is_std_tuple_v<std::vector<int>>);
  static_assert(!dtc::is_std_tuple_v<std::deque<int>>);
  static_assert(!dtc::is_std_tuple_v<std::list<int>>);
  static_assert(!dtc::is_std_tuple_v<std::map<int, int>>);
  static_assert(!dtc::is_std_tuple_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_std_tuple_v<std::set<int>>);
  static_assert(!dtc::is_std_tuple_v<std::unordered_set<int>>);
  static_assert(!dtc::is_std_tuple_v<std::variant<int, float>>);
  static_assert(!dtc::is_std_tuple_v<std::forward_list<int>>);
  static_assert(!dtc::is_std_tuple_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_std_tuple_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_std_tuple_v<std::optional<std::tuple<int, int>>>);
}

// ---- Eigen traits ------------------------------------------------------------------------------

// Test case:: TraitsTest.Eigen.Matrix
TEST_CASE("TraitsTest.Eigen.Matrix") {
  
  static_assert(dtc::is_eigen_matrix_v<Eigen::MatrixXi>);
  static_assert(dtc::is_eigen_matrix_v<Eigen::MatrixXd>);
  static_assert(dtc::is_eigen_matrix_v<Eigen::MatrixXf>);
  static_assert(dtc::is_eigen_matrix_v<Eigen::VectorXi>);
  static_assert(dtc::is_eigen_matrix_v<Eigen::VectorXf>);
  static_assert(dtc::is_eigen_matrix_v<Eigen::VectorXd>);

  static_assert(!dtc::is_eigen_matrix_v<int>);
  static_assert(!dtc::is_eigen_matrix_v<std::basic_string<char>>);
  static_assert(!dtc::is_eigen_matrix_v<std::array<int, 1024>>);
  static_assert(!dtc::is_eigen_matrix_v<std::vector<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::deque<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::list<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::map<int, int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::unordered_map<int, int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::set<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::unordered_set<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::variant<int, float>>);
  static_assert(!dtc::is_eigen_matrix_v<std::forward_list<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::unique_ptr<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::shared_ptr<int>>);
  static_assert(!dtc::is_eigen_matrix_v<std::optional<int>>);
}

// ---- Closure traits ----------------------------------------------------------------------------

// Test case: TraitsTest.Lambda
TEST_CASE("TraitsTest.Lambda") {

  auto l1 = [] (int, double&, const std::string&) { };

  static_assert(dtc::closure_traits<decltype(l1)>::arity::value == 3);
  static_assert(dtc::closure_traits<decltype(l1)>::is_const::value);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l1)>::result_type, void>);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<0>, int>);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<1>, double&>);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<2>, const std::string&>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l1)>::result_type, int>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<0>, int&>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<1>, double>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l1)>::arg<2>, std::string&>);

  auto l2 = [] (std::vector<std::string>, std::any&) mutable { 
    std::unordered_map<int, int> map = {{1, 2}, {2, 3}, {3, 4}, {4,5}};
    return map;
  };
  
  static_assert(dtc::closure_traits<decltype(l2)>::arity::value == 2);
  static_assert(!dtc::closure_traits<decltype(l2)>::is_const::value);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l2)>::result_type, std::unordered_map<int, int>>);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l2)>::arg<0>, std::vector<std::string>>);
  static_assert(std::is_same_v<dtc::closure_traits<decltype(l2)>::arg<1>, std::any&>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l2)>::result_type, int>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l2)>::arg<0>, std::vector<std::string>&>);
  static_assert(!std::is_same_v<dtc::closure_traits<decltype(l2)>::arg<1>, double>);

}

// Test case: TraitsTest.Function
TEST_CASE("TraitsTest.Function") {

  using F1 = std::function<void(int, double&, const std::string&)>;

  static_assert(dtc::closure_traits<F1>::arity::value == 3);
  static_assert(dtc::closure_traits<F1>::is_const::value);
  static_assert(std::is_same_v<dtc::closure_traits<F1>::result_type, void>);
  static_assert(std::is_same_v<dtc::closure_traits<F1>::arg<0>, int>);
  static_assert(std::is_same_v<dtc::closure_traits<F1>::arg<1>, double&>);
  static_assert(std::is_same_v<dtc::closure_traits<F1>::arg<2>, const std::string&>);
  static_assert(!std::is_same_v<dtc::closure_traits<F1>::result_type, int>);
  static_assert(!std::is_same_v<dtc::closure_traits<F1>::arg<0>, int&>);
  static_assert(!std::is_same_v<dtc::closure_traits<F1>::arg<1>, double>);
  static_assert(!std::is_same_v<dtc::closure_traits<F1>::arg<2>, std::string&>);

  using F2 = std::function<std::unordered_map<int, int>(std::vector<std::string>, std::any&)>;

  static_assert(dtc::closure_traits<F2>::arity::value == 2);
  static_assert(dtc::closure_traits<F2>::is_const::value);
  static_assert(std::is_same_v<dtc::closure_traits<F2>::result_type, std::unordered_map<int, int>>);
  static_assert(std::is_same_v<dtc::closure_traits<F2>::arg<0>, std::vector<std::string>>);
  static_assert(std::is_same_v<dtc::closure_traits<F2>::arg<1>, std::any&>);
  static_assert(!std::is_same_v<dtc::closure_traits<F2>::result_type, int>);
  static_assert(!std::is_same_v<dtc::closure_traits<F2>::arg<0>, std::vector<std::string>&>);
  static_assert(!std::is_same_v<dtc::closure_traits<F2>::arg<1>, double>);

}















