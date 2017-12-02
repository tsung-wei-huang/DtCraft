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

// This file is created and modified based on the cereal serialization/deserialization
// library invented by Randolph Voorhies and Shane Grant (Copyright (c) 2014).

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

// Struct: PODs
struct PODs {

  uint8_t       _bool   = dtc::random<decltype(_bool)>(); 
  char          _char   = dtc::random<decltype(_char)>();  
  unsigned char _uchar  = dtc::random<decltype(_uchar)>(); 
  uint8_t       _uint8  = dtc::random<decltype(_uint8)>();
  int8_t        _int8   = dtc::random<decltype(_int8)>();  
  uint16_t      _uint16 = dtc::random<decltype(_uint16)>();
  int16_t       _int16  = dtc::random<decltype(_int16)>(); 
  uint32_t      _uint32 = dtc::random<decltype(_uint32)>();
  int32_t       _int32  = dtc::random<decltype(_int32)>(); 
  uint64_t      _uint64 = dtc::random<decltype(_uint64)>();
  int64_t       _int64  = dtc::random<decltype(_int64)>(); 
  float_t       _float  = dtc::random<decltype(_float)>(); 
  double_t      _double = dtc::random<decltype(_double)>();

  template <typename ArchiverT>
  std::streamsize archive( ArchiverT& ar ) {
    return ar(
      _bool,  
      _char,  
      _uchar, 
      _uint8,
      _int8,  
      _uint16,
      _int16, 
      _uint32,
      _int32, 
      _uint64,
      _int64, 
      _float, 
      _double
    );
  }

  bool operator == (const PODs& rhs) const {
    return _bool   == rhs._bool   && 
           _char   == rhs._char   && 
           _uchar  == rhs._uchar  &&  
           _uint8  == rhs._uint8  &&  
           _int8   == rhs._int8   && 
           _uint16 == rhs._uint16 &&  
           _int16  == rhs._int16  && 
           _uint32 == rhs._uint32 && 
           _int32  == rhs._int32  && 
           _uint64 == rhs._uint64 && 
           _int64  == rhs._int64  && 
           _float  == rhs._float  && 
           _double == rhs._double; 
  }

  bool operator != (const PODs& rhs) const {
    return !(*this == rhs);
  }
}; 

// Procedure: test_pod
// The templated procedure for testing POD. Caller must specify the output 
// and input archiver type.
template <typename SerializerT, typename DeserializerT>
void test_pod() {

  // Output stream.
  dtc::OutputStreamBuffer os;
  SerializerT oar(os);

  const auto o_bool   = dtc::random<int8_t>();
  const auto o_char   = dtc::random<char>(); 
  const auto o_uchar  = dtc::random<unsigned char>();
  const auto o_uint8  = dtc::random<uint8_t>();      
  const auto o_int8   = dtc::random<int8_t>();      
  const auto o_uint16 = dtc::random<uint16_t>();     
  const auto o_int16  = dtc::random<int16_t>();       
  const auto o_uint32 = dtc::random<uint32_t>();     
  const auto o_int32  = dtc::random<int32_t>();      
  const auto o_uint64 = dtc::random<uint64_t>();     
  const auto o_int64  = dtc::random<int64_t>();      
  const auto o_float  = dtc::random<float>();        
  const auto o_double = dtc::random<double>();       
  
  auto o_sz = oar(
    o_bool,  
    o_char,  
    o_uchar, 
    o_uint8,
    o_int8,  
    o_uint16,
    o_int16, 
    o_uint32,
    o_int32, 
    o_uint64,
    o_int64, 
    o_float, 
    o_double
  );

  REQUIRE(o_sz == os.out_avail());

  // InputStreamBuffer
  dtc::InputStreamBuffer is(os); 
  DeserializerT iar(is);

  auto i_bool   = dtc::random<int8_t>();
  auto i_char   = dtc::random<char>(); 
  auto i_uchar  = dtc::random<unsigned char>();
  auto i_uint8  = dtc::random<uint8_t>();      
  auto i_int8   = dtc::random<int8_t>();      
  auto i_uint16 = dtc::random<uint16_t>();     
  auto i_int16  = dtc::random<int16_t>();       
  auto i_uint32 = dtc::random<uint32_t>();     
  auto i_int32  = dtc::random<int32_t>();      
  auto i_uint64 = dtc::random<uint64_t>();     
  auto i_int64  = dtc::random<int64_t>();      
  auto i_float  = dtc::random<float>();        
  auto i_double = dtc::random<double>();       

  auto i_sz = iar(
    i_bool,  
    i_char,  
    i_uchar, 
    i_uint8,
    i_int8,  
    i_uint16,
    i_int16, 
    i_uint32,
    i_int32, 
    i_uint64,
    i_int64, 
    i_float, 
    i_double
  );
  REQUIRE(is.in_avail() == 0);

  REQUIRE(i_sz == o_sz);
  REQUIRE(o_bool == i_bool);
  REQUIRE(o_char == i_char);
  REQUIRE(o_uchar == i_uchar);
  REQUIRE(o_uint8 == i_uint8);
  REQUIRE(o_int8 == i_int8);
  REQUIRE(o_uint16 == i_uint16);
  REQUIRE(o_int16 == i_int16);
  REQUIRE(o_uint32 == i_uint32);
  REQUIRE(o_int32 == i_int32);
  REQUIRE(o_uint64 == i_uint64);
  REQUIRE(o_int64 == i_int64);
  REQUIRE(o_float == i_float);
  REQUIRE(o_double == i_double);
}

// Procedure: test_struct
// The templated procedure for testing POD. Caller must specify the output 
// and input archiver type.
template <typename SerializerT, typename DeserializerT>
void test_struct() {

  for(size_t i=0; i<1024; ++i) {
    
    // POD struct.
    PODs o_pods;
    PODs i_pods;

    // Outputstream
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto o_sz = oar(o_pods);
    REQUIRE(o_sz == os.out_avail());

    // Inputstream
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    auto i_sz = iar(i_pods);
    REQUIRE(is.in_avail() == 0);
    
    REQUIRE(o_sz == i_sz);
    REQUIRE(o_pods == i_pods);
  }
}

// Procedure: test_string
// Template for testing basic strings. Caller must specify the output and input archiver type.
template <typename SerializerT, typename DeserializerT>
void test_string() {

  for(size_t i=0; i<1024; i++) {

    // Outputstream
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);

    std::string o_char_str = dtc::random<std::string>();
    auto o_sz = oar(o_char_str);
    REQUIRE(o_sz == os.out_avail());

    // Inputstream
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    
    std::string i_char_str;
    auto i_sz = iar(i_char_str);
    REQUIRE(is.in_avail() == 0);

    REQUIRE(o_sz == i_sz);
    REQUIRE(o_char_str == i_char_str);
  }
}

#define TEST_SEQ_CONT_BODY(container)                            \
                                                                 \
  for(size_t i=0; i<1024; i++) {                                 \
    const size_t num_data = dtc::random<size_t>(1, 1024);        \
    dtc::OutputStreamBuffer os;                                        \
    SerializerT oar(os);                                     \
                                                                 \
    std::container <int32_t>     o_int32s  (num_data);           \
    std::container <int64_t>     o_int64s  (num_data);           \
    std::container <char>        o_chars   (num_data);           \
    std::container <float>       o_floats  (num_data);           \
    std::container <double>      o_doubles (num_data);           \
    std::container <std::string> o_strings (num_data);           \
    std::container <PODs>        o_podses  (num_data);           \
                                                                 \
    for(auto& v : o_int32s)  v = dtc::random<int32_t>();         \
    for(auto& v : o_int64s)  v = dtc::random<int64_t>();         \
    for(auto& v : o_chars)   v = dtc::random<char>();            \
    for(auto& v : o_floats)  v = dtc::random<float>();           \
    for(auto& v : o_doubles) v = dtc::random<double>();          \
    for(auto& v : o_strings) v = dtc::random<std::string>();     \
                                                                 \
    auto o_sz = oar(o_int32s, o_int64s, o_chars, o_floats, o_doubles, o_strings, o_podses);\
                                                                 \
    dtc::InputStreamBuffer is(os);                                     \
    DeserializerT iar(is);                                      \
                                                                 \
    std::container <int32_t>     i_int32s;                       \
    std::container <int64_t>     i_int64s;                       \
    std::container <char>        i_chars;                        \
    std::container <float>       i_floats;                       \
    std::container <double>      i_doubles;                      \
    std::container <std::string> i_strings;                      \
    std::container <PODs>        i_podses;                       \
                                                                 \
    auto i_sz = iar(i_int32s, i_int64s, i_chars, i_floats, i_doubles, i_strings, i_podses);\
                                     \
    REQUIRE(o_sz == i_sz);           \
    REQUIRE(o_int32s == i_int32s);   \
    REQUIRE(o_int64s == i_int64s);   \
    REQUIRE(o_chars == i_chars);     \
    REQUIRE(o_floats == i_floats);   \
    REQUIRE(o_doubles == i_doubles); \
    REQUIRE(o_strings == i_strings); \
    REQUIRE(o_podses == i_podses);   \
  }                                                                           


// Procedure: test_vector
// Template procedure for testing vector container.
template <typename SerializerT, typename DeserializerT>
void test_vector() {
  TEST_SEQ_CONT_BODY(vector)
}

// Procedure: test_deque
// Template procedure for testing deque container.
template <typename SerializerT, typename DeserializerT>
void test_deque() {
  TEST_SEQ_CONT_BODY(deque)
}

// Procedure: test_list
// Template procedure for testing list container.
template <typename SerializerT, typename DeserializerT>
void test_list() {
  TEST_SEQ_CONT_BODY(list)
}

// Procedure: test_forward_list
// Template procedure for testing forward list container.
template <typename SerializerT, typename DeserializerT>
void test_forward_list() {
  TEST_SEQ_CONT_BODY(forward_list)
}

#define TEST_MAP_CONT_BODY(container)                                              \
                                                                                   \
  for (size_t i = 0; i < 1024; i++) {                                              \
    const size_t num_data = dtc::random<size_t>(1, 1024);                          \
    dtc::OutputStreamBuffer os;                                                          \
    SerializerT oar(os);                                                       \
                                                                                   \
    std::container<int32_t, int32_t> o_int32s;                                     \
    std::container<int64_t, int64_t> o_int64s;                                     \
    std::container<char, char> o_chars;                                            \
    std::container<float, float> o_floats;                                         \
    std::container<double, double> o_doubles;                                      \
    std::container<std::string, std::string> o_strings;                            \
                                                                                   \
    for (size_t j = 0; j < num_data; j++) {                                        \
      o_int32s.emplace(dtc::random<int32_t>(), dtc::random<int32_t>());            \
      o_int64s.emplace(dtc::random<int64_t>(), dtc::random<int64_t>());            \
      o_chars.emplace(dtc::random<char>(), dtc::random<char>());                   \
      o_floats.emplace(dtc::random<float_t>(), dtc::random<float_t>());            \
      o_doubles.emplace(dtc::random<double_t>(), dtc::random<double_t>());         \
      o_strings.emplace(dtc::random<std::string>(), dtc::random<std::string>());   \
    }                                                                              \
                                                                                   \
    auto o_sz = oar(o_int32s, o_int64s, o_chars, o_floats, o_doubles , o_strings); \
    REQUIRE(o_sz == os.out_avail());                                               \
                                                                                   \
    dtc::InputStreamBuffer is(os);                                                       \
    DeserializerT iar(is);                                                         \
                                                                                   \
    std::container<int32_t, int32_t> i_int32s;                                     \
    std::container<int64_t, int64_t> i_int64s;                                     \
    std::container<char, char> i_chars;                                            \
    std::container<float, float> i_floats;                                         \
    std::container<double, double> i_doubles;                                      \
    std::container<std::string, std::string> i_strings;                            \
                                                                                   \
    auto i_sz = iar(i_int32s, i_int64s, i_chars, i_floats, i_doubles , i_strings); \
    REQUIRE(0 == is.in_avail());                                                   \
                                                                                   \
    REQUIRE(o_sz == i_sz);                                                         \
    REQUIRE(o_int32s == i_int32s);                                                 \
    REQUIRE(o_int64s == i_int64s);                                                 \
    REQUIRE(o_chars == i_chars);                                                   \
    REQUIRE(o_floats == i_floats);                                                 \
    REQUIRE(o_doubles == i_doubles);                                               \
    REQUIRE(o_strings == i_strings);                                               \
  }

// Procedure: test_map
// Template procedure for testing map container.
template <typename SerializerT, typename DeserializerT>
void test_map() {
  TEST_MAP_CONT_BODY(map)
}

// Procedure: test_unordered_map
// Template procedure for testing unordered_map container.
template <typename SerializerT, typename DeserializerT>
void test_unordered_map() {
  TEST_MAP_CONT_BODY(unordered_map);
}

#define TEST_SET_CONT_BODY(container)                                             \
                                                                                  \
  for (size_t i = 0; i < 1024; i++) {                                             \
    const size_t num_data = dtc::random<size_t>(1, 1024);                         \
    dtc::OutputStreamBuffer os;                                                         \
    SerializerT oar(os);                                                      \
                                                                                  \
    std::container<int32_t> o_int32s;                                             \
    std::container<int64_t> o_int64s;                                             \
    std::container<char> o_chars;                                                 \
    std::container<float> o_floats;                                               \
    std::container<double> o_doubles;                                             \
    std::container<std::string> o_strings;                                        \
                                                                                  \
    for (size_t j = 0; j < num_data; j++) {                                       \
      o_int32s.emplace(dtc::random<int32_t>());                                   \
      o_int64s.emplace(dtc::random<int64_t>());                                   \
      o_chars.emplace(dtc::random<char>());                                       \
      o_floats.emplace(dtc::random<float_t>());                                   \
      o_doubles.emplace(dtc::random<double_t>());                                 \
      o_strings.emplace(dtc::random<std::string>());                              \
    }                                                                             \
    auto o_sz = oar(o_int32s, o_int64s, o_chars, o_floats, o_doubles, o_strings); \
    REQUIRE(o_sz == os.out_avail());                                              \
                                                                                  \
    dtc::InputStreamBuffer is(os);                                                      \
    DeserializerT iar(is);                                                        \
                                                                                  \
    std::container<int32_t> i_int32s;                                             \
    std::container<int64_t> i_int64s;                                             \
    std::container<char> i_chars;                                                 \
    std::container<float> i_floats;                                               \
    std::container<double> i_doubles;                                             \
    std::container<std::string> i_strings;                                        \
                                                                                  \
    auto i_sz = iar(i_int32s, i_int64s, i_chars, i_floats, i_doubles, i_strings); \
    REQUIRE(0 == is.in_avail());                                                  \
                                                                                  \
    REQUIRE(o_sz == i_sz);                                                        \
    REQUIRE(o_int32s == i_int32s);                                                \
    REQUIRE(o_int64s == i_int64s);                                                \
    REQUIRE(o_chars == i_chars);                                                  \
    REQUIRE(o_floats == i_floats);                                                \
    REQUIRE(o_doubles == i_doubles);                                              \
    REQUIRE(o_strings == i_strings);                                              \
  }

// Procedure: test_set
// Template procedure for testing set container.
template <typename SerializerT, typename DeserializerT>
void test_set() {
  TEST_SET_CONT_BODY(set)
}

// Procedure: test_unordered_set
// Template procedure for testing unordered_set container.
template <typename SerializerT, typename DeserializerT>
void test_unordered_set() {
  TEST_SET_CONT_BODY(unordered_set);
}

// Procedure: test_array
// Template procedure for testing array container.
template <typename SerializerT, typename DeserializerT>
void test_array() {

  for(size_t i=0; i<1024; ++i) {

    // Output
    std::array<char, 1> ochar;
    std::array<int, 512> oint;
    std::array<double, 1024> odouble;
    std::array<std::string, 2048> ostring;

    for(auto &i : ochar) i = dtc::random<char>();
    for(auto &i : oint) i = dtc::random<int>();
    for(auto &i : odouble) i = dtc::random<double>();
    for(auto &i : ostring) i = dtc::random<std::string>();

    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto osz = oar(ochar, oint, odouble, ostring);
    REQUIRE(osz == os.out_avail());

    // Input
    std::array<char, 1> ichar;
    std::array<int, 512> iint;
    std::array<double, 1024> idouble;
    std::array<std::string, 2048> istring;
    
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);

    auto isz = iar(ichar, iint, idouble, istring);
    REQUIRE(0 == is.in_avail());

    REQUIRE(osz == isz);
    REQUIRE(ochar == ichar);
    REQUIRE(oint == iint);
    REQUIRE(odouble == idouble);
    REQUIRE(ostring == istring);
  } 
}

// Procedure: test_variant
template <typename SerializerT, typename DeserializerT>
void test_variant() {

  for (size_t i = 0; i < 1024; i++) {            
    
    // Single POD variant.
    std::variant<int> opod1 = dtc::random<int>();
    std::variant<int> ipod1 = dtc::random<int>();

    // Multiple POD variant
    std::variant<int, double> opod2 = dtc::random<double>();
    std::variant<int, double> ipod2 = dtc::random<int>();
  
    // Multiple POD variant
    std::variant<int, double, bool> opod3 = dtc::random<int>()%2;
    std::variant<int, double, bool> ipod3 = dtc::random<double>();
    
    // Mixing float and string
    std::variant<float, std::string> omix2 = dtc::random<std::string>();
    std::variant<float, std::string> imix2 = dtc::random<float>();

    // Recursive variant
    std::variant<int, decltype(omix2)> orec2 = omix2;
    std::variant<int, decltype(omix2)> irec2 = dtc::random<int>();

    // Output archiver
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto osz = oar(opod1, opod2, opod3, omix2, orec2);
    REQUIRE(osz == os.out_avail());
    
    // Input archiver
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    auto isz = iar(ipod1, ipod2, ipod3, imix2, irec2);
    REQUIRE(0 == is.in_avail());

    REQUIRE(osz == isz);
    REQUIRE(opod1 == ipod1);
    REQUIRE(opod2 == ipod2);
    REQUIRE(opod3 == ipod3);
    REQUIRE(omix2 == imix2);
    REQUIRE(orec2 == irec2);
  }
}

// Procedure: test_smart_ptr
template <typename SerializerT, typename DeserializerT>
void test_smart_pointer() {

  for (size_t i = 0; i < 1024; i++) {            

    // Single POD.
    std::unique_ptr<int> opod(new int(dtc::random<int>()));
    std::unique_ptr<int> ipod;

    // NULL data
    std::unique_ptr<int> onull;
    std::unique_ptr<int> inull(new int(dtc::random<int>()));

    // String
    auto ostr = std::make_unique<std::string>(dtc::random<std::string>());
    auto istr = std::make_unique<std::string>(dtc::random<std::string>());

    // Output archiver
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto osz = oar(opod, onull, ostr);
    REQUIRE(osz == os.out_avail());
    
    // Input archiver
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    auto isz = iar(ipod, inull, istr);
    REQUIRE(0 == is.in_avail());

    REQUIRE(osz == isz);
    REQUIRE(*opod == *ipod);
    REQUIRE((inull == nullptr && onull == nullptr));
    REQUIRE(*ostr == *istr);
  }
}

// Procedure: test_atomicity
template <typename SerializerT, typename DeserializerT>
void test_atomicity() {

  const size_t W = std::max(2u, std::thread::hardware_concurrency());
  const size_t N = W << 1;
    
  for(size_t i=0; i<1024; ++i) {
    
    // Output field.
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    std::atomic<std::streamsize> o_sz{0};

    std::vector<std::set<std::string>> o_strs(N*W);
    for(auto& str : o_strs) {
      for(int i=dtc::random<int>(1, N); i>=0; --i) {
        str.insert(dtc::random<std::string>());
      }
    }

    std::vector<std::thread> othreads;
    for(size_t i=0; i<W; ++i) {    
      othreads.emplace_back(
        [&oar, &o_strs, &o_sz, beg=i*N, N] () {
          for(size_t j=beg; j<beg+N; ++j) {
            o_sz += oar(o_strs[j]);
          }
        }
      );
    }
    for(auto& t : othreads) t.join();
    REQUIRE(o_sz == os.out_avail());
    
    // Input field.
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    std::atomic<std::streamsize> i_sz{0};

    std::vector<std::set<std::string>> i_strs(N*W);

    std::vector<std::thread> ithreads;
    for(size_t i=0; i<W; ++i) {
      ithreads.emplace_back(
        [&iar, &i_strs, &i_sz, beg=i*N, N] () {
          for(size_t j=beg; j<beg+N; ++j) {
            i_sz += iar(i_strs[j]);
          }
        }
      ); 
    }
    for(auto& t : ithreads) t.join();
    REQUIRE(0 == is.in_avail());

    std::sort(o_strs.begin(), o_strs.end());
    std::sort(i_strs.begin(), i_strs.end());

    REQUIRE(o_sz == i_sz);
    REQUIRE(o_strs == i_strs);
  }
}

// Procedure: test_error_code
template <typename SerializerT, typename DeserializerT>
void test_error_code() {

  for(auto i=0; i<1024; ++i) {            

    std::error_code oeg(i, std::generic_category());
    std::error_code oes(i, std::system_category());
    std::error_code oeio(i, std::iostream_category());
    std::error_code oef(i, std::future_category());
    
    std::error_code ieg, ies, ieio, ief;

    // Output archiver
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto osz = oar(oeg, oes, oeio, oef);
    REQUIRE(osz == os.out_avail());
    
    // Input archiver
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    auto isz = iar(ieg, ies, ieio, ief);
    REQUIRE(0 == is.in_avail());

    REQUIRE((oeg == ieg && oeg != ies && oeg != ieio && oeg != ief));
    REQUIRE((oes == ies && oes != ieg && oes != ieio && oes != ief));
    REQUIRE((oef == ief && oef != ieg && oef != ieio && oef != ies));
    REQUIRE((oeio == ieio && oeio != ieg && oeio != ief && oeio != ies));
  }
}

// Procedure: test_chrono
template <typename SerializerT, typename DeserializerT>
void test_chrono() {

  for(auto i=0; i<1024; ++i) {

    auto o_tpt1 = std::chrono::system_clock::now();
    auto o_tpt2 = std::chrono::steady_clock::now();
    auto o_tpt3 = std::chrono::high_resolution_clock::now();

    auto o_dur1 = std::chrono::system_clock::now() - o_tpt1;
    auto o_dur2 = std::chrono::steady_clock::now() - o_tpt2;
    auto o_dur3 = std::chrono::high_resolution_clock::now() - o_tpt3;
    
    // Output archiver
    dtc::OutputStreamBuffer os;
    SerializerT oar(os);
    auto osz = oar(o_tpt1, o_tpt2, o_tpt3, o_dur1, o_dur2, o_dur3);
    REQUIRE(osz == os.out_avail());

    decltype(o_tpt1) i_tpt1;
    decltype(o_tpt2) i_tpt2;
    decltype(o_tpt3) i_tpt3;
    decltype(o_dur1) i_dur1;
    decltype(o_dur2) i_dur2;
    decltype(o_dur3) i_dur3;

    // Input archiver
    dtc::InputStreamBuffer is(os);
    DeserializerT iar(is);
    auto isz = iar(i_tpt1, i_tpt2, i_tpt3, i_dur1, i_dur2, i_dur3);
    REQUIRE(0 == is.in_avail());

    REQUIRE(o_tpt1 == i_tpt1);
    REQUIRE(o_tpt2 == i_tpt2);
    REQUIRE(o_tpt3 == i_tpt3);
    REQUIRE(o_dur1 == i_dur1);
    REQUIRE(o_dur2 == i_dur2);
    REQUIRE(o_dur3 == i_dur3);
  }
}

// ---- Archiver test -----------------------------------------------------------------------------

// Unittest: ArchiverTest.BinaryPOD
TEST_CASE("ArchiverTest.BinaryPOD") {
  test_pod<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryStruct
TEST_CASE("ArchiverTest.BinaryStruct") {
  test_struct<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryBasicString
TEST_CASE("ArchiverTest.BinaryBasicString") {
  test_string<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryVector
TEST_CASE("ArchiverTest.BinaryVector") {
  test_vector<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryDeque
TEST_CASE("ArchiverTest.BinaryDeque") {
  test_deque<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryList
TEST_CASE("ArchiverTest.BinaryList") {
  test_list<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryForwardList
TEST_CASE("ArchiverTest.BinaryForwardList") {
  test_forward_list<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
} 

// Unittest: ArchiverTest.BinaryMap
TEST_CASE("ArchiverTest.BinaryMap") {
  test_map<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryUnorderedMap
TEST_CASE("ArchiverTest.BinaryUnorderedMap") {
  test_unordered_map<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinarySet
TEST_CASE("ArchiverTest.BinarySet") {
  test_set<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryArray
TEST_CASE("ArchiverTest.BinaryArray") {
  test_array<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.BinaryVariant
TEST_CASE("ArchiverTest.BinaryVariant") {
  test_variant<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.SmartPointer
TEST_CASE("ArchiverTest.SmartPointer") {
  test_smart_pointer<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.ErrorCode
TEST_CASE("ArchiverTest.ErrorCode") {
  test_error_code<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// Unittest: ArchiverTest.Atomicity
TEST_CASE("ArchiverTest.BinaryAtomicity") {
  test_atomicity<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
} 

// Unittest: ArchiverTest.Chrono
TEST_CASE("ArchiverTest.Chrono") {
  test_chrono<dtc::BinaryOutputArchiver, dtc::BinaryInputArchiver>();
}

// ---- Packager test -----------------------------------------------------------------------------

// Unittest: PackagerTest.BinaryPOD
TEST_CASE("PackagerTest.BinaryPOD") {
  test_pod<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryStruct
TEST_CASE("PackagerTest.BinaryStruct") {
  test_struct<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryBasicString
TEST_CASE("PackagerTest.BinaryBasicString") {
  test_string<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryVector
TEST_CASE("PackagerTest.BinaryVector") {
  test_vector<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryDeque
TEST_CASE("PackagerTest.BinaryDeque") {
  test_deque<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryList
TEST_CASE("PackagerTest.BinaryList") {
  test_list<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryForwardList
TEST_CASE("PackagerTest.BinaryForwardList") {
  test_forward_list<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
} 

// Unittest: PackagerTest.BinaryMap
TEST_CASE("PackagerTest.BinaryMap") {
  test_map<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryUnorderedMap
TEST_CASE("PackagerTest.BinaryUnorderedMap") {
  test_unordered_map<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinarySet
TEST_CASE("PackagerTest.BinarySet") {
  test_set<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryArray
TEST_CASE("PackagerTest.BinaryArray") {
  test_array<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.BinaryVariant
TEST_CASE("PackagerTest.BinaryVariant") {
  test_variant<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.SmartPointer
TEST_CASE("PackagerTest.SmartPointer") {
  test_smart_pointer<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.ErrorCode
TEST_CASE("PackagerTest.ErrorCode") {
  test_error_code<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.Atomicity
TEST_CASE("PackagerTest.BinaryAtomicity") {
  test_atomicity<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}

// Unittest: PackagerTest.Chrono
TEST_CASE("PackagerTest.Chrono") {
  test_chrono<dtc::BinaryOutputPackager, dtc::BinaryInputPackager>();
}




