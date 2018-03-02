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

#ifndef DTC_TRAITS_HPP_
#define DTC_TRAITS_HPP_

#include <functional>
#include <tuple>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <stack>
#include <type_traits>
#include <memory>
#include <map>
#include <set>
#include <variant>
#include <chrono>
#include <any>
#include <unordered_map>
#include <unordered_set>

#include <dtc/Eigen/Core>

// Namespace: dtc
namespace dtc {

//-------------------------------------------------------------------------------------------------
// stl type query
//-------------------------------------------------------------------------------------------------

template <typename T> struct is_std_basic_string : std::false_type {};
template <typename... ArgsT> struct is_std_basic_string <std::basic_string<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_basic_string_v = is_std_basic_string<T>::value;

template <typename T> struct is_std_array : std::false_type {};
template <typename T, size_t N> struct is_std_array <std::array<T, N>> : std::true_type {};
template <typename T> constexpr bool is_std_array_v = is_std_array<T>::value;

template <typename T> struct is_std_vector : std::false_type {};
template <typename... ArgsT> struct is_std_vector <std::vector<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_vector_v = is_std_vector<T>::value;

template <typename T> struct is_std_deque : std::false_type {};
template <typename... ArgsT> struct is_std_deque <std::deque<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_deque_v = is_std_deque<T>::value;

template <typename T> struct is_std_list : std::false_type {};
template <typename... ArgsT> struct is_std_list <std::list<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_list_v = is_std_list<T>::value;

template <typename T> struct is_std_forward_list : std::false_type {};
template <typename... ArgsT> struct is_std_forward_list <std::forward_list<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_forward_list_v = is_std_forward_list<T>::value;

template <typename T> struct is_std_map : std::false_type {};
template <typename... ArgsT> struct is_std_map <std::map<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_map_v = is_std_map<T>::value;

template <typename T> struct is_std_unordered_map : std::false_type {};
template <typename... ArgsT> struct is_std_unordered_map <std::unordered_map<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_unordered_map_v = is_std_unordered_map<T>::value;

template <typename T> struct is_std_set : std::false_type {};
template <typename... ArgsT> struct is_std_set <std::set<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_set_v = is_std_set<T>::value;

template <typename T> struct is_std_unordered_set : std::false_type {};
template <typename... ArgsT> struct is_std_unordered_set <std::unordered_set<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_unordered_set_v = is_std_unordered_set<T>::value;

template <typename T> struct is_std_variant : std::false_type {};
template <typename... ArgsT> struct is_std_variant <std::variant<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_variant_v = is_std_variant<T>::value;

template <typename T> struct is_std_optional : std::false_type {};
template <typename... ArgsT> struct is_std_optional <std::optional<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_optional_v = is_std_optional<T>::value;

template <typename T> struct is_std_unique_ptr : std::false_type {};
template <typename... ArgsT> struct is_std_unique_ptr <std::unique_ptr<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_unique_ptr_v = is_std_unique_ptr<T>::value;

template <typename T> struct is_std_shared_ptr : std::false_type {};
template <typename... ArgsT> struct is_std_shared_ptr <std::shared_ptr<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_shared_ptr_v = is_std_shared_ptr<T>::value;

template <typename T> struct is_std_error_code : std::false_type {};
template <> struct is_std_error_code <std::error_code> : std::true_type {};
template <typename T> constexpr bool is_std_error_code_v = is_std_error_code<T>::value;

template <typename T> struct is_std_duration : std::false_type {};
template <typename... ArgsT> struct is_std_duration<std::chrono::duration<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_duration_v = is_std_duration<T>::value;

template <typename T> struct is_std_time_point : std::false_type {};
template <typename... ArgsT> struct is_std_time_point<std::chrono::time_point<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_time_point_v = is_std_time_point<T>::value;

template <typename T> struct is_std_tuple : std::false_type {};
template <typename... ArgsT> struct is_std_tuple<std::tuple<ArgsT...>> : std::true_type {};
template <typename T> constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

template <typename T> struct is_eigen_matrix : std::false_type {};
template <typename S, int R, int C, int O, int MR, int MC>
struct is_eigen_matrix<Eigen::Matrix<S, R, C, O, MR, MC>> : std::true_type {};
template <typename T> constexpr bool is_eigen_matrix_v = is_eigen_matrix<T>::value;


// Pairwise Boolean AND - Generic definition.
template <bool... Vs> struct all_true;

// Pairwise Boolean AND - Partial base definition.
template <bool V> struct all_true<V> : std::integral_constant<bool, V> {}; 

// Pairwise Boolean AND - Partial recursive definition.
template <bool V, bool... Vs> 
struct all_true<V, Vs...> : std::integral_constant<bool, V && all_true<Vs...>::value> {}; 

// Pairwise Boolean OR - Generic definition.
template <bool... Vs> struct any_true;

// Pairwise Boolean OR - Partial base definition.
template <bool V> struct any_true<V> : std::integral_constant<bool, V> {}; 

// Pairwise Boolean OR - Partial recursive definition.
template <bool V, bool... Vs> 
struct any_true<V, Vs...> : std::integral_constant<bool, V || any_true<Vs...>::value> {}; 

// 
template <class T, class C> inline
C const & container( std::queue<T, C> const & queue )
{
  struct H : public std::queue<T, C>
  {
    static C const & get( std::queue<T, C> const & q )
    {    
      return q.*(&H::c);
    }    
  };
  return H::get( queue );
}

//-------------------------------------------------------------------------------------------------
// Closure traits
//-------------------------------------------------------------------------------------------------

template <typename T>
struct closure_traits : closure_traits<decltype(&T::operator())> {};

#define DTC_CLOSURE_REM_CTOR(...) __VA_ARGS__
#define DTC_CLOSURE_TRAITS_SPEC(cv, var, is_var)                                \
template <typename C, typename R, typename... Args>                             \
struct closure_traits<R (C::*) (Args... DTC_CLOSURE_REM_CTOR var) cv>{          \
  using arity = std::integral_constant<std::size_t, sizeof...(Args) >;          \
  using is_variadic = std::integral_constant<bool, is_var>;                     \
  using is_const    = std::is_const<int cv>;                                    \
                                                                                \
  using result_type = R;                                                        \
                                                                                \
  template <std::size_t i>                                                      \
  using arg = std::tuple_element_t<i, std::tuple<Args...>>;                     \
                                                                                \
  template <std::size_t i>                                                      \
  using decay_arg = std::tuple_element_t<i, std::tuple<std::decay_t<Args>...>>; \
                                                                                \
  using args = std::tuple<std::decay_t<Args>...>;                               \
  using decay_args = std::tuple<std::decay_t<Args>...>;                         \
};

DTC_CLOSURE_TRAITS_SPEC(const, (,...), 1)
DTC_CLOSURE_TRAITS_SPEC(const, (), 0)
DTC_CLOSURE_TRAITS_SPEC(, (,...), 1)
DTC_CLOSURE_TRAITS_SPEC(, (), 0)

//-------------------------------------------------------------------------------------------------
// Type extraction.
//-------------------------------------------------------------------------------------------------

// extract_type: forward declaration
template <size_t, typename> 
struct extract_type;

// extract_type_t: alias interface
template <size_t idx, typename C>
using extract_type_t = typename extract_type<idx, C>::type;

// extract_type: base
template <template <typename...> typename C, typename T, typename... RestT>
struct extract_type <0, C<T, RestT...>> {
  using type = T;
};

// extract_type: base
template <typename T>
struct extract_type <0, T> {
  using type = T;
};

// extract_type: recursive definition.
template <size_t idx, template <typename...> typename C, typename T, typename... RestT>
struct extract_type <idx, C<T, RestT...>> : extract_type<idx-1, C<RestT...>> {
};

// index_of_type: find the index of the given type in a parameter pack.
template <typename T, size_t N, typename... Ts>
struct index_of_type;

template <typename T, typename... Ts>
struct index_of_type<T, 1u, T, Ts...> : std::integral_constant<size_t, 0> {};

template <typename T, size_t N, typename... Ts>
struct index_of_type<T, N, T, Ts...> : std::integral_constant<size_t, index_of_type<T, N-1, Ts...>::value + 1> {};

template <typename T, size_t N, typename U, typename... Ts>
struct index_of_type<T, N, U, Ts...> : std::integral_constant<size_t, index_of_type<T, N, Ts...>::value + 1> {};

template <typename T, size_t N, typename... Ts>
constexpr auto index_of_type_v = index_of_type<T, N, Ts...>::value;

template <size_t Offset, size_t... Indices>
constexpr std::index_sequence<Offset + Indices... > add(std::index_sequence<Indices...>);

template <size_t Beg, size_t End>
using make_index_range = decltype(add<Beg>(std::make_index_sequence<End-Beg>()));

template <typename T, size_t B, size_t E, typename... ArgsT>
struct ConstructFromArguments {

  std::tuple<ArgsT...> tuple;
  
  ConstructFromArguments(ArgsT&&... args) : tuple {std::forward<ArgsT>(args)...} {}

  T operator()() {
    return gen(make_index_range<B, E>());
  }

  template <size_t... Indices>
  T gen(std::index_sequence<Indices...>) {
    return T(std::get<Indices>(tuple)...);
  }
};

template <typename T, size_t B, size_t E, typename... ArgsT>
struct MakeSharedFromArguments {

  std::tuple<ArgsT...> tuple;
  
  MakeSharedFromArguments(ArgsT&&... args) : tuple {std::forward<ArgsT>(args)...} {}

  std::shared_ptr<T> operator()() {
    return gen(make_index_range<B, E>());
  }

  template <size_t... Indices>
  std::shared_ptr<T> gen(std::index_sequence<Indices...>) {
    return std::make_shared<T>(std::get<Indices>(tuple)...);
  }
};

// Get the nth element from a parameter.
template<size_t I, typename T, typename... Ts, std::enable_if_t<I==0>* = nullptr>
constexpr auto get(T&& t, Ts&&... ts) {
  return t;
}

template<size_t I, typename T, typename... Ts, std::enable_if_t<(I>0 && I<=sizeof...(Ts))>* = nullptr>
constexpr auto get(T&& t, Ts&&... ts) {
    return get<I-1>(std::forward<Ts>(ts)...);
}

//-------------------------------------------------------------------------------------------------
// Functors.
//-------------------------------------------------------------------------------------------------

// Overloadded.
template <typename... Ts>
struct Functors : Ts... { 
  using Ts::operator()... ;
};

template <typename... Ts>
Functors(Ts...) -> Functors<Ts...>;

//-------------------------------------------------------------------------------------------------
// Optional
//-------------------------------------------------------------------------------------------------

// Composition of optional object.
template <typename T>
struct add_optionality {
  using type = std::optional<T>;
};

template <typename T>
struct add_optionality < std::optional<T> > {
  using type = std::optional<T>;
};

template <>
struct add_optionality <void> {
  using type = void;
};

template <typename T>
using add_optionality_t = typename add_optionality<T>::type;

template <typename T, typename L>
auto operator | (std::optional<T>& opt, L&& lambda) -> add_optionality_t<decltype(lambda(*opt))> {
  if(opt) {
    return lambda(*opt);
  }
  else
    return {};
}

//-------------------------------------------------------------------------------------------------
// Tuple
//-------------------------------------------------------------------------------------------------

template <int Trim, typename... T, std::size_t... I>
auto __trim_tuple_impl(const std::tuple<T...>& t, std::index_sequence<I...>) {
  return std::make_tuple(std::get<I+Trim>(t)...);
}

template <int Trim, typename... T>
auto __trim_tuple(const std::tuple<T...>& t) {
  return __trim_tuple_impl<Trim>(t, std::make_index_sequence<sizeof...(T) - Trim>());
}

template <int Trim, typename T>
struct trim_tuple {
	using type = decltype(__trim_tuple<Trim>(std::declval<T>()));
};

template <int Trim, typename T>
using trim_tuple_t = typename trim_tuple<Trim, T>::type;

// Make shared from tuple
template <class T, class Tuple, std::size_t... I>
constexpr auto make_shared_from_tuple_impl( Tuple&& t, std::index_sequence<I...> ) {
  return std::make_shared<T>(std::get<I>(std::forward<Tuple>(t))...);
}
 
template <class T, class Tuple>
constexpr auto make_shared_from_tuple( Tuple&& t ) {
  return make_shared_from_tuple_impl<T>(
    std::forward<Tuple>(t),
    std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{}
  );
}

//-------------------------------------------------------------------------------------------------
// Dependent false.
//-------------------------------------------------------------------------------------------------

// Dependent false
template <typename T> 
struct dependent_false : std::false_type {};

template <typename T>
constexpr auto dependent_false_v = dependent_false<T>::value;

};  // End of namespace dtc. --------------------------------------------------------------


#endif



