// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_VALIDATOR_H__
#define __WN_SCRIPTING_TYPE_VALIDATOR_H__

#include <algorithm>

#include "WNCore/inc/WNBase.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {

enum class cast_direction { left, right, invalid, none };

enum cast_type {
  down = 1 << 0,
  up = 1 << 1,
};

struct allowed_builtin_operations {
  bool m_arithmetic[static_cast<wn_uint32>(arithmetic_type::max)];
  bool m_assignment[static_cast<wn_uint32>(assign_type::max)];
  bool m_unary[static_cast<wn_uint32>(unary_type::max)];
  bool m_post_unary[static_cast<wn_uint32>(post_unary_type::max)];
  bool m_short_circuit[static_cast<wn_uint32>(short_circuit_type::max)];
};

static_assert(
    static_cast<wn_size_t>(type_classification::max) == 8,
    "The number of classifications has changed, please update these tables");

const allowed_builtin_operations valid_builtin_operations[7]{
    // clang-format off
    // empty
    {//+     -      *      /      %      ==     !=     <=     >=     <      >
     {false, false, false, false, false, false, false, false, false, false, false},
     //=    +=    -=      *=     /=     %=     <==
     {true, false, false, false, false, false, false},
     // ++x --x   -x
     {false, false, false},
     // x++  x--
     {false, false},
     // &&   ||
     {false, false}},
    // void
    {//+     -      *      /      %      ==     !=     <=     >=     <      >
     {false, false, false, false, false, false, false, false, false, false, false},
     //=    +=    -=      *=     /=     %=     <==
     {true, false, false, false, false, false, false},
     // ++x --x   -x
     {false, false, false},
     // x++  x--
     {false, false},
     // &&   ||
     {false, false}},
    // int
    {//+    -     *     /     %     ==    !=    <=    >=    <     >
     {true, true, true, true, true, true, true, true, true, true, true},
     //=    +=    -=    *=    /=   %=     <==
     {true, true, true, true, true, true, false},
     // ++x --x   -x
     {true, true, true},
     // x++  x--
     {true, true},
     // &&   ||
     {false, false}},
    // float
    {//+    -     *     /     %     ==    !=    <=    >=    <     >
     {true, true, true, true, true, true, true, true, true, true, true},
     //=    +=    -=    *=    /=   %=     <==
     {true, true, true, true, true, true, false},
     // ++x  --x    -x
     {false, false, false},
     // x++  x--
     {false, false},
     // &&   ||
     {false, false}},
    // char
    {//+    -     *     /     %     ==    !=    <=    >=    <     >
     {true, true, true, true, true, true, true, true, true, true, true},
     //=    +=    -=    *=    /=   %=     <==
     {true, true, true, true, true, true, false},
     // ++x --x   -x
     {true, true, true},
     // x++  x--
     {true, true},
     // &&   ||
     {false, false}},
    // string
    {//+     -      *      /      %      ==     !=     <=     >=     <      >
     {false, false, false, false, false, false, false, false, false, false, false},
     //=    +=    -=    *=    /=   %=     <==
     {true, false, false, false, false, false, true},
     // ++x --x   -x
     {false, false, false},
     // x++  x--
     {false, false},
     // &&   ||
     {false, false}},
    // bool
    {//+     -      *      /      %      ==    !=    <=     >=     <      >
     {false, false, false, false, false, true, true, false, false, false, false},
     //=    +=     -=     *=     /=     %=     <==
     {true, false, false, false, false, false, false},
     // ++x --x   -x
     {false, false, false},
     // x++  x--
     {false, false},
     // &&   ||
     {true, true}}
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

struct type_operations {
  explicit type_operations(memory::allocator* _allocator)
      : m_casts(_allocator),
        m_ids(_allocator),
        m_functions(_allocator),
        m_allocator(_allocator) {
    m_ops = valid_builtin_operations[0];  // default to nothing valid
  }
  type_operations(const allowed_builtin_operations& operation,
                  memory::allocator* _allocator)
      : m_casts(_allocator),
        m_ids(_allocator),
        m_functions(_allocator),
        m_allocator(_allocator) {
    m_ops = operation;
  }
  wn_uint32 get_cast_type(wn_uint32 _val) {
    auto it = std::find_if(
        m_casts.begin(), m_casts.end(),
        [_val](cast_operation& cast) { return _val == cast.cast_to; });
    return (it == m_casts.end()) ? 0 : it->type;
  }
  wn_uint32 get_member_id(containers::string_view _member) {
    auto it =
        std::find_if(m_ids.begin(), m_ids.end(),
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
  allowed_builtin_operations m_ops;
  containers::dynamic_array<cast_operation> m_casts;
  containers::dynamic_array<member_id> m_ids;
  containers::dynamic_array<member_function> m_functions;
  memory::allocator* m_allocator;
};

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
    m_types.push_back(type_operations(m_allocator));
    m_types.push_back(
        type_operations(valid_builtin_operations[1], m_allocator));
    m_types.push_back(
        type_operations(valid_builtin_operations[2], m_allocator));
    m_types.push_back(
        type_operations(valid_builtin_operations[3], m_allocator));
    m_types.push_back(
        type_operations(valid_builtin_operations[4], m_allocator));
    m_types.push_back(
        type_operations(valid_builtin_operations[5], m_allocator));

    // Int casts up to float, down to char, bool
    m_types[1].m_casts.push_back({2, up});
    m_types[1].m_casts.push_back({3, down});
    m_types[1].m_casts.push_back({5, down});

    // Float casts down to everything
    m_types[2].m_casts.push_back({1, down});
    m_types[2].m_casts.push_back({3, down});
    m_types[2].m_casts.push_back({5, down});

    // Char casts up to float and int, down to bool
    m_types[3].m_casts.push_back({1, up});
    m_types[3].m_casts.push_back({2, up});
    m_types[3].m_casts.push_back({5, down});

    // Bool casts up to int, bool, and char
    m_types[5].m_casts.push_back({1, up});
    m_types[5].m_casts.push_back({2, up});
    m_types[5].m_casts.push_back({3, up});
  }

  wn_uint32 get_or_register_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name);
    if (it != m_mapping.end()) {
      return it->second;
    }
    auto str_it = m_names.insert(m_names.end(), _name.to_string());
    wn_uint32 type = m_max_types++;
    m_mapping.insert(std::make_pair(*str_it, type));
    m_types.push_back(type_operations(m_allocator));
    return type;
  }

  wn_uint32 get_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name);
    if (it != m_mapping.end()) {
      return 0;
    }
    return it->second;
  }

  bool is_valid_type(wn_uint32 _type) {
    return _type != 0 && _type <= m_types.size();
  }

  void enable_cast(wn_uint32 _from_type, wn_uint32 _to_type, cast_type _type) {
    WN_DEBUG_ASSERT_DESC(
        _from_type >=
                static_cast<wn_uint32>(type_classification::custom_type) ||
            _to_type >=
                static_cast<wn_uint32>(type_classification::custom_type),
        "It is invalid to redefine a builtin cast type");
    WN_DEBUG_ASSERT_DESC(m_types[_from_type].get_cast_type(_to_type) == 0,
                         "This cast has already been defined");
    WN_DEBUG_ASSERT_DESC(_type != 0, "Cannot define a NON cast");
    m_types[_from_type].m_casts.push_back({_to_type, _type});
  }

  void add_id(wn_uint32 _type, containers::string_view _id,
              wn_uint32 _out_type) {
    WN_DEBUG_ASSERT_DESC(_type < m_types.size(),
                         "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(_out_type < m_types.size(),
                         "Trying to index non-existent out-type");

    m_types[_type].m_ids.push_back({_id.to_string(m_allocator), _out_type});
  }

  void add_method(wn_uint32 _type, containers::string_view _name,
                  wn_uint32 _return_type,
                  containers::contiguous_range<wn_uint32> _types) {
    WN_DEBUG_ASSERT_DESC(_type < m_types.size(),
                         "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(_return_type < m_types.size(),
                         "Trying to index non-existent return_type");
    WN_DEBUG_ASSERT_DESC(
        _types.end() == std::find_if(_types.begin(), _types.end(),
                                     [this](wn_uint32 type) {
                                       return type >= m_types.size();
                                     }),
        "One of the return types is out of bounds");

    m_types[_type].m_functions.push_back(
        {_name.to_string(m_allocator), _return_type,
         containers::dynamic_array<wn_uint32>(_types.begin(), _types.end(),
                                              m_allocator)});
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

 private:
  containers::hash_map<containers::string_view, wn_uint32> m_mapping;
  containers::deque<containers::string> m_names;  // list of all type names
  containers::deque<type_operations> m_types;
  memory::allocator* m_allocator;
  wn_uint32 m_max_types;
};
}  // namespace scripting
}  // namesapce wn

#endif  //__WN_SCRIPTING_TYPE_VALIDATOR_H__
