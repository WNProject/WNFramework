// Copyright (c) 2016, WNProject Authors. All rights reserved.
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

enum cast_type {
  invalid,
  down,
  up,
  transparent,
  only_explicit,
  // This can only be explcitly cast.
  max
};

// Data for builtin operations
struct allowed_builtin_operations {
  uint32_t m_arithmetic[static_cast<uint32_t>(arithmetic_type::max)];
  uint32_t m_assignment[static_cast<uint32_t>(assign_type::max)];
  uint32_t m_unary[static_cast<uint32_t>(unary_type::max)];
  uint32_t m_post_unary[static_cast<uint32_t>(post_unary_type::max)];
  uint32_t m_short_circuit[static_cast<uint32_t>(short_circuit_type::max)];
};

static_assert(static_cast<size_t>(type_classification::max) == 13,
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
static const uint32_t SIZE_TYPE =
    static_cast<uint32_t>(type_classification::size_type);
static const uint32_t VOID_PTR_TYPE =
    static_cast<uint32_t>(type_classification::void_ptr_type);
static const uint32_t FUNCTION_PTR_TYPE =
    static_cast<uint32_t>(type_classification::function_ptr_type);

// Tables for all of the internal types.
const allowed_builtin_operations valid_builtin_operations[12]{
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
     // size_type this should not be used for any normal operations
     // TODO(awoloszyn): Expose this later to the user.
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&   ||
     {INVALID_TYPE, INVALID_TYPE}},
    // void_ptr_type: This should not be exposed to the user.
    //                This is just so we can generate certain casts.
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&   ||
     {INVALID_TYPE, INVALID_TYPE}},
     // function_pointer_type: This should not be exposed to the user.
    //                This is just so we can generate certain casts.
    {//+            -             *             /             %             ==         !=         <=            >=            <             >
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     //=         +=            -=            *=            /=            %=            <==
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // ++x         --x           -x
     {INVALID_TYPE, INVALID_TYPE, INVALID_TYPE},
     // x++         x--
     {INVALID_TYPE, INVALID_TYPE},
     // &&   ||
     {INVALID_TYPE, INVALID_TYPE}},
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
  uint32_t cast_to;
  cast_type type;
};

struct member_id {
  containers::string id;
  uint32_t type_id;
};

struct member_function {
  containers::string function_name;
  uint32_t type_id;
  containers::dynamic_array<uint32_t> parameter_ids;
};

// All of the data associated with a given type.
// Describes what operations are allowed on a type,
// what IDs are accessible through an instance of this type,
// what functions are callable through an instance of this type
// and what operations are allowable on this type.
struct type_definition {
  explicit type_definition(
      bool destructible_type, memory::allocator* _allocator)
    : m_casts(_allocator),
      m_ids(_allocator),
      m_functions(_allocator),
      m_allocator(_allocator),
      m_mode(reference_type::raw),
      m_mangling(_allocator),
      m_destructible_type(destructible_type) {
    m_ops = valid_builtin_operations[0];  // default to nothing valid
  }

  type_definition(const allowed_builtin_operations& operation,
      bool destructible_type, memory::allocator* _allocator)
    : m_casts(_allocator),
      m_ids(_allocator),
      m_functions(_allocator),
      m_allocator(_allocator),
      m_mode(reference_type::raw),
      m_mangling(_allocator),
      m_destructible_type(destructible_type) {
    m_ops = operation;
  }

  reference_type get_reference_type() const {
    return m_mode;
  }

  cast_type get_cast_type(uint32_t _val) {
    auto it = std::find_if(m_casts.begin(), m_casts.end(),
        [_val](cast_operation& cast) { return _val == cast.cast_to; });
    return (it == m_casts.end()) ? cast_type::invalid : it->type;
  }

  // Registers a subtype with the given name. Returns
  // false if the type already exists.
  bool register_sub_type(
      containers::string_view _member, uint32_t sub_type_type) {
    if (get_member_id(_member)) {
      return false;
    }
    m_ids.push_back(member_id({_member.to_string(m_allocator), sub_type_type}));
    return true;
  }

  uint32_t get_member_id(containers::string_view _member) const {
    auto it = std::find_if(m_ids.begin(), m_ids.end(),
        [_member](const member_id& id) { return _member == id.id; });
    return (it != m_ids.end()) ? it->type_id : 0;
  }

  uint32_t get_member_index(containers::string_view _member) const {
    auto it = std::find_if(m_ids.begin(), m_ids.end(),
        [_member](const member_id& id) { return _member == id.id; });
    return (it != m_ids.end()) ? static_cast<uint32_t>(it - m_ids.begin())
                               : static_cast<uint32_t>(-1);
  }

  uint32_t get_function_call(containers::string_view _function) {
    auto it = std::find_if(m_functions.begin(), m_functions.end(),
        [_function](member_function& funct) {
          return _function == funct.function_name;
        });
    return (it != m_functions.end()) ? it->type_id : 0;
  }

  uint32_t get_operation(arithmetic_type _op) const {
    return m_ops.m_arithmetic[static_cast<uint32_t>(_op)];
  }
  uint32_t get_operation(assign_type _op) const {
    return m_ops.m_assignment[static_cast<uint32_t>(_op)];
  }
  uint32_t get_operation(unary_type _op) const {
    return m_ops.m_unary[static_cast<uint32_t>(_op)];
  }
  uint32_t get_operation(post_unary_type _op) const {
    return m_ops.m_post_unary[static_cast<uint32_t>(_op)];
  }
  uint32_t get_operation(short_circuit_type _op) const {
    return m_ops.m_short_circuit[static_cast<uint32_t>(_op)];
  }

  allowed_builtin_operations m_ops;
  containers::dynamic_array<cast_operation> m_casts;
  containers::dynamic_array<member_id> m_ids;
  containers::dynamic_array<member_function> m_functions;
  containers::string m_mangling;
  reference_type m_mode;
  bool m_destructible_type;
  memory::allocator* m_allocator;
};

WN_INLINE void append_number(size_t number, containers::string& _str) {
  auto insert_pt = _str.end();

  if (number == 0) {
    _str.push_back('0');
    return;
  }

  while (number > 0) {
    _str.insert(insert_pt, '0' + number % 10);
    number -= number % 10;
    number /= 10;
    insert_pt = _str.end() - 1;
  }
}

// The main class for all type-related operations.
// Queryable for operations, methods, custom types,
// name mangling etc.
class type_validator {
  static memory::basic_allocator s_default_allocator;

public:
  type_validator(memory::allocator* _allocator = &s_default_allocator)
    : m_mapping(_allocator),
      m_names(_allocator),
      m_types(_allocator),
      m_allocator(_allocator),
      m_max_types(1) {
    m_names.emplace_back("", m_allocator);
    m_mapping.insert(core::make_pair("Void", m_max_types++));
    m_names.emplace_back("Void", m_allocator);

    m_mapping.insert(core::make_pair("Int", m_max_types++));
    m_names.emplace_back("Int", m_allocator);

    m_mapping.insert(core::make_pair("Float", m_max_types++));
    m_names.emplace_back("Float", m_allocator);

    m_mapping.insert(core::make_pair("Char", m_max_types++));
    m_names.emplace_back("Char", m_allocator);

    m_mapping.insert(core::make_pair("String", m_max_types++));
    m_names.emplace_back("String", m_allocator);

    m_mapping.insert(core::make_pair("Bool", m_max_types++));
    m_names.emplace_back("Bool", m_allocator);

    m_mapping.insert(core::make_pair("_Size", m_max_types++));
    m_names.emplace_back("_Size", m_allocator);

    m_mapping.insert(core::make_pair("_VoidPtr", m_max_types++));
    m_names.emplace_back("_VoidPtr", m_allocator);

    m_mapping.insert(core::make_pair("_FunctionPtr", m_max_types++));
    m_names.emplace_back("_FunctionPtr", m_allocator);

    // Need to add extras to m_names
    m_names.push_back(containers::string());
    m_names.push_back(containers::string());
    // Increment max_types by 2. This is because we keep
    // dummy types around for array and struct.
    m_max_types += 2;

    m_types.push_back(type_definition(false, m_allocator));
    for (size_t i = 1; i < 12; ++i) {
      m_types.push_back(
          type_definition(valid_builtin_operations[i], false, m_allocator));
    }

    m_types[1].m_mangling = "v";

    // Int casts up to float, down to char, bool
    m_types[INT_TYPE].m_casts.push_back({2, up});
    m_types[INT_TYPE].m_casts.push_back({3, down});
    m_types[INT_TYPE].m_casts.push_back({5, down});
    m_types[INT_TYPE].m_mangling = "l";

    // Float casts down to everything
    m_types[FLOAT_TYPE].m_casts.push_back({1, down});
    m_types[FLOAT_TYPE].m_casts.push_back({3, down});
    m_types[FLOAT_TYPE].m_casts.push_back({5, down});
    m_types[FLOAT_TYPE].m_mangling = "f";

    // Char casts up to float and int, down to bool
    m_types[CHAR_TYPE].m_casts.push_back({1, up});
    m_types[CHAR_TYPE].m_casts.push_back({2, up});
    m_types[CHAR_TYPE].m_casts.push_back({5, down});
    m_types[CHAR_TYPE].m_mangling = "c";

    // Bool casts up to int, bool, and char
    m_types[BOOL_TYPE].m_casts.push_back({1, up});
    m_types[BOOL_TYPE].m_casts.push_back({2, up});
    m_types[BOOL_TYPE].m_casts.push_back({3, up});
    m_types[BOOL_TYPE].m_mangling = "b";

    m_types[VOID_PTR_TYPE].m_mangling = "vp";

    m_types[SIZE_TYPE].m_mangling = "s";

    m_types[FUNCTION_PTR_TYPE].m_mangling = "fp";
  }

  // This will register 4 types, 1 for each reference mode.
  // The value returned will be that of the base type.
  // The reference_types::max values starting at the returned value,
  // represent the additional types.
  uint32_t register_struct_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name.to_string(m_allocator));
    if (it != m_mapping.end()) {
      return it->second;
    }

    const char* struct_modes[static_cast<uint32_t>(reference_type::max)] = {
        "", "R", "P", "RP", "RR", "PP"};

    uint32_t returned_type = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(reference_type::max); ++i) {
      auto str_it = m_names.insert(m_names.end(), _name.to_string(m_allocator));
      uint32_t type = m_max_types++;
      returned_type = (returned_type == 0) ? type : returned_type;
      containers::string name_string = _name.to_string(m_allocator);
      if (i == 0) {
        m_mapping.insert(
            core::make_pair(_name.to_string(m_allocator), type));
      }

      m_types.push_back(type_definition(
          (i != static_cast<uint32_t>(reference_type::unique)), m_allocator));
      // Sharable types (i.e. heap objects) are castable to/from void*.
      if (i == static_cast<uint32_t>(reference_type::shared)) {
        m_types.back().m_casts.push_back(
            {VOID_PTR_TYPE, cast_type::transparent});
        m_types[VOID_PTR_TYPE].m_casts.push_back(
            {type, cast_type::transparent});
      }

      type_definition& def = m_types.back();
      def.m_mode = static_cast<reference_type>(i);
      size_t total_length = 10 + 2 + 2 + _name.size();

      def.m_mangling = containers::string(m_allocator);
      def.m_mangling.reserve(total_length);
      def.m_mangling.push_back('N');
      def.m_mangling.append(struct_modes[i]);
      append_number(_name.size(), def.m_mangling);
      def.m_mangling.append(_name.data(), _name.size());
      def.m_mangling.push_back('E');
    }

    return returned_type;
  }

  uint32_t get_type(const containers::string_view& _name) {
    auto it = m_mapping.find(_name.to_string(m_allocator));
    if (it == m_mapping.end()) {
      return 0;
    }
    return it->second;
  }

  const containers::string_view get_type_name(uint32_t _type) const {
    return m_names[_type];
  }

  const type_definition& get_operations(uint32_t _type) const {
    return m_types[_type];
  }

  type_definition& get_operations(uint32_t _type) {
    return m_types[_type];
  }

  bool is_valid_type(uint32_t _type) {
    return _type != 0 && _type <= m_types.size();
  }

  void enable_cast(uint32_t _from_type, uint32_t _to_type, cast_type _type) {
    return m_types[_from_type].m_casts.push_back({_to_type, _type});
  }

  bool is_destructible_type(uint32_t _type) const {
    return m_types[_type].m_destructible_type;
  }

  cast_type get_cast(uint32_t _from_type, uint32_t _to_type) {
    if (_from_type == _to_type) {
      return cast_type::transparent;
    }
    return m_types[_from_type].get_cast_type(_to_type);
  }

  void add_id(uint32_t _type, containers::string_view _id, uint32_t _out_type) {
    WN_DEBUG_ASSERT_DESC(
        _type < m_types.size(), "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(
        _out_type < m_types.size(), "Trying to index non-existent out-type");

    m_types[_type].m_ids.push_back({_id.to_string(m_allocator), _out_type});
  }

  void add_method(uint32_t _type, containers::string_view _name,
      uint32_t _return_type, containers::contiguous_range<uint32_t> _types) {
    WN_DEBUG_ASSERT_DESC(
        _type < m_types.size(), "Trying to index non-existent type");
    WN_DEBUG_ASSERT_DESC(_return_type < m_types.size(),
        "Trying to index non-existent return_type");
    WN_DEBUG_ASSERT_DESC(
        _types.end() ==
            std::find_if(_types.begin(), _types.end(),
                [this](uint32_t type) { return type >= m_types.size(); }),
        "One of the return types is out of bounds");

    m_types[_type].m_functions.push_back({_name.to_string(m_allocator),
        _return_type, containers::dynamic_array<uint32_t>(
                          _types.begin(), _types.end(), m_allocator)});
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

    return core::move(value);
  }

private:
  containers::hash_map<containers::string, uint32_t> m_mapping;
  containers::deque<containers::string> m_names;  // list of all type names
  containers::deque<type_definition> m_types;
  memory::allocator* m_allocator;
  uint32_t m_max_types;
};
}  // namespace scripting
}  // namesapce wn

#endif  //__WN_SCRIPTING_TYPE_VALIDATOR_H__
