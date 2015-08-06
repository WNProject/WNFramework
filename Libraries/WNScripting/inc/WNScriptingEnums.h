// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_ENUMS_H__
#define __WN_SCRIPTING_ENUMS_H__

namespace wn {
namespace scripting {

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

static const char *assign_type_names[static_cast<size_t>(assign_type::max)] = {
    "=", "+=", "-=", "*=", "/=", "%=", "<=="};

enum class short_circuit_type { and_operator, or_operator, max };

static const char *short_circuit_type_names[static_cast<size_t>(
    short_circuit_type::max)] = {"And", "Or"};

enum class equality_type {
  equal,
  not_equal,
  less_than,
  greater_than,
  less_than_or_equal_to,
  greater_than_or_equal_to,
  max
};

// The only place these are used is to define constants that are handled in a
// special way
// special insofar as the language recognizes them
enum class type_classification {
  int_type,
  float_type,
  char_type,
  string_type,
  bool_type,
  custom_type,
  max
};

static const char *
    type_classification_names[static_cast<size_t>(type_classification::max)] = {
        "Int", "Float", "Char", "Char", "Bool", "Custom"};

static const wn_size_t type_array_depth[static_cast<size_t>(
    type_classification::max)] = {0, 0, 0, 1, 0, 0};

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

static const char
    *arithmetic_type_names[static_cast<size_t>(arithmetic_type::max)] = {
        "+", "-", "*", "/", "%", "==", "!=", "<=", ">=", "<", ">"};

enum class unary_type { pre_increment, pre_decrement, negation, max };

static const char *unary_type_names[static_cast<size_t>(unary_type::max)] = {
    "++", "--", "-"};

enum class post_unary_type { post_increment, post_decrement, max };

static const char *
    post_un_names[static_cast<size_t>(post_unary_type::max)] = {"++", "--"};
}
}

#endif //__WN_SCRIPTING_ENUMS_H__
