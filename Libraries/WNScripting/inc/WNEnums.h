// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ENUMS_H__
#define __WN_SCRIPTING_ENUMS_H__

namespace wn {
namespace scripting {

enum class reference_type { raw, self, unique, weak, shared, nullable, max };

// All possible asisgnment operators in WNScript.
enum class assign_type {
  equal,
  add_equal,
  sub_equal,
  mult_equal,
  div_equal,
  mod_equal,
  change_owner,
  max
};

// The textual representation of the assignment operators.
static const char* assign_type_names[static_cast<size_t>(assign_type::max)] = {
    "=", "+=", "-=", "*=", "/=", "%=", "<=="};

// All possible short_circuit operators in WNScript.
enum class short_circuit_type { and_operator, or_operator, max };

// The textual representation of all short_circuit operators.
static const char* short_circuit_type_names[static_cast<size_t>(
    short_circuit_type::max)] = {"&&", "||"};

// A classification of all types.
// Note this will have to be expanded to more than just
// an enum in the future to handle named types.
enum class type_classification {
  invalid_type,
  void_type,
  int_type,
  float_type,
  char_type,
  string_type,
  bool_type,
  size_type,
  void_ptr_type,
  function_ptr_type,
  array_type,
  struct_type,
  custom_type,
  max
};

// Textual representation of classifications.
static const char*
    type_classification_names[static_cast<size_t>(type_classification::max)] = {
        "Int", "Float", "Char", "Char", "Bool", "Custom"};

// How deep each of the types is in in terms of array indexing.
static const size_t type_array_depth[static_cast<size_t>(
    type_classification::max)] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0};

// All possible binary arithmetic operations in WNScript.
enum class arithmetic_type {
  arithmetic_add,
  arithmetic_sub,
  arithmetic_mult,
  arithmetic_div,
  arithmetic_mod,
  arithmetic_equal,
  arithmetic_not_equal,
  arithmetic_less_than_or_equal,
  arithmetic_greater_than_or_equal,
  arithmetic_less_than,
  arithmetic_greater_than,
  max
};

// The textual representation of all possible binary arithmetic
// operations in WNScript.
static const char*
    arithmetic_type_names[static_cast<size_t>(arithmetic_type::max)] = {
        "+", "-", "*", "/", "%", "==", "!=", "<=", ">=", "<", ">"};

// All possible pre-unary operations in WNScript.
enum class unary_type { pre_increment, pre_decrement, negation, max };

// The textual representation of all possible pre-unary operations in WNScript.
static const char* unary_type_names[static_cast<size_t>(unary_type::max)] = {
    "++", "--", "-"};

// All possible post-unary operations in WNScript.
enum class post_unary_type { post_increment, post_decrement, max };

// The textual representation of all possible post-unary operations in
// WNScript.
static const char* post_un_names[static_cast<size_t>(post_unary_type::max)] = {
    "++", "--"};
}
}

#endif  //__WN_SCRIPTING_ENUMS_H__
