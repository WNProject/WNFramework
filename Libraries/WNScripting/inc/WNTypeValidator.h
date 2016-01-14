// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_VALIDATOR_H__
#define __WN_SCRIPTING_TYPE_VALIDATOR_H__

#include <algorithm>

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNBase.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {

enum class cast_direction { left, right, invalid, none };

enum cast_type {
  down = 1 << 0,
  up = 1 << 1,
};

// Data for builtin operations
struct allowed_builtin_operations {
  uint32_t m_arithmetic[static_cast<wn_uint32>(arithmetic_type::max)];
  uint32_t m_assignment[static_cast<wn_uint32>(assign_type::max)];
  uint32_t m_unary[static_cast<wn_uint32>(unary_type::max)];
  uint32_t m_post_unary[static_cast<wn_uint32>(post_unary_type::max)];
  uint32_t m_short_circuit[static_cast<wn_uint32>(short_circuit_type::max)];
};

static_assert(static_cast<wn_size_t>(type_classification::max) == 10,
    "The number of classifications has changed, please update these tables");
static const uint32_t INVALID_TYPE =
    static_cast<uint32_t>(type_classification::invalid_type);
static const uint32_t VOID_TYPE =
    static_cast<uint32_t>(type_classification::void_type);
static const uint32_t INT_TYPE =
    static_cast<uint32_t>(type_classification::int_type);
static const uint32_t FLOAT_TYPE =
    static_cast<uint32_t>(type_classification::float_type);
static const uint32_t BOOL_TYPE =
    static_cast<uint32_t>(type_classification::bool_type);
static const uint32_t CHAR_TYPE =
    static_cast<uint32_t>(type_classification::char_type);

// Tables for all of the internal types.
const allowed_builtin_operations valid_builtin_operations[9]{
    // clang-format off
    // empty
    {//+            -             *             /             %             ==            !=            <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=            +=            -=            *=            /=            %=            <==
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // void
    {//+            -             *             /             %             ==            !=            <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {VOID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // int
    {//+        -         *         /         %         ==         !=         <=         >=         <          >
     {INT_TYPE, INT_TYPE, INT_TYPE, INT_TYPE, INT_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE},
     //=         +=         -=         *=         /=         %=         <==
     {VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, INVALID_TYPE},
     // ++x     --x       -x
     {INT_TYPE, INT_TYPE, INT_TYPE},
     // x++     x--
     {INT_TYPE, INT_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // float
    {//+          -           *           /           %           ==         !=         <=        >=          <          >
     {FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE},
     //=         +=         -=         *=         /=         %=         <==
     {VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, FLOAT_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // char
    {//+         -          *          /          %          ==         !=         <=         >=         <          >
     {CHAR_TYPE, CHAR_TYPE, CHAR_TYPE, CHAR_TYPE, CHAR_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE},
     //=         +=         -=         *=         /=         %=         <==
     {VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, VOID_TYPE, INVALID_TYPE},
     // ++x      --x        -x
     {CHAR_TYPE, CHAR_TYPE, CHAR_TYPE},
     // x++      x--
     {CHAR_TYPE, CHAR_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // string
    {//+            -             *             /             %             ==         !=         <=         >=         <          >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE, BOOL_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {VOID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // bool
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, BOOL_TYPE, BOOL_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {VOID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&   ||
     {BOOL_TYPE, BOOL_TYPE}},
    // array
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, BOOL_TYPE, BOOL_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {VOID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}},
    // struct
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, BOOL_TYPE, BOOL_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {VOID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&          ||
     {INVALID_TYPE, INVALID_TYPE}}
    // clang-format on
};

struct cast_operation {
  wn_uint32 cast_to;
  cast_type type;
};

struct member_id {
  containers::string id;
  wn_uint32 type_id;
};

struct member_function {
  containers::string function_name;
  wn_uint32 type_id;
  wn::containers::dynamic_array<wn_uint32> parameter_ids;
};

// All of the data associated with a given type.
// Describes what operations are allowed on a type,
// what IDs are accessible through an instance of this type,
// what functions are callable through an instance of this type
// and what operations are allowable on this type.
struct type_definition {
  explicit type_definition(memory::allocator* _allocator)
    : m_casts(_allocator),
      m_ids(_allocator),
      m_functions(_allocator),
      m_allocator(_allocator),
      m_mangling(_allocator) {
    m_ops = valid_builtin_operations[0];  // default to nothing valid
  }
  type_definition(const allowed_builtin_operations& operation,
      memory::allocator* _allocator)
    : m_casts(_allocator),
      m_ids(_allocator),
      m_functions(_allocator),
      m_allocator(_allocator),
      m_mangling(_allocator) {
    m_ops = operation;
  }
  wn_uint32 get_cast_type(wn_uint32 _val) {
    auto it = std::find_if(m_casts.begin(), m_casts.end(),
        [_val](cast_operation& cast) { return _val == cast.cast_to; });
    return (it == m_casts.end()) ? 0 : it->type;
  }
  wn_uint32 get_member_id(containers::string_view _member) {
    auto it = std::find_if(m_ids.begin(), m_ids.end(),
        [_member](member_id& id) { return _member == id.id; });
    return (it != m_ids.end()) ? it->type_id : 0;
  }
  wn_uint32 get_function_call(containers::string_view _function) {
    auto it = std::find_if(m_functions.begin(), m_functions.end(),
        [_function](member_function& funct) {
          return _function == funct.function_name;
        });
    return (it != m_functions.end()) ? it->type_id : 0;
  }

  uint32_t get_operation(arithmetic_type _op) const {
    return m_ops.m_arithmetic[static_cast<wn_uint32>(_op)];
  }
  uint32_t get_operation(assign_type _op) const {
    return m_ops.m_assignment[static_cast<wn_uint32>(_op)];
  }
  uint32_t get_operation(unary_type _op) const {
    return m_ops.m_unary[static_cast<wn_uint32>(_op)];
  }
  uint32_t get_operation(post_unary_type _op) const {
    return m_ops.m_post_unary[static_cast<wn_uint32>(_op)];
  }
  uint32_t get_operation(short_circuit_type _op) const {
    return m_ops.m_short_circuit[static_cast<wn_uint32>(_op)];
  }

  allowed_builtin_operations m_ops;
  containers::dynamic_array<cast_operation> m_casts;
  containers::dynamic_array<member_id> m_ids;
  containers::dynamic_array<member_function> m_functions;
  containers::string m_mangling;
  memory::allocator* m_allocator;
};

// The main class for all type-related operations.
// Queryable for operations, methods, custom types,
// name mangling etc.
class type_validator {
  static memory::default_allocator s_default_allocator;

public:
  type_validator(memory::allocator* _allocator = &s_default_allocator)
    : m_mapping(_allocator),
      m_names(_allocator),
      m_types(_allocator),
      m_allocator(_allocator),
      m_max_types(1) {
    m_mapping.insert(std::make_pair("Void", m_max_types++));
    m_mapping.insert(std::make_pair("Int", m_max_types++));
    m_mapping.insert(std::make_pair("Float", m_max_types++));
    m_mapping.insert(std::make_pair("Char", m_max_types++));
    m_mapping.insert(std::make_pair("String", m_max_types++));
    m_mapping.insert(std::make_pair("Bool", m_max_types++));
    m_types.push_back(type_definition(m_allocator));
    for (size_t i = 1; i < 9; ++i) {
      m_types.push_back(
          type_definition(valid_builtin_operations[i], m_allocator));
    }

    m_types[1].m_mangling = "v";

    // Int casts up to float, down to char, bool
    m_types[2].m_casts.push_back({2, up});
    m_types[2].m_casts.push_back({3, down});
    m_types[2].m_casts.push_back({5, down});
    m_types[2].m_mangling = "l";

    // Float casts down to everything
    m_types[3].m_casts.push_back({1, down});
    m_types[3].m_casts.push_back({3, down});
    m_types[3].m_casts.push_back({5, down});
    m_types[3].m_mangling = "f";

    // Char casts up to float and int, down to bool
    m_types[4].m_casts.push_back({1, up});
    m_types[4].m_casts.push_back({2, up});
    m_types[4].m_casts.push_back({5, down});
    m_types[4].m_mangling = "c";

    // Bool casts up to int, bool, and char
    m_types[6].m_casts.push_back({1, up});
    m_types[6].m_casts.push_back({2, up});
    m_types[6].m_casts.push_back({3, up});
    m_types[6].m_mangling = "b";
  }

  wn_uint32 get_or_register_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name);
    if (it != m_mapping.end()) {
      return it->second;
    }
    auto str_it = m_names.insert(m_names.end(), _name.to_string());
    wn_uint32 type = m_max_types++;
    m_mapping.insert(std::make_pair(*str_it, type));
    m_types.push_back(type_definition(m_allocator));
    return type;
  }

  wn_uint32 get_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name);
    if (it != m_mapping.end()) {
      return 0;
    }
    return it->second;
  }

  const type_definition& get_operations(wn_uint32 _type) const {
    return m_types[_type];
  }

  bool is_valid_type(wn_uint32 _type) {
    return _type != 0 && _type <= m_types.size();
  }

  void enable_cast(wn_uint32 _from_type, wn_uint32 _to_type, cast_type _type) {
    WN_DEBUG_ASSERT_DESC(_from_type >= static_cast<wn_uint32>(
                                           type_classification::custom_type) ||
                             _to_type >= static_cast<wn_uint32>(
                                             type_classification::custom_type),
        "It is invalid to redefine a builtin cast type");
    WN_DEBUG_ASSERT_DESC(m_types[_from_type].get_cast_type(_to_type) == 0,
        "This cast has already been defined");
    WN_DEBUG_ASSERT_DESC(_type != 0, "Cannot define a NON cast");
    m_types[_from_type].m_casts.push_back({_to_type, _type});
  }

  void add_id(
      wn_uint32 _type, containers::string_view _id, wn_uint32 _out_type) {
    WN_DEBUG_ASSERT_DESC(
        _type < m_types.size(), "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(
        _out_type < m_types.size(), "Trying to index non-existent out-type");

    m_types[_type].m_ids.push_back({_id.to_string(m_allocator), _out_type});
  }

  void add_method(wn_uint32 _type, containers::string_view _name,
      wn_uint32 _return_type, containers::contiguous_range<wn_uint32> _types) {
    WN_DEBUG_ASSERT_DESC(
        _type < m_types.size(), "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(_return_type < m_types.size(),
        "Trying to index non-existent return_type");
    WN_DEBUG_ASSERT_DESC(
        _types.end() ==
            std::find_if(_types.begin(), _types.end(),
                [this](wn_uint32 type) { return type >= m_types.size(); }),
        "One of the return types is out of bounds");

    m_types[_type].m_functions.push_back({_name.to_string(m_allocator),
        _return_type, containers::dynamic_array<wn_uint32>(_types.begin(),
                                              _types.end(), m_allocator)});
  }

  bool is_cast_possible(wn_uint32 _from_type, wn_uint32 _to_type) {
    WN_DEBUG_ASSERT_DESC(
        _from_type < m_types.size() && _to_type < m_types.size(),
        "Undefined type name");
    return m_types[_from_type].get_cast_type(_to_type) != 0;
  }

  cast_direction get_cast_direction(wn_uint32 _from_type, wn_uint32 _to_type) {
    if (_from_type >=
            static_cast<wn_uint32>(type_classification::custom_type) &&
        _to_type >= static_cast<wn_uint32>(type_classification::custom_type)) {
      return cast_direction::invalid;
    }

    wn_uint32 a_to_b = m_types[_from_type].get_cast_type(_to_type);
    wn_uint32 b_to_a = m_types[_to_type].get_cast_type(_from_type);

    WN_DEBUG_ASSERT_DESC(a_to_b == 0 || a_to_b != b_to_a,
        "Cannot determine in which direction to cast");
    if (a_to_b || b_to_a) {
      return a_to_b > b_to_a ? cast_direction::left : cast_direction::right;
    }
    return cast_direction::invalid;
  }

  containers::string get_mangled_name(const containers::string_view& name,
      const uint32_t& return_type,
      const containers::contiguous_range<uint32_t>& parameters) const {
    containers::string value(m_allocator);
    value += "_Z3wns";
    auto insert_pt = value.end();
    size_t name_length = name.size();

    while (name_length > 0) {
      value.insert(insert_pt, '0' + name_length % 10);
      name_length -= name_length % 10;
      name_length /= 10;
      insert_pt = value.end() - 1;
    }

    value.insert(value.end(), name.begin(), name.end());
    value += "E";
    value += m_types[return_type].m_mangling;
    for (size_t i = 0; i < parameters.size(); ++i) {
      value += (m_types[parameters[i]].m_mangling);
    }

    return std::move(value);
  }

private:
  containers::hash_map<containers::string_view, wn_uint32> m_mapping;
  containers::deque<containers::string> m_names;  // list of all type names
  containers::deque<type_definition> m_types;
  memory::allocator* m_allocator;
  wn_uint32 m_max_types;
};
}  // namespace scripting
}  // namesapce wn

#endif  //__WN_SCRIPTING_TYPE_VALIDATOR_H__
