/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_LEXICAL_CAST_HPP_
#define DTC_UTILITY_LEXICAL_CAST_HPP_

namespace dtc::lexical_cast_impl {

template <typename T>
struct Converter;

// Numeric to string
template <>
struct Converter<std::string> {
  static auto convert(auto&& from) {
    return std::to_string(from);
  }
};

template <>
struct Converter<int> {
  static auto convert(std::string_view from) {
    return std::atoi(from.data());
  }
};

template <>
struct Converter<long> {
  static auto convert(std::string_view from) {
    return std::atol(from.data());
  }
};

template <>
struct Converter<long long> {
  static auto convert(std::string_view from) {
    return std::atoll(from.data());
  }
};

template <>
struct Converter<unsigned long> {
  static auto convert(std::string_view from) {
    return std::strtoul(from.data(), nullptr, 10);
  }
};

template <>
struct Converter<unsigned long long> {
  static auto convert(std::string_view from) {
    return std::strtoull(from.data(), nullptr, 10);
  }
};

template <>
struct Converter<float> {
  static auto convert(std::string_view from) {
    return std::strtof(from.data(), nullptr);
  }
};

template <>
struct Converter<double> {
  static auto convert(std::string_view from) {
    return std::strtod(from.data(), nullptr);
  }
};

template <>
struct Converter<long double> {
  static auto convert(std::string_view from) {
    return std::strtold(from.data(), nullptr);
  }
};

};  // end of namespace dtc::lexical_cast_details. ------------------------------------------------

namespace dtc {

template <typename To, typename From>
auto lexical_cast(From&& from) {
  
  using T = std::decay_t<To>;
  using F = std::decay_t<From>;

  if constexpr (std::is_same_v<T, F>) {
    return from;
  } 
  else {
    return lexical_cast_impl::Converter<T>::convert(from);
  }
}

};  // End of namespace dtc. ---------------------------------------------------------------------


#endif
