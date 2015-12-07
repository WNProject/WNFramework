// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "WNCore/inc/Internal/WNExtendedTypes.h"

#include <type_traits>

namespace wn {
namespace core {

// type trait constants

using std::integral_constant;
using std::true_type;
using std::false_type;

// supported operations

using std::is_constructible;

// type modifications

using std::add_lvalue_reference;
using std::add_rvalue_reference;
using std::add_const;
using std::add_volatile;
using std::add_cv;

// type properties

using std::is_const;

// type transformations

using std::enable_if;
using std::common_type;
using std::result_of;
using std::decay;

// relationships and property queries

using std::is_same;
using std::is_convertible;

template <const size_t Value>
using index_constant = integral_constant<size_t, Value>;

#ifdef _WN_HAS_CPP17_STD_BOOL_CONSTANT
template <const bool Value>
using bool_constant = std::bool_constant<Value>;
#else
template <const bool Value>
using bool_constant = integral_constant<bool, Value>;
#endif

#ifdef _WN_HAS_CPP14_STD_INTEGER_SEQUENCE
template <typename T, const T... Values>
using integral_sequence = std::integer_sequence<T, Values...>;

template <const size_t... Values>
using index_sequence = std::index_sequence<Values...>;
#else
template <typename T, const T... Values>
struct integral_sequence {
  using value_type = T;

  static WN_FORCE_INLINE size_t size() {
    return sizeof...(Values);
  }
};

template <const wn_size_t... Values>
using index_sequence = integral_sequence<size_t, Values...>;
#endif

template <const bool... Values>
using bool_sequence = integral_sequence<bool, Values...>;

#ifdef _WN_HAS_CPP14_STD_ENABLE_IF_T
template <const bool Test, typename T = void>
using enable_if_t = std::enable_if_t<Test, T>;
#else
template <const bool Test, typename T = void>
using enable_if_t = typename enable_if<Test, T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_DECAY_T
template <typename T>
using decay_t = std::decay_t<T>;
#else
template <typename T>
using decay_t = typename decay<T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_ADD_LVALUE_REFERENCE_T
template <typename T>
using add_lvalue_reference_t = std::add_lvalue_reference_t<T>;
#else
template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_ADD_RVALUE_REFERENCE_T
template <typename T>
using add_rvalue_reference_t = std::add_rvalue_reference_t<T>;
#else
template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
#endif

template <typename T1, typename T2>
using is_same_decayed = is_same<decay_t<T1>, decay_t<T2>>;

template <typename T1, typename T2>
using is_same_decayed_t = typename is_same_decayed<T1, T2>::type;

template <const bool... Values>
using bool_and =
    is_same<bool_sequence<Values...>, bool_sequence<(Values || true)...>>;

template <const bool... Values>
using bool_or = bool_constant<!bool_and<!Values...>::value>;

template <typename... T>
struct are_same;

template <typename T1, typename... T2>
struct are_same<T1, T1, T2...> : are_same<T1, T2...> {};

template <typename T1, typename T2, typename... T3>
struct are_same<T1, T2, T3...> : false_type {};

template <typename T>
struct are_same<T> : true_type {};

template <typename T1, typename... T2>
using are_same_decayed = are_same<decay_t<T1>, decay_t<T2>...>;

#ifdef _WN_HAS_CPP14_STD_IS_NULL_POINTER
using std::is_null_pointer;
#else
template <typename T>
using is_null_pointer = is_same_decayed<decltype(nullptr), T>;
#endif

#ifdef _WN_HAS_CPP14_STD_COMMON_TYPE_T
template <typename... T>
using common_type_t = std::common_type_t<T...>;
#else
template <typename... T>
using common_type_t = typename common_type<T...>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_RESULT_OF_T
template <typename T>
using result_of_t = std::result_of_t<T>;
#else
template <typename T>
using result_of_t = typename result_of<T>::type;
#endif

template <typename T>
struct exists : true_type {};

namespace internal {

template <typename T>
struct is_extended_type : false_type {};

template <typename _TraitsType>
struct is_extended_type<arithmetic_type<_TraitsType>> : true_type {};

template <typename T, typename = enable_if_t<true>>
struct is_floating_point : std::is_floating_point<T> {};

template <typename T>
struct is_floating_point<
    T, typename enable_if<
           exists<typename T::traits_type::conversion_type>::value>::type>
    : true_type {};

template <typename T, typename = enable_if_t<true>>
struct is_fixed_point : false_type {};

template <typename T>
struct is_fixed_point<T, typename enable_if<exists<
                             typename T::traits_type::scale_type>::value>::type>
    : true_type {};

template <typename T, typename = enable_if_t<true>>
struct is_signed : std::is_signed<T> {};

template <typename T>
struct is_signed<T, typename enable_if<is_floating_point<T>::value>::type>
    : true_type {};

template <typename T>
struct is_signed<T, typename enable_if<bool_and<is_extended_type<T>::value,
                        !is_floating_point<T>::value>::value>::type>
    : std::is_signed<typename T::value_type> {};

template <typename T, typename = enable_if_t<true>>
struct is_unsigned
    : bool_or<std::is_unsigned<T>::value, is_floating_point<T>::value> {};

template <typename T>
struct is_unsigned<T, typename enable_if<is_extended_type<T>::value>::type>
    : std::is_unsigned<typename T::value_type> {};

template <typename T>
struct is_arithmetic : std::is_arithmetic<T> {};

template <typename traits_type>
struct is_arithmetic<arithmetic_type<traits_type>> : true_type {};

template <typename F, typename R, typename... Args>
class is_callable {
private:
  template <typename T, const bool Test, typename U, typename... A>
  struct same_return : false_type {};

  template <typename T, typename U, typename... A>
  struct same_return<T, true, U, A...> : is_same<result_of_t<T(A...)>, U> {};

  using invalid = wn_char(&)[1];
  using valid = wn_char(&)[2];

  template <typename T>
  struct helper;

  template <typename T>
  static valid checker(
      helper<decltype(std::declval<T>()(std::declval<Args>()...))>*);

  template <typename T>
  static invalid checker(...);

  template <typename T>
  struct callable : bool_constant<sizeof(checker<T>(0)) == sizeof(valid)> {};

public:
  enum : bool { value = same_return<F, callable<F>::value, R, Args...>::value };
};

}  // namespace internal

template <typename T>
struct is_extended_type : internal::is_extended_type<decay_t<T>> {};

template <typename T>
struct is_arithmetic : internal::is_arithmetic<decay_t<T>> {};

template <typename T>
struct is_signed : internal::is_signed<decay_t<T>> {};

template <typename T>
struct is_unsigned : internal::is_unsigned<decay_t<T>> {};

template <typename T>
struct is_floating_point : internal::is_floating_point<decay_t<T>> {};

template <typename T>
struct is_fixed_point : internal::is_fixed_point<decay_t<T>> {};

template <typename T>
struct is_real
    : bool_or<is_fixed_point<T>::value, is_floating_point<T>::value> {};

template <typename F, typename T>
struct is_callable : false_type {};

template <typename F, typename R, typename... Args>
struct is_callable<F, R(Args...)> : internal::is_callable<F, R, Args...> {};

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_TYPE_TRAITS_H__
