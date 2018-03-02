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

#ifndef DTC_ARCHIVE_BINARY_HPP_
#define DTC_ARCHIVE_BINARY_HPP_

#include <dtc/archive/archive.hpp>
#include <dtc/ipc/streambuf.hpp>

namespace dtc {

// Forward archiver declaration.
class BinaryOutputArchiver;
class BinaryInputArchiver;

//-------------------------------------------------------------------------------------------------

// Class: BinaryOutputArchiver
// Binary output archiver is designed to store data in 0/1 binary representation. For example,
// an int32_t is represented only in 4 bytes. The binary output archiver should be initialied
// with a std::ostream object. Data will be stored in a compact way to the underlying streambuffer.
class BinaryOutputArchiver {

  friend class BinaryOutputPackager;

  public:      
    
    inline BinaryOutputArchiver(OutputStreamBuffer&);
    
    template <typename... T>
    std::streamsize operator()(T&&...);

  private:

    OutputStreamBuffer& _osbuf;
    
    template <typename T>
    std::streamsize _archive(T&& t);
};

// Constructor
inline BinaryOutputArchiver::BinaryOutputArchiver(OutputStreamBuffer& ostream) : _osbuf(ostream) {
}
    
// Operator: main archiver function
template <typename... T>
std::streamsize BinaryOutputArchiver::operator()(T&&... t) {
  std::scoped_lock lock(_osbuf._mutex);
  return (_archive(std::forward<T>(t)) + ... );
}
    
// Function: _archive
template <typename T>
std::streamsize BinaryOutputArchiver::_archive(T&& t) {

  using U = std::decay_t<T>;
  
  if constexpr(std::is_arithmetic_v<U>) {
    return _osbuf._write(std::addressof(t), sizeof(t));
  }
  else if constexpr(is_std_basic_string_v<U>) {
    return _archive(make_size_tag(t.size())) +
           _osbuf._write(t.data(), t.size()*sizeof(typename U::value_type));
  }
  else if constexpr(is_std_vector_v<U>) {
    if constexpr (std::is_arithmetic_v<typename U::value_type>) {
      return _archive(make_size_tag(t.size())) + 
             _osbuf._write(t.data(), t.size() * sizeof(typename U::value_type));
    } else {
      auto sz = _archive(make_size_tag(t.size()));
      for(auto&& item : t) {
        sz += _archive(item);
      }
      return sz;
    }
  }
  else if constexpr(is_std_deque_v<U> || is_std_list_v<U>) {
    auto sz = _archive(make_size_tag(t.size()));
    for(auto&& item : t) {
      sz += _archive(item);
    }
    return sz;
  }
  else if constexpr(is_std_forward_list_v<U>) {
    auto sz = _archive(make_size_tag(std::distance(t.begin(), t.end())));
    for(auto&& item : t) {
      sz += _archive(item);
    }
    return sz;
  }
  else if constexpr(is_std_map_v<U> || is_std_unordered_map_v<U>) {
    auto sz = _archive(make_size_tag(t.size()));
    for(auto&& [k, v] : t) {
      sz += _archive(make_kv_pair(k, v));
    }
    return sz;
  }
  else if constexpr(is_std_set_v<U> || is_std_unordered_set_v<U>) {
    auto sz = _archive(make_size_tag(t.size()));
    for(auto&& item : t) {
      sz += _archive(item);
    }
    return sz;
  }
  else if constexpr(std::is_enum_v<U>) {
    return _archive(static_cast<std::underlying_type_t<U>>(t));
  }
  else if constexpr(is_std_array_v<U>) {
    static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");

    if constexpr(std::is_arithmetic_v<typename U::value_type>) {
      return _osbuf._write(t.data(), sizeof(t));
    } 
    else {
      auto sz = std::streamsize {0};
      for(auto&& item : t) {
        sz += _archive(item);
      }
      return sz;
    }
  }
  else if constexpr(is_std_variant_v<U>) {
    return _archive(t.index()) + std::visit([&](auto&& arg){ return _archive(arg);}, t);
  }
  else if constexpr(is_std_unique_ptr_v<U>) {
    if(t == nullptr) return _archive(uint8_t(0));
    return _archive(uint8_t(1)) + _archive(*t);
  }
  else if constexpr(is_std_error_code_v<U>) {
    return _archive(ErrorCodeItem(t));
  }
  else if constexpr(is_std_duration_v<U>) {
    return _archive(t.count());
  }
  else if constexpr(is_std_time_point_v<U>) {
    return _archive(t.time_since_epoch());
  }
  else if constexpr(is_std_optional_v<U>) {
    if(bool flag = t.has_value(); flag) {
      return _archive(flag) + _archive(*t);
    }
    else {
      return _archive(flag);
    }
  }
  else if constexpr(is_std_tuple_v<U>) {
    return std::apply(
      [this] (auto&&... args) {
        return (_archive(std::forward<decltype(args)>(args)) + ... + 0); 
      },
      std::forward<T>(t)
    );
  }
  else if constexpr(is_eigen_matrix_v<U>) {
    std::streamsize sz {0};
    auto rows = t.rows();
    auto cols = t.cols();
    sz += _osbuf._write(&rows, sizeof(rows));
    sz += _osbuf._write(&cols, sizeof(cols));
    sz += _osbuf._write(t.data(), t.size() * sizeof(typename U::Scalar));
    return sz;
  }
  // Fall back to user-defined archive method.
  else {
    return t.archive(*this);
  }
}


//-------------------------------------------------------------------------------------------------

// Class: BinaryInputArchiver
// Binary input archiver is designed to store data in 0/1 binary representation. For example,
// an int32_t is represented only in 4 bytes. The binary input archiver should be initialied
// with a std::istream object. Data will be stored in a compact way to the underlying streambuffer.
class BinaryInputArchiver {
  
  friend class BinaryInputPackager;

  public:      
    
    inline BinaryInputArchiver(InputStreamBuffer&);
    
    template <typename... T>
    std::streamsize operator()(T&&...);

  private:

    InputStreamBuffer& _isbuf;
    
    template <typename T>
    std::streamsize _archive(T&&);
    
    // Function: _variant_helper
    template <size_t I = 0, typename... ArgsT, std::enable_if_t<I==sizeof...(ArgsT)>* = nullptr>
    std::streamsize _variant_helper(const size_t, std::variant<ArgsT...>&);
    
    // Function: _variant_helper
    template <size_t I = 0, typename... ArgsT, std::enable_if_t<I<sizeof...(ArgsT)>* = nullptr>
    std::streamsize _variant_helper(const size_t, std::variant<ArgsT...>&);

};

// Constructor
inline BinaryInputArchiver::BinaryInputArchiver(InputStreamBuffer& istream) : _isbuf(istream) {
}
    
// Operator ()
template <typename... T>
std::streamsize BinaryInputArchiver::operator()(T&&... args) {
  std::scoped_lock lock(_isbuf._mutex);
  //return _archive(std::forward<T>(args)...);
  return (_archive(std::forward<T>(args)) + ...);
}

// Function: _archive
template <typename T>
std::streamsize BinaryInputArchiver::_archive(T&& t) {

  using U = std::decay_t<T>;

  if constexpr(std::is_arithmetic_v<U>) {
    return _isbuf._read(std::addressof(t), sizeof(t));
  }
  else if constexpr(is_std_basic_string_v<U>) {
    typename U::size_type num_chars;
    auto sz = _archive(make_size_tag(num_chars));
    t.resize(num_chars);
    return sz + _isbuf._read(t.data(), num_chars*sizeof(typename U::value_type));
  }
  else if constexpr(is_std_vector_v<U>) {

    typename U::size_type num_data;
    if constexpr(std::is_arithmetic_v<typename U::value_type>) {
      auto sz = _archive(make_size_tag(num_data));
      t.resize(num_data);
      return sz + _isbuf._read(t.data(), num_data * sizeof(typename U::value_type));
    } 
    else {
      auto sz = _archive(make_size_tag(num_data));
      t.resize(num_data);
      for(auto && v : t) {
        sz += _archive(v);
      }
      return sz;
    }
  }
  else if constexpr(is_std_deque_v<U> || is_std_list_v<U> || is_std_forward_list_v<U>) {

    typename U::size_type num_data;
    auto sz = _archive(make_size_tag(num_data));

    t.resize(num_data);
    for(auto && v : t) {
      sz += _archive(v);
    }
    return sz;
  }
  else if constexpr(is_std_map_v<U>) {

    typename U::size_type num_data;
    auto sz = _archive(make_size_tag(num_data));
    
    t.clear();
    auto hint = t.begin();
      
    typename U::key_type k;
    typename U::mapped_type v;

    for(size_t i=0; i<num_data; ++i) {
      sz += _archive(make_kv_pair(k, v));
      hint = t.emplace_hint(hint, std::move(k), std::move(v));
    }
    return sz;
  }
  else if constexpr(is_std_unordered_map_v<U>) {

    typename U::size_type num_data;
    auto sz = _archive(make_size_tag(num_data));

    t.clear();
    t.reserve(num_data);

    typename U::key_type k;
    typename U::mapped_type v;

    for(size_t i=0; i<num_data; ++i) {
      sz += _archive(make_kv_pair(k, v));
      t.emplace(std::move(k), std::move(v));
    }
    
    return sz;
  }
  else if constexpr(is_std_set_v<U>) {

    typename U::size_type num_data;
    auto sz = _archive(make_size_tag(num_data));

    t.clear();
    auto hint = t.begin();
      
    typename U::key_type k;

    for(size_t i=0; i<num_data; ++i) {   
      sz += _archive(k);
      hint = t.emplace_hint(hint, std::move(k));
    }   
    return sz;
  }
  else if constexpr(is_std_unordered_set_v<U>) {

    typename U::size_type num_data;
    auto sz = _archive(make_size_tag(num_data));

    t.clear();
    t.reserve(num_data);
      
    typename U::key_type k;

    for(size_t i=0; i<num_data; ++i) {   
      sz += _archive(k);
      t.emplace(std::move(k));
    }   
    return sz;
  }
  else if constexpr(std::is_enum_v<U>) {
    std::underlying_type_t<U> k;
    auto sz = _archive(k);
    t = static_cast<U>(k);
    return sz;
  }
  else if constexpr(is_std_array_v<U>) {
    static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");
      
    if constexpr(std::is_arithmetic_v<typename U::value_type>) {
      return _isbuf._read(t.data(), sizeof(t));
    } 
    else {
      auto sz = std::streamsize{0};
      for(auto && v : t) {
        sz += _archive(v);
      }
      return sz;
    }
  }
  else if constexpr(is_std_variant_v<U>) {
    std::decay_t<decltype(t.index())> idx;
    auto s = _archive(idx);
    return s + _variant_helper(idx, t);
  }
  else if constexpr(is_std_unique_ptr_v<U>) {
    uint8_t flag;
    auto s = _archive(flag);

    if(flag == 0) {
      t.reset(nullptr);
    } 
    else {
      //auto g = std::make_unique<typename U::element_type>();
      U g(new typename U::element_type());
      s += _archive(*g);
      t.reset(g.release());
    }
    return s;
  }
  else if constexpr(is_std_error_code_v<U>) {
    ErrorCodeItem i;
    auto s = _archive(i);
    t = i.to_error_code();
    return s;
  }
  else if constexpr(is_std_duration_v<U>) {
    typename U::rep count;
    auto s = _archive(count);
    t = U{count};
    return s;
  }
  else if constexpr(is_std_time_point_v<U>) {
    typename U::duration elapsed;
    auto s = _archive(elapsed);
    t = U{elapsed};
    return s;
  }
  else if constexpr(is_std_optional_v<U>) {
    bool has_value;
    auto s = _archive(has_value);
    if(has_value) {
      if(!t) {
        t = typename U::value_type();
      }
      s += _archive(*t);
    }
    else {
      t.reset(); 
    }
    return s;
  }
  else if constexpr(is_std_tuple_v<U>) {
    return std::apply(
      [this] (auto&&... args) {
        return (_archive(std::forward<decltype(args)>(args)) + ... + 0); 
      },
      std::forward<T>(t)
    );
  }
  else if constexpr(is_eigen_matrix_v<U>) {
    std::streamsize sz {0};
    auto rows = t.rows();
    auto cols = t.cols();
    sz += _isbuf._read(&rows, sizeof(rows));
    sz += _isbuf._read(&cols, sizeof(cols));
    t.resize(rows, cols);
    sz += _isbuf._read(t.data(), t.size() * sizeof(typename U::Scalar));
    return sz;
  }
  else {
    return t.archive(*this);
  }
}

// Function: _variant_helper
template <size_t I, typename... ArgsT, std::enable_if_t<I==sizeof...(ArgsT)>*>
std::streamsize BinaryInputArchiver::_variant_helper(const size_t i, std::variant<ArgsT...>& v) {
  return 0;
}


// Function: _variant_helper
template <size_t I, typename... ArgsT, std::enable_if_t<I<sizeof...(ArgsT)>*>
std::streamsize BinaryInputArchiver::_variant_helper(const size_t i, std::variant<ArgsT...>& v) {
  if(i == 0) {
    using type = extract_type_t<I, std::variant<ArgsT...>>;
    if(v.index() != I) {
      static_assert(
        std::is_default_constructible<type>::value, 
        "Failed to archive variant (type should be default constructible T())"
      );
      v = type();
    }
    return _archive(std::get<type>(v));
  }
  return _variant_helper<I+1, ArgsT...>(i-1, v);
}

//-------------------------------------------------------------------------------------------------

// Class: BinaryOutputPackager
// BinaryOutputArchiver-based packager.
class BinaryOutputPackager {

  private:

    BinaryOutputArchiver ar;

  public:

    inline BinaryOutputPackager(OutputStreamBuffer&);

    template <typename... T>
    std::streamsize operator()(T&&...);
}; 

// Constructor
inline BinaryOutputPackager::BinaryOutputPackager(OutputStreamBuffer& os) : ar(os) {}
    
// Operator ()
template <typename... T>
std::streamsize BinaryOutputPackager::operator()(T&&... items) {
  std::streamsize sz;
  std::scoped_lock lock(ar._osbuf._mutex);
  sz = ar(sz, std::forward<T>(items)...);
  if(sz != -1) {
    *reinterpret_cast<std::streamsize*>(ar._osbuf._pptr - sz) = sz;
  }
  return sz;
}

//-------------------------------------------------------------------------------------------------

// Class: BinaryInputPackager
// BinaryInputArchiver-based packager.
class BinaryInputPackager {
  
  private:
    
    BinaryInputArchiver ar;
     
  public:
    
    inline BinaryInputPackager(InputStreamBuffer&);

    template <typename... T>
    std::streamsize operator()(T&&...);

    operator bool () const {
      std::scoped_lock lock(ar._isbuf._mutex);
      std::streamsize sz = ar._isbuf._in_avail();
      return (sz >= static_cast<std::streamsize>(sizeof(std::streamsize)) && 
              sz >= *reinterpret_cast<std::streamsize*>(ar._isbuf._gptr));
    }
};

// Constructor.
inline BinaryInputPackager::BinaryInputPackager(InputStreamBuffer& is) : ar{is} {}

// Operator ()
template <typename... T>
std::streamsize BinaryInputPackager::operator()(T&&... items) {
  std::scoped_lock lock(ar._isbuf._mutex);
  std::streamsize sz = ar._isbuf._in_avail();
  if(sz >= static_cast<std::streamsize>(sizeof(std::streamsize)) && 
     sz >= *reinterpret_cast<std::streamsize*>(ar._isbuf._gptr)) {
    return ar(sz, std::forward<T>(items)...);
  }
  return -1;
}


};  // End of namespace cdt::archive. -------------------------------------------------------------



#endif
