// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "WNCore/inc/Internal/WNExtendedTypes.h"

#include <type_traits>

#ifndef _WN_HAS_CPP11_STD_ALIGNED_UNION
#include <algorithm>
#endif

namespace wn {
namespace core {

// c++11 traits ///////////////////////////////////////////////////////////////

// type categories

using std::is_void;
using std::is_array;
using std::is_pointer;
using std::is_enum;
using std::is_union;
using std::is_class;
using std::is_function;
using std::is_object;
using std::is_scalar;
using std::is_compound;
using std::is_integral;
// using std::is_floating_point;
using std::is_fundamental;
// using std::is_arithmetic;
using std::is_reference;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_member_pointer;
using std::is_member_object_pointer;
using std::is_member_function_pointer;

// type properties

using std::is_const;
using std::is_volatile;
using std::is_pod;
using std::is_empty;
using std::is_polymorphic;
using std::is_abstract;
using std::is_trivial;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_COPYABLE
using std::is_trivially_copyable;
#else
template <typename T>
struct is_trivially_copyable
    : std::integral_constant<bool,
          is_scalar<typename std::remove_all_extents<T>::type>::value> {};
#endif

using std::is_standard_layout;
using std::is_literal_type;
// using std::is_signed;
// using std::is_unsigned;

// type constants

using std::integral_constant;
using std::true_type;
using std::false_type;

// supported operations

using std::is_constructible;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_CONSTRUCTIBLE
using std::is_trivially_constructible;
#else
template <typename T, typename... Args>
struct is_trivially_constructible : false_type {};

template <typename T>
struct is_trivially_constructible<T>
    : integral_constant<bool, is_scalar<T>::value> {};

template <typename T>
struct is_trivially_constructible<T, T&&>
    : integral_constant<bool, is_scalar<T>::value> {};

template <typename T>
struct is_trivially_constructible<T, const T&>
    : integral_constant<bool, is_scalar<T>::value> {};

template <typename T>
struct is_trivially_constructible<T, T&>
    : integral_constant<bool, is_scalar<T>::value> {};
#endif

using std::is_nothrow_constructible;
using std::is_default_constructible;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE
using std::is_trivially_default_constructible;
#else
template <typename T>
struct is_trivially_default_constructible : is_trivially_constructible<T> {};
#endif

using std::is_nothrow_default_constructible;
using std::is_copy_constructible;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_COPY_CONSTRUCTIBLE
using std::is_trivially_copy_constructible;
#else
template <typename T>
struct is_trivially_copy_constructible
    : is_trivially_constructible<T,
          typename std::add_lvalue_reference<
              typename std::add_const<T>::type>::type> {};
#endif

using std::is_nothrow_copy_constructible;
using std::is_move_constructible;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_MOVE_CONSTRUCTIBLE
using std::is_trivially_move_constructible;
#else
template <typename T>
struct is_trivially_move_constructible
    : is_trivially_constructible<T,
          typename std::add_rvalue_reference<T>::type> {};
#endif

using std::is_nothrow_move_constructible;
using std::is_assignable;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_ASSIGNABLE
using std::is_trivially_assignable;
#else
template <typename T, typename Arg>
struct is_trivially_assignable : false_type {};

template <typename T>
struct is_trivially_assignable<T&, T>
    : integral_constant<bool, is_scalar<T>::value> {};

template <typename T>
struct is_trivially_assignable<T&, T&>
    : integral_constant<bool, is_scalar<T>::value> {};

template <typename T>
struct is_trivially_assignable<T&, const T&>
    : integral_constant<bool, is_scalar<T>::value> {};
#endif

using std::is_nothrow_assignable;
using std::is_copy_assignable;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_COPY_ASSIGNABLE
using std::is_trivially_copy_assignable;
#else
template <typename T>
struct is_trivially_copy_assignable
    : is_trivially_assignable<typename std::add_lvalue_reference<T>::type,
          typename std::add_lvalue_reference<const T>::type> {};
#endif

using std::is_nothrow_copy_assignable;
using std::is_move_assignable;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_MOVE_ASSIGNABLE
using std::is_trivially_move_assignable;
#else
template <typename T>
struct is_trivially_move_assignable
    : is_trivially_assignable<typename std::add_lvalue_reference<T>::type,
          typename std::add_rvalue_reference<T>::type> {};
#endif

using std::is_nothrow_move_assignable;
using std::is_destructible;

#ifdef _WN_HAS_CPP11_STD_IS_TRIVIALLY_DESTRUCTIBLE
using std::is_trivially_destructible;
#else
namespace internal {

template <typename T>
struct trivial_destructor
    : integral_constant<bool, is_scalar<T>::value || is_reference<T>::value> {};

}  // namespace internal

template <typename T>
struct is_trivially_destructible
    : internal::trivial_destructor<typename std::remove_all_extents<T>::type> {
};
#endif

using std::is_nothrow_destructible;
using std::has_virtual_destructor;

// relationships and property queries

using std::is_same;
using std::is_base_of;
using std::is_convertible;
using std::alignment_of;
using std::rank;
using std::extent;

// type modifications

using std::remove_cv;
using std::remove_const;
using std::remove_volatile;
using std::add_cv;
using std::add_const;
using std::add_volatile;
using std::make_signed;
using std::make_unsigned;
using std::remove_reference;
using std::add_lvalue_reference;
using std::add_rvalue_reference;
using std::remove_pointer;
using std::add_pointer;
using std::remove_extent;
using std::remove_all_extents;

// type transformations

using std::aligned_storage;

#ifdef _WN_HAS_CPP11_STD_ALIGNED_UNION
using std::aligned_union;
#else
namespace internal {

template <typename... Ts>
struct largest_alignment;

template <typename T, typename... Ts>
struct largest_alignment<T, Ts...> {
  using type = T;

  static const size_t value =
      alignment_of<typename largest_alignment<Ts...>::type>::value >
              alignment_of<T>::value
          ? alignment_of<typename largest_alignment<Ts...>::type>::value
          : alignment_of<T>::value;
};

template <typename T>
struct largest_alignment<T> {
  using type = T;

  static const std::size_t value = alignment_of<T>::value;
};

template <typename... Ts>
using largest_alignment_t = typename largest_alignment<Ts...>::type;

}  // namespace internal

template <std::size_t Length, typename... Ts>
struct aligned_union {
  static const size_t alignment_value =
      internal::largest_alignment<Ts...>::value;

  using type = typename aligned_storage<
      (Length > sizeof(typename internal::largest_alignment_t<Ts...>)
              ? Length
              : sizeof(typename internal::largest_alignment_t<Ts...>)),
      alignment_value>::type;
};
#endif

using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
using std::underlying_type;
using std::result_of;

// c++14 //////////////////////////////////////////////////////////////////////

// type properties

#ifdef _WN_HAS_CPP14_STD_IS_NULL_POINTER
using std::is_null_pointer;
#else
template <typename T>
struct is_null_pointer : is_same<decltype(nullptr), typename decay<T>::type> {};
#endif

// type sequences

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

template <const size_t... Values>
using index_sequence = integral_sequence<size_t, Values...>;
#endif

namespace internal {

template <const bool Negative, const bool Zero, typename IntegerConstant,
    typename IntegerSequence>
struct make_sequence {
  static_assert(
      !Negative, "make_integer_sequence<T, N> requires N to be non-negative.");
};

template <typename T, const T... Values>
struct make_sequence<false, true, integral_constant<T, 0>,
    integral_sequence<T, Values...>> : integral_sequence<T, Values...> {};

template <typename T, const T Index, const T... Values>
struct make_sequence<false, false, integral_constant<T, Index>,
    integral_sequence<T, Values...>>
    : make_sequence<false, Index == 1, integral_constant<T, Index - 1>,
          integral_sequence<T, Index - 1, Values...>> {};

}  // namespace internal

template <typename T, const T N>
using make_integral_sequence = typename internal::make_sequence<(N < 0), N == 0,
    integral_constant<T, N>, integral_sequence<T>>::type;

template <const size_t N>
using make_index_sequence = make_integral_sequence<size_t, N>;

// type modifications

#ifdef _WN_HAS_CPP14_STD_REMOVE_CONST_T
template <typename T>
using remove_const_t = std::remove_const_t<T>;
#else
template <typename T>
using remove_const_t = typename remove_const<T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_ADD_CONST_T
template <typename T>
using add_const_t = std::add_const_t<T>;
#else
template <typename T>
using add_const_t = typename add_const<T>::type;
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

#ifdef _WN_HAS_CPP14_STD_REMOVE_ALL_EXTENTS_T
template <typename T>
using remove_all_extents_t = std::remove_all_extents_t<T>;
#else
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;
#endif

// type transformations

#ifdef _WN_HAS_CPP14_STD_DECAY_T
template <typename T>
using decay_t = std::decay_t<T>;
#else
template <typename T>
using decay_t = typename decay<T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_ENABLE_IF_T
template <const bool Test, typename T = void>
using enable_if_t = std::enable_if_t<Test, T>;
#else
template <const bool Test, typename T = void>
using enable_if_t = typename enable_if<Test, T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_CONDITIONAL_T
template <const bool Test, typename T1, typename T2>
using conditional_t = std::conditional_t<Test, T1, T2>;
#else
template <const bool Test, typename T1, typename T2>
using conditional_t = typename conditional<Test, T1, T2>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_COMMON_TYPE_T
template <typename... Ts>
using common_type_t = std::common_type_t<Ts...>;
#else
template <typename... Ts>
using common_type_t = typename common_type<Ts...>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_UNDERLYING_TYPE_T
template <typename T>
using underlying_type_t = std::underlying_type_t<T>;
#else
template <typename T>
using underlying_type_t = typename underlying_type<T>::type;
#endif

#ifdef _WN_HAS_CPP14_STD_RESULT_OF_T
template <typename T>
using result_of_t = std::result_of_t<T>;
#else
template <typename T>
using result_of_t = typename result_of<T>::type;
#endif

// c++17 //////////////////////////////////////////////////////////////////////

// type constants

#ifdef _WN_HAS_CPP17_STD_BOOL_CONSTANT
template <const bool Value>
using bool_constant = std::bool_constant<Value>;
#else
template <const bool Value>
using bool_constant = integral_constant<bool, Value>;
#endif

// meta functions

#ifdef _WN_HAS_CPP17_STD_CONJUNCTION
using std::conjunction;
#else
template <typename... Ts>
struct conjunction : true_type {};

template <typename T>
struct conjunction<T> : T {};

template <typename T, typename... Ts>
struct conjunction<T, Ts...>
    : conditional_t<T::value != false, conjunction<Ts...>, T> {};
#endif

#ifdef _WN_HAS_CPP17_STD_DISJUNCTION
using std::disjunction;
#else
template <typename... Ts>
struct disjunction : false_type {};

template <typename T>
struct disjunction<T> : T {};

template <typename T, typename... Ts>
struct disjunction<T, Ts...>
    : conditional_t<T::value != false, T, disjunction<Ts...>> {};
#endif

#ifdef _WN_HAS_CPP17_STD_NEGATION
using std::negation;
#else
template <typename T>
struct negation : bool_constant<!T::value> {};
#endif

///////////////////////////////////////////////////////////////////////////////

// type properties

template <typename... Ts>
struct are_pod : conjunction<is_pod<Ts>...> {};

// type constants

template <const size_t Value>
using index_constant = integral_constant<size_t, Value>;

// type sequences

template <const bool... Values>
using bool_sequence = integral_sequence<bool, Values...>;

// meta functions

template <const bool... Values>
struct bool_and
    : is_same<bool_sequence<Values...>, bool_sequence<(Values || true)...>> {};

template <const bool... Values>
struct bool_or : integral_constant<bool, !bool_and<!Values...>::value> {};

// relationships and property queries

template <typename T1, typename T2>
struct is_same_decayed : is_same<decay_t<T1>, decay_t<T2>> {};

template <typename T, typename... Ts>
struct are_same : conjunction<is_same<T, Ts>...> {};

template <typename T, typename... Ts>
struct are_same_decayed : are_same<decay_t<T>, decay_t<Ts>...> {};

///////////////////////////////////////////////////////////////////////////////

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
struct is_floating_point<T,
    typename enable_if<
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

  using invalid = char (&)[1];
  using valid = char (&)[2];

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
  enum { value = same_return<F, callable<F>::value, R, Args...>::value };
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

template <typename T>
struct remove_lvalue_reference {
  typedef T type;
};
template <typename T>
struct remove_lvalue_reference<T&> {
  typedef T type;
};
template <typename T>
struct remove_lvalue_reference<T&&> {
  typedef T&& type;
};

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_TYPE_TRAITS_H__