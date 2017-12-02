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

#ifndef DTC_ARCHIVE_ARCHIVE_HPP_
#define DTC_ARCHIVE_ARCHIVE_HPP_

#include <dtc/headerdef.hpp>

namespace dtc {

// Struct: SizeTag
// Class that wraps a given size item which can be customized. Notice that the member stores
// a reference to the given item if it is passed by reference (by default), or a copy to that
// item otherwise.
template <typename T>
class SizeTag {

  public: 
  
    using type = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::decay_t<T>>;
    
    SizeTag(T&& item) : _item(std::forward<T>(item)) {}
    
    SizeTag& operator = (const SizeTag&) = delete;

    inline const T& get() const {return _item;}

    template <typename ArchiverT>
    std::streamsize archive(ArchiverT & ar) { return ar(_item); }

  private:

    type _item;
};

// Function: make_size_tag
template <typename T>
SizeTag<T> make_size_tag(T&& t) {
  return { std::forward<T>(t) };
}

//-------------------------------------------------------------------------------------------------

// Class: ErrorCodeItem
class ErrorCodeItem {

  enum Category {
    GENERIC,
    SYSTEM,
    IOSTREAM,
    FUTURE,
  };


  public:

    ErrorCodeItem() = default;

    ErrorCodeItem(const std::error_code& c) {
      if(c.category() == std::generic_category()) {
        _category = GENERIC;
      }
      else if(c.category() == std::system_category()) {
        _category = SYSTEM;
      }
      else if(c.category() == std::iostream_category()) {
        _category = IOSTREAM;
      }
      else if(c.category() == std::future_category()) {
        _category = FUTURE;
      }
      //else if(c.category() == DtCraftCategory::get()) {
      //  _category = DTCRAFT;
      //}
      else {
        throw std::invalid_argument("unsupported category in ErrorCodeItem");
      }
      _value = c.value();
    }

    std::error_code to_error_code() const {
      if(_category == GENERIC) {
        return std::error_code(_value, std::generic_category());
      }
      else if(_category == SYSTEM) {
        return std::error_code(_value, std::system_category());
      }
      else if(_category == IOSTREAM) {
        return std::error_code(_value, std::iostream_category());
      }
      else if(_category == FUTURE) {
        return std::error_code(_value, std::future_category());
      }
      //else if(_category == DTCRAFT) {
      //  return std::error_code(_value, DtCraftCategory::get());
      //}
      else {
        throw std::invalid_argument("unsupported category in ErrorCodeItem");
      }
    }
    
    template <typename ArchiverT>
    std::streamsize archive(ArchiverT & ar) { 
      return ar(_category, _value); 
    }
  
  private:
    
    Category _category;
    int _value;
};

//-------------------------------------------------------------------------------------------------

// Class: MapItem
template <typename KeyT, typename ValueT>
class MapItem {
  
  public:
  
    using KeyType = std::conditional_t <std::is_lvalue_reference_v<KeyT>, KeyT, std::decay_t<KeyT>>;
    using ValueType = std::conditional_t <std::is_lvalue_reference_v<ValueT>, ValueT, std::decay_t<ValueT>>;

    MapItem(KeyT&& k, ValueT&& v) : _key(std::forward<KeyT>(k)), _value(std::forward<ValueT>(v)) {}
    MapItem& operator = (const MapItem&) = delete;

    inline const KeyT& key() const { return _key; }
    inline const ValueT& value() const { return _value; }

    template <typename ArchiverT>
    std::streamsize archive(ArchiverT & ar) { return ar(_key, _value); }

  private:

    KeyType _key;
    ValueType _value;
};

// Function: make_kv_pair
template <typename KeyT, typename ValueT>
MapItem<KeyT, ValueT> make_kv_pair(KeyT&& k, ValueT&& v) {
  return { std::forward<KeyT>(k), std::forward<ValueT>(v) };
}

//-------------------------------------------------------------------------------------------------

// Template: has_member_archive
// Return true if the given object is archivable through its member procedure archive. A valid
// archive member must follow the signature 
//
// template <typename ArchiverT>
// void archive(ArchiverT &) { ... }
// 
template <typename A, typename I, typename E = void> struct has_member_archive
: std::false_type {};

template <typename T, typename A> 
struct has_member_archive_impl {

  using yes = char [1];
  using no  = char [2];

  template<typename ArchiverT, typename ItemT> inline
  static auto member_archive_helper(ArchiverT && ar, ItemT && t) -> decltype(t.archive(ar)) {}

  template <typename TT, typename AA>   
  static auto test(int) -> decltype( 
    member_archive_helper(std::declval<AA>(), std::declval<TT>()), std::declval<yes>()   
  ); 

  template <typename, typename> static auto test(...) -> decltype(std::declval<no>()); 

  static constexpr bool value = (sizeof(decltype(test<T, A>(0))) == sizeof(yes));
};                          

template <typename A, typename I>
constexpr bool has_member_archive_v = has_member_archive<A, I>::value;


};  // End of namespace dtc -----------------------------------------------------------------------



#endif








