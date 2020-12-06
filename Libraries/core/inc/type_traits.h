// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "core/inc/types.h"

#include <type_traits>

namespace wn {
namespace core {

// standard ////////////////////////////////////////////////////////////////////

// type categories

using std::is_arithmetic;
using std::is_array;
using std::is_class;
using std::is_compound;
using std::is_enum;
using std::is_floating_point;
using std::is_function;
using std::is_fundamental;
using std::is_integral;
using std::is_lvalue_reference;
using std::is_member_function_pointer;
using std::is_member_object_pointer;
using std::is_member_pointer;
using std::is_null_pointer;
using std::is_object;
using std::is_pointer;
using std::is_reference;
using std::is_rvalue_reference;
using std::is_scalar;
using std::is_union;
using std::is_void;

// type properties

using std::is_abstract;
using std::is_const;
using std::is_empty;
using std::is_literal_type;
using std::is_pod;
using std::is_polymorphic;
using std::is_signed;
using std::is_standard_layout;
using std::is_trivial;
using std::is_trivially_copyable;
using std::is_unsigned;
using std::is_volatile;

// type constants

using std::bool_constant;
using std::false_type;
using std::integral_constant;
using std::true_type;

// supported operations

using std::has_virtual_destructor;
using std::is_assignable;
using std::is_constructible;
using std::is_copy_assignable;
using std::is_copy_constructible;
using std::is_default_constructible;
using std::is_destructible;
using std::is_move_assignable;
using std::is_move_constructible;
using std::is_nothrow_assignable;
using std::is_nothrow_constructible;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_destructible;
using std::is_nothrow_move_assignable;
using std::is_nothrow_move_constructible;
using std::is_trivially_assignable;
using std::is_trivially_constructible;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_default_constructible;
using std::is_trivially_destructible;
using std::is_trivially_move_assignable;
using std::is_trivially_move_constructible;

// relationships and property queries

using std::alignment_of;
using std::extent;
using std::is_base_of;
using std::is_convertible;
using std::is_invocable;
using std::is_invocable_r;
using std::is_same;
using std::rank;

// type modifications

using std::add_const;
using std::add_const_t;
using std::add_cv;
using std::add_lvalue_reference;
using std::add_lvalue_reference_t;
using std::add_pointer;
using std::add_pointer_t;
using std::add_rvalue_reference;
using std::add_rvalue_reference_t;
using std::add_volatile;
using std::make_signed;
using std::make_unsigned;
using std::remove_all_extents;
using std::remove_all_extents_t;
using std::remove_const;
using std::remove_const_t;
using std::remove_cv;
using std::remove_extent;
using std::remove_pointer;
using std::remove_reference;
using std::remove_volatile;

// type transformations

using std::aligned_storage;
using std::aligned_union;
using std::common_type;
using std::common_type_t;
using std::conditional;
using std::conditional_t;
using std::decay;
using std::decay_t;
using std::enable_if;
using std::enable_if_t;
using std::invoke_result;
using std::invoke_result_t;
using std::result_of;
using std::result_of_t;
using std::underlying_type;
using std::underlying_type_t;
using std::void_t;

// meta functions

using std::conjunction;
using std::disjunction;
using std::negation;

// custom //////////////////////////////////////////////////////////////////////

// type properties

template <typename... Ts>
struct are_pod : conjunction<is_pod<Ts>...> {};

// type constants

template <size_t Value>
using index_constant = integral_constant<size_t, Value>;

// relationships and property queries

template <typename T1, typename T2>
struct is_same_decayed : is_same<decay_t<T1>, decay_t<T2>> {};

template <typename T, typename... Ts>
struct are_same : conjunction<is_same<T, Ts>...> {};

template <typename T, typename... Ts>
struct are_same_decayed : are_same<decay_t<T>, decay_t<Ts>...> {};

// type transformations

template <typename T>
struct remove_lvalue_reference {
  using type = T;
};

template <typename T>
struct remove_lvalue_reference<T&> {
  using type = T;
};

template <typename T>
struct remove_lvalue_reference<T&&> {
  using type = T&&;
};

template <typename T>
using remove_lvalue_reference_t = typename remove_lvalue_reference<T>::type;

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_TYPE_TRAITS_H__
