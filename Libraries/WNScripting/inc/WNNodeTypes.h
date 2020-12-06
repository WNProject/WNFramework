// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_NODE_TYPES_H__
#define __WN_SCRIPTING_NODE_TYPES_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"
#include "WNScripting/inc/source_location.h"
#include "core/inc/pair.h"

namespace wn {
namespace scripting {

// clang-format off
enum class node_type {
  arg_list,
  array_type,
  runtime_array_type,
  slice_type,
  parameter_list,
  parameter,
  expression,
    replaced_expression,
    array_allocation_expression,
    binary_expression,
    cast_expression,
    cond_expression,
    constant_expression,
    id_expression,
    resource_expression,
    null_allocation_expression,
    post_expression,
      array_access_expression,
      slice_expression,
      function_call_expression,
      member_access_expression,
      post_unary_expression,
    sizeof_expression,
    alignof_expression,
    short_circuit_expression,
    struct_allocation_expression,
    unary_expression,
    builtin_unary_expression,
  function,
  instruction,
    assignment_instruction,
    break_instruction,
    continue_instruction,
    declaration,
    do_instruction,
    else_if_instruction,
    expression_instruction,
    for_instruction,
    if_instruction,
    return_instruction,
    while_instruction,
  instruction_list,
  lvalue,
  script_file,
  struct_definition,
  top_level_resource,
  type
};
// clang-format on
class declaration;
class expression;
class function;
class instruction;
class node;
class parameter;
class paramter_list;
class constant_expression;

template <typename T>
const T* cast_to(const node* _node) {
  return static_cast<const T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
}

template <typename T>
memory::unique_ptr<T> clone_node(memory::allocator* _allocator, const T* val) {
  if (!val) {
    return nullptr;
  }
  memory::unique_ptr<node> n = val->clone(_allocator);
  // FUNCTION CALLS DO NOT GUARANTEE ARGUMENT ORDERING.
  // the allocator may have already been destroyed if you call
  // n.get_allocator()         vvvvv    there
  return memory::unique_ptr<T>(_allocator, static_cast<T*>(n.release()));
}

inline bool decode_string(
    containers::string_view input, containers::string* _output) {
  input = input.substr(1, input.length() - 2);
  bool escaping = false;

  for (auto it = input.begin(); it != input.end(); ++it) {
    if (escaping) {
      switch (*it) {
        case '\\':
          *_output += '\\';
          break;
        case 'n':
          *_output += '\n';
          break;
        case 'r':
          *_output += '\r';
          break;
        case 't':
          *_output += '\t';
          break;
        case '"':
          *_output += '"';
          break;
        default:
          *_output = "Unknown escape: ";
          *_output += *it;
          return false;
      }
      escaping = false;
      continue;
    }
    if (*it == '\\') {
      escaping = true;
      continue;
    }
    *_output += *it;
  }
  return true;
}

struct print_context {
  print_context(memory::allocator* _allocator, logging::log* _log,
      logging::log_level _level, const containers::string_view _prefix)
    : m_prefix(_prefix.to_string(_allocator)),
      m_allocator(_allocator),
      m_level(_level),
      m_log(_log) {}
  using node_type = const node*;
  containers::string m_prefix;
  memory::allocator* m_allocator;
  logging::log_level m_level;
  logging::log* m_log;
  bool ignore_next_prefix = false;

  void enter_log_scope() {
    ignore_next_prefix = false;
    m_prefix.insert(0, "|   ");
  }
  void leave_log_scope() {
    m_prefix = m_prefix.substr(0, m_prefix.size() - 4);
    ignore_next_prefix = false;
  }
  void print_prefix() {
    if (ignore_next_prefix) {
      ignore_next_prefix = false;
    } else {
      m_log->log_params(m_level,
          static_cast<size_t>(logging::log_flags::no_newline), m_prefix);
    }
  }
  template <typename... Args>
  void print_line(const Args&... args) {
    print_prefix();
    m_log->log_params(
        m_level, static_cast<size_t>(logging::log_flags::no_header), args...);
  }

  template <typename... Args>
  void print_header(const Args&... args) {
    if (ignore_next_prefix) {
      m_log->log_params(
          m_level, static_cast<size_t>(logging::log_flags::no_header), args...);
    } else {
      m_log->log_params(m_level, 0, m_prefix, args...);
    }

    ignore_next_prefix = false;
  }
  template <typename... Args, typename V>
  void print_value(const V& value, const Args&... name) {
    print_line("|-", name..., " = ", value);
  }
  template <typename... Args>
  void print_value(const containers::string& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = \"\"");
    } else {
      print_line("|-", name..., " = ", value);
    }
  }
  template <typename... Args>
  void print_value(const node_type& value, const Args&... name);

  template <typename... Args, typename T>
  void print_value(const memory::unique_ptr<T>& value, const Args&... name) {
    return print_value(static_cast<const node*>(value.get()), name...);
  }

  template <typename... Args, typename T>
  void print_value(
      const containers::dynamic_array<T>& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = <EMPTY>");
    } else {
      print_line("|-", name..., " = ");
      enter_log_scope();
      for (size_t i = 0; i < value.size(); ++i) {
        print_value(value[i], "|-[", i, "]");
      }
      leave_log_scope();
    }
  }

  template <typename... Args, typename T>
  void print_value(const containers::deque<T>& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = <EMPTY>");
    } else {
      print_line("|-", name..., " = ");
      enter_log_scope();
      for (size_t i = 0; i < value.size(); ++i) {
        print_value(value[i], "|-{", i, "}");
      }
      leave_log_scope();
    }
  }
};
// Base class for all nodes in AST.
class node {
public:
  node(memory::allocator* _allocator, node_type _type)
    : m_allocator(_allocator), m_type(_type) {
#ifdef WN_ALLOW_TESTING_LOG
    __f = &node::print_node;
#endif
  }
  virtual ~node() {}

  void copy_location_from(const node* _node) {
    m_source_location = _node->m_source_location;
  }

  // Sets the start location of first character representing this node.
  void set_start_location(const source_location& _location) {
    m_source_location = _location;
  }

  // Modifies the source_location with the given source index of the last
  // character that represents this node.
  void set_end_index(int64_t _idx) {
    m_source_location.m_end_index = _idx;
  }

  // Returns the source location representing this node.
  const source_location& get_start_location() const {
    return (m_source_location);
  }

  // Writes the first line of this expression to the given log.
  void log_line(logging::log* _log, logging::log_level _level) const {
    size_t line_length = 1023;
    const char* c = memory::strnchr(
        reinterpret_cast<const char*>(m_source_location.m_line_start), '\n',
        1023);
    if (c == nullptr) {
      // If there is no \n then we simply print everything
      c = reinterpret_cast<const char*>(m_source_location.m_line_start) +
          memory::strnlen(
              reinterpret_cast<const char*>(m_source_location.m_line_start),
              1023);
    }
    line_length =
        (c - reinterpret_cast<const char*>(m_source_location.m_line_start));
    char* data_buffer = static_cast<char*>(WN_STACK_ALLOC(line_length + 1));
    memcpy(data_buffer, m_source_location.m_line_start, line_length);
    data_buffer[line_length] = '\0';
    _log->log_params(_level, 0, "Line: ", m_source_location.m_line_number, "\n",
        data_buffer);
  }

  node_type get_node_type() const {
    return m_type;
  }
  memory::allocator* get_allocator() const {
    return m_allocator;
  }

  void print_node(logging::log* _log, logging::log_level _level) const {
    print_context c(m_allocator, _log, _level, "");
    return print_node_internal(&c);
  }
  virtual void print_node_internal(print_context*) const {}

  virtual memory::unique_ptr<node> clone(memory::allocator*) const {
    WN_RELEASE_ASSERT(false, "Unimplemented clone");
    return nullptr;
  }

  void copy_underlying_from(memory::allocator*, const node* _other) {
    copy_location_from(_other);
    m_type = _other->m_type;
  }

protected:
  // Location of the first character of the first token contributing
  // to this node.
  source_location m_source_location;
  memory::allocator* m_allocator;
  node_type m_type;
#ifdef WN_ALLOW_TESTING_LOG
  // Force the print_node functions to not go away if
  // test logging is turned on.
  void (wn::scripting::node::*__f)(logging::log*, logging::log_level) const;
#endif
};

template <typename... Args>
void print_context::print_value(
    const print_context::node_type& value, const Args&... name) {
  if (!ignore_next_prefix) {
    m_log->log_params(m_level,
        static_cast<size_t>(logging::log_flags::no_newline), m_prefix, "|-",
        name..., " = [", value, "]");
  } else {
    m_log->log_params(m_level,
        static_cast<size_t>(logging::log_flags::no_newline) |
            static_cast<size_t>(logging::log_flags::no_header),
        "|-", name..., " = [", value, "]");
  }

  enter_log_scope();
  ignore_next_prefix = true;
  if (value) {
    value->print_node_internal(this);
  } else {
    print_line("<NULL>");
  }
  leave_log_scope();
}

class top_level_resource : public node {
public:
  top_level_resource(
      memory::allocator* _allocator, const char* _type, const char* _str)
    : node(_allocator, node_type::top_level_resource),
      m_resource_type(_allocator, _type),
      m_string(_allocator, _str) {}

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<top_level_resource>(
        _allocator, _allocator, m_resource_type.c_str(), m_string.c_str());
    t->copy_underlying_from(_allocator, this);
    return t;
  }

  containers::string_view get_type() const {
    return m_resource_type;
  }
  containers::string_view get_string() const {
    return m_string;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Resource Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_resource_type, "ResourceType");
    c->print_value(m_string, "String");
  }

private:
  containers::string m_resource_type;
  containers::string m_string;
};

class type : public node {
public:
  type(memory::allocator* _allocator, const char* _custom_type)
    : node(_allocator, node_type::type),
      m_type(0),
      m_reference_type(reference_type::raw),
      m_custom_type(_allocator, _custom_type) {}

  explicit type(memory::allocator* _allocator)
    : node(_allocator, node_type::type) {}

  type(memory::allocator* _allocator, containers::string_view _custom_type)
    : node(_allocator, node_type::type),
      m_type(0),
      m_reference_type(reference_type::raw),
      m_custom_type(_custom_type.to_string(_allocator)) {}

  type(memory::allocator* _allocator, uint32_t _type)
    : node(_allocator, node_type::type),
      m_type(_type),
      m_reference_type(reference_type::raw) {}

  type(memory::allocator* _allocator, uint32_t _type, reference_type _ref)
    : node(_allocator, node_type::type),
      m_type(_type),
      m_reference_type(_ref) {}

  type(memory::allocator* _allocator, type_classification _type,
      node_type _node_type = node_type::type,
      reference_type _ref = reference_type::raw)
    : node(_allocator, _node_type),
      m_type(static_cast<uint32_t>(_type)),
      m_reference_type(_ref),
      m_custom_type(_allocator) {}

  type(const type& _other) = delete;

  void set_reference_type(reference_type _reference_type) {
    // If we adjust the reference type, then we have to
    // adjust the type id as well.
    if (m_type != static_cast<uint32_t>(type_classification::array_type) &&
        m_type !=
            static_cast<uint32_t>(type_classification::runtime_array_type)) {
      m_type += (static_cast<int32_t>(_reference_type) -
                 static_cast<int32_t>(m_reference_type));
    }
    m_reference_type = _reference_type;
  }

  containers::string_view custom_type_name() const {
    return m_custom_type;
  }

  reference_type get_reference_type() const {
    return m_reference_type;
  }

  virtual const type* get_subtype() const {
    return nullptr;
  }

  virtual type* get_subtype() {
    return nullptr;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Type");
    c->print_value(m_custom_type, "Custom Name");
    c->print_value(m_type, "Numeric Type");
    c->print_value(m_reference_type, "Reference Type");
  }

  void set_custom_type_data(const containers::string_view& v) {
    m_custom_type_data = v.to_string(m_allocator);
  }

  containers::string_view get_custom_type_data() const {
    return m_custom_type_data;
  }

  void set_type_name(const containers::string_view& v) {
    m_custom_type = v.to_string(m_allocator);
  }

  uint32_t get_classification() const {
    return m_type;
  }

  virtual const expression* get_size() const {
    return nullptr;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<type>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    return t;
  }

  void copy_underlying_from(memory::allocator* _allocator, const type* _other) {
    node::copy_location_from(_other);
    m_type = _other->m_type;
    m_reference_type = _other->m_reference_type;
    m_custom_type = containers::string(_allocator, _other->m_custom_type);
    m_custom_type_data =
        containers::string(_allocator, _other->m_custom_type_data);
  }

protected:
  uint32_t m_type;
  reference_type m_reference_type;
  containers::string m_custom_type;
  containers::string m_custom_type_data;
};

class array_type : public type {
public:
  array_type(memory::allocator* _allocator, type* _sub_type)
    : type(_allocator, type_classification::array_type, node_type::array_type),
      m_subtype(memory::unique_ptr<type>(_allocator, _sub_type)) {}

  explicit array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type, node_type::array_type) {
  }

  array_type(memory::allocator* _allocator, node_type _node_type)
    : type(_allocator, type_classification::array_type, _node_type) {}

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array");
    c->print_value(m_type, "Type Value");
    c->print_value(m_subtype, "SubType");
  }

  void set_constant(expression* _const) {
    m_size = memory::unique_ptr<expression>(m_allocator, _const);
  }

  const expression* get_size() const override {
    return m_size.get();
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<array_type>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_size = clone_node(_allocator, m_size.get());
    t->m_subtype = clone_node(_allocator, m_subtype.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_size;
  memory::unique_ptr<type> m_subtype;
};

class slice_type : public type {
public:
  slice_type(memory::allocator* _allocator, type* _sub_type)
    : type(_allocator, type_classification::slice_type, node_type::slice_type),
      m_subtype(memory::unique_ptr<type>(_allocator, _sub_type)),
      m_dimensions(1) {}

  explicit slice_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::slice_type, node_type::slice_type) {
  }

  slice_type(memory::allocator* _allocator, node_type _node_type)
    : type(_allocator, type_classification::slice_type, _node_type) {}

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Slice");
    c->print_value(m_type, "Type Value");
    c->print_value(m_subtype, "SubType");
    c->print_value(m_dimensions, "Dimensions");
  }

  void set_dimensions(const containers::string_view& _dim) {
    long long val = atoll(_dim.to_string(m_allocator).c_str());
    m_dimensions = static_cast<uint32_t>(val);
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<slice_type>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_dimensions = m_dimensions;
    t->m_subtype = clone_node(_allocator, m_subtype.get());
    return t;
  }

  uint32_t dimensions() const {
    return m_dimensions;
  }

private:
  memory::unique_ptr<type> m_subtype;
  uint32_t m_dimensions;
};

class runtime_array_type : public type {
public:
  runtime_array_type(memory::allocator* _allocator, type* _sub_type)
    : type(_allocator, type_classification::array_type,
          node_type::runtime_array_type),
      m_subtype(memory::unique_ptr<type>(_allocator, _sub_type)) {}

  explicit runtime_array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type,
          node_type::runtime_array_type) {}

  runtime_array_type(memory::allocator* _allocator, node_type _node_type)
    : type(_allocator, type_classification::array_type, _node_type) {}

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array");
    c->print_value(m_type, "Type Value");
    c->print_value(m_subtype, "SubType");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<runtime_array_type>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_subtype = clone_node(_allocator, m_subtype.get());
    return t;
  }

private:
  memory::unique_ptr<type> m_subtype;
};

class struct_allocation_expression;

// Base class for all expression nodes in the AST.
class expression : public node {
public:
  expression(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type), m_type(nullptr) {}

  expression(memory::allocator* _allocator, node_type _node_type, type* _type)
    : node(_allocator, _node_type),
      m_type(memory::unique_ptr<type>(_allocator, _type)) {}

  const type* get_type() const {
    return m_type.get();
  }

  void set_type(type* _type) {
    m_type = memory::unique_ptr<type>(m_allocator, _type);
  }

protected:
  void copy_underlying_from(
      memory::allocator* _allocator, const expression* _other) {
    node::copy_location_from(_other);
    m_type = clone_node(_allocator, _other->m_type.get());
  }

  memory::unique_ptr<type> m_type;
};

enum class struct_initialization_mode {
  invalid,
  unspecified,
  unique,
  strong,
  max
};

class arg_list;
class array_allocation_expression : public expression {
public:
  explicit array_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::array_allocation_expression),
      m_array_initializers(_allocator),
      m_init_mode(struct_initialization_mode::invalid),
      m_levels(0),
      m_is_runtime(false) {}

  const containers::deque<memory::unique_ptr<expression>>&
  get_array_initializers() const {
    return m_array_initializers;
  }

  struct_initialization_mode set_initialization_mode() const {
    return m_init_mode;
  }

  void set_initialization_mode(struct_initialization_mode _mode) {
    m_init_mode = _mode;
  }

  bool is_runtime() const {
    return m_is_runtime;
  }

  void set_runtime(bool _runtime) {
    m_is_runtime = _runtime;
  }

  void add_expression(expression* _expr) {
    m_array_initializers.emplace_back(
        memory::unique_ptr<expression>(m_allocator, _expr));
    m_levels++;
  }
  void add_level() {
    m_levels++;
  }
  void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::unique_ptr<expression>(m_allocator, _expression);
  }

  const expression* get_copy_initializer() const {
    return m_copy_initializer.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array Initializer");
    c->print_value(m_type, "Type");
    c->print_value(m_array_initializers, "Initializers");
    c->print_value(m_copy_initializer, "Copy Initializer");
    c->print_value(m_inline_initializers, "Inline Initializers");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<array_allocation_expression>(
        m_allocator, m_allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_array_initializers =
        containers::deque<memory::unique_ptr<expression>>(_allocator);
    for (auto& init : m_array_initializers) {
      t->m_array_initializers.push_back(clone_node(_allocator, init.get()));
    }
    t->m_copy_initializer = clone_node(_allocator, m_copy_initializer.get());
    t->m_inline_initializers =
        clone_node(_allocator, m_inline_initializers.get());
    t->m_constructor_name = containers::string(_allocator, m_constructor_name);
    t->m_destructor_name = containers::string(_allocator, m_constructor_name);
    t->m_init_mode = m_init_mode;
    t->m_levels = m_levels;
    t->m_is_runtime = m_is_runtime;
    return t;
  }

  void set_inline_initializers(arg_list* _list) {
    m_inline_initializers = memory::unique_ptr<arg_list>(m_allocator, _list);
  }

  const arg_list* get_inline_initializers() const {
    return m_inline_initializers.get();
  }

private:
  containers::deque<memory::unique_ptr<expression>> m_array_initializers;
  memory::unique_ptr<arg_list> m_inline_initializers;
  memory::unique_ptr<expression> m_copy_initializer;
  containers::string m_constructor_name;
  containers::string m_destructor_name;
  struct_initialization_mode m_init_mode;
  size_t m_levels;
  bool m_is_runtime;
};

class binary_expression : public expression {
public:
  binary_expression(memory::allocator* _allocator, arithmetic_type _type,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(_type),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  explicit binary_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(arithmetic_type::max) {}

  const expression* get_lhs() const {
    return m_lhs.get();
  }
  const expression* get_rhs() const {
    return m_rhs.get();
  }
  const arithmetic_type get_arithmetic_type() const {
    return m_arith_type;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Binary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_arith_type, "ArithType");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<binary_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_lhs = clone_node(_allocator, m_lhs.get());
    t->m_rhs = clone_node(_allocator, m_rhs.get());
    t->m_arith_type = m_arith_type;
    return t;
  }

private:
  arithmetic_type m_arith_type;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};

class cond_expression : public expression {
public:
  cond_expression(memory::allocator* _allocator, expression* _cond,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::cond_expression),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  explicit cond_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::cond_expression) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Conditional Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_condition, "Condition");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<cond_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_lhs = clone_node(_allocator, m_lhs.get());
    t->m_rhs = clone_node(_allocator, m_rhs.get());
    t->m_condition = clone_node(_allocator, m_condition.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};

class constant_expression : public expression {
public:
  constant_expression(memory::allocator* _allocator, type_classification _type,
      const char* _text)
    : expression(_allocator, node_type::constant_expression),
      m_type_classification(static_cast<uint32_t>(_type)),
      m_text(_allocator, _text) {}

  constant_expression(
      memory::allocator* _allocator, type* _type, const char* _text)
    : expression(_allocator, node_type::constant_expression, _type),
      m_type_classification(0),
      m_text(_allocator, _text) {
    m_type_classification = m_type->get_classification();
  }

  explicit constant_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::constant_expression) {}

  uint32_t get_index() const {
    return m_type_classification;
  }
  const containers::string& get_type_text() const {
    return m_text;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Constant");
    c->print_value(m_type, "Type");
    c->print_value(m_type_classification, "Classification");
    c->print_value(m_text, "Value");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<constant_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_type_classification = m_type_classification;
    t->m_text = containers::string(_allocator, m_text);
    return t;
  }

private:
  uint32_t m_type_classification;
  containers::string m_text;
};

class id_expression : public expression {
public:
  id_expression(memory::allocator* _allocator, const char* _name)
    : expression(_allocator, node_type::id_expression),
      m_name(_allocator, _name) {}

  id_expression(memory::allocator* _allocator, containers::string_view _name)
    : expression(_allocator, node_type::id_expression),
      m_name(_name.to_string(_allocator)) {}

  explicit id_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::id_expression) {}
  containers::string_view get_name() const {
    return m_name;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Id Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_name, "Value");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<id_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_name = containers::string(_allocator, m_name);
    return t;
  }

private:
  containers::string m_name;
};

class null_allocation_expression : public expression {
public:
  null_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::null_allocation_expression) {}
  void print_node_internal(print_context* c) const override {
    c->print_header("Null Allocation");
    c->print_value(m_type, "Type");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<null_allocation_expression>(
        m_allocator, m_allocator);
    t->copy_underlying_from(_allocator, this);
    return t;
  }

private:
};

class post_expression : public expression {
public:
  post_expression(memory::allocator* _allocator, node_type _type)
    : expression(_allocator, _type) {}
  void add_base_expression(expression* _expr) {
    m_base_expression = memory::unique_ptr<expression>(m_allocator, _expr);
  }
  void add_base_expression(memory::unique_ptr<expression> _expr) {
    m_base_expression = core::move(_expr);
  }
  const expression* get_base_expression() const {
    return m_base_expression.get();
  }

  void copy_underlying_from(
      memory::allocator* _allocator, const post_expression* _other) {
    expression::copy_location_from(_other);
    m_base_expression = clone_node(_allocator, _other->m_base_expression.get());
  }

protected:
  memory::unique_ptr<expression> m_base_expression;
};

class array_access_expression : public post_expression {
public:
  array_access_expression(memory::allocator* _allocator, expression* _expr)
    : post_expression(_allocator, node_type::array_access_expression),
      m_array_access(memory::unique_ptr<expression>(m_allocator, _expr)),
      m_is_construction(false) {}

  explicit array_access_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::array_access_expression) {}

  expression* get_access() const {
    return m_array_access.get();
  }

  bool is_construction() const {
    return m_is_construction;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array Access");
    c->print_value(m_type, "Type");
    c->print_value(m_base_expression, "Base Expression");
    c->print_value(m_array_access, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<array_access_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_array_access = clone_node(_allocator, m_array_access.get());
    t->m_is_construction = m_is_construction;
    return t;
  }

private:
  memory::unique_ptr<expression> m_array_access;
  bool m_is_construction;
};

class slice_expression : public post_expression {
public:
  slice_expression(
      memory::allocator* _allocator, expression* _index0, expression* _index1)
    : post_expression(_allocator, node_type::slice_expression),
      m_index_0(memory::unique_ptr<expression>(m_allocator, _index0)),
      m_index_1(memory::unique_ptr<expression>(m_allocator, _index1)) {}

  explicit slice_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::slice_expression) {}

  expression* get_index0() const {
    return m_index_0.get();
  }

  expression* get_index1() const {
    return m_index_1.get();
  }

  void set_index0(expression* _index) {
    m_index_0 = memory::unique_ptr<expression>(m_allocator, _index);
  }

  void set_index1(expression* _index) {
    m_index_1 = memory::unique_ptr<expression>(m_allocator, _index);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Slice");
    c->print_value(m_type, "Type");
    c->print_value(m_base_expression, "Base Expression");
    c->print_value(m_index_0, "Index0");
    c->print_value(m_index_1, "Index1");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<slice_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_index_0 = clone_node(_allocator, m_index_0.get());
    t->m_index_1 = clone_node(_allocator, m_index_1.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_index_0;
  memory::unique_ptr<expression> m_index_1;
};
class short_circuit_expression : public expression {
public:
  short_circuit_expression(memory::allocator* _allocator,
      short_circuit_type _type, expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::short_circuit_expression),
      m_ss_type(_type),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  explicit short_circuit_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::short_circuit_expression) {}

  const expression* get_lhs() const {
    return m_lhs.get();
  }

  const expression* get_rhs() const {
    return m_rhs.get();
  }

  short_circuit_type get_ss_type() const {
    return m_ss_type;
  }
  void print_node_internal(print_context* c) const override {
    c->print_header("Short Circuit");
    c->print_value(m_type, "Type");
    c->print_value(m_ss_type, "Short-Circuit Type");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<short_circuit_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_lhs = clone_node(_allocator, m_lhs.get());
    t->m_rhs = clone_node(_allocator, m_rhs.get());
    t->m_ss_type = m_ss_type;
    return t;
  }

private:
  short_circuit_type m_ss_type;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};

class member_access_expression : public post_expression {
public:
  member_access_expression(memory::allocator* _allocator, const char* _member)
    : post_expression(_allocator, node_type::member_access_expression),
      m_member(_allocator, _member) {}

  member_access_expression(
      memory::allocator* _allocator, containers::string_view _member)
    : post_expression(_allocator, node_type::member_access_expression),
      m_member(_member.to_string(_allocator)) {}

  member_access_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::member_access_expression) {}

  const containers::string_view get_name() const {
    return m_member;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Member Access");
    c->print_value(m_type, "Type");
    c->print_value(m_member, "Value");
    c->print_value(m_base_expression, "Base Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<member_access_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_member = containers::string(_allocator, m_member);
    return t;
  }

private:
  containers::string m_member;
};

class post_unary_expression : public post_expression {
public:
  post_unary_expression(memory::allocator* _allocator, post_unary_type _type)
    : post_expression(_allocator, node_type::post_unary_expression),
      m_unary_type(_type) {}
  explicit post_unary_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::post_unary_expression) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Post Unary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_unary_type, "Unary Type");
    c->print_value(m_base_expression, "Base Expression");
  }

  post_unary_type get_post_unary_type() const {
    return m_unary_type;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<post_unary_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_unary_type = m_unary_type;
    return t;
  }

private:
  post_unary_type m_unary_type;
};

class cast_expression : public expression {
public:
  cast_expression(memory::allocator* _allocator, expression* _expression)
    : expression(_allocator, node_type::cast_expression),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expression)) {}

  cast_expression(memory::allocator* _allocator,
      memory::unique_ptr<expression>&& _expression)
    : expression(_allocator, node_type::cast_expression),
      m_expression(core::move(_expression)) {}

  explicit cast_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::cast_expression) {}

  const expression* get_expression() const {
    return m_expression.get();
  }
  expression* get_expression() {
    return m_expression.get();
  }
  void set_expression(memory::unique_ptr<expression> expr) {
    m_expression = core::move(expr);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Cast");
    c->print_value(m_type, "Type");
    c->print_value(m_expression, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<cast_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_expression = clone_node(_allocator, m_expression.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_expression;
};

struct function_expression {
  function_expression(
      memory::allocator* _allocator, expression* _expr, bool _hand_ownership)
    : m_expr(_allocator, _expr), m_hand_ownership(_hand_ownership) {}
  function_expression(
      memory::unique_ptr<expression>&& _expr, bool _hand_ownership)
    : m_expr(core::move(_expr)), m_hand_ownership(_hand_ownership) {}

  explicit function_expression(memory::allocator*) {}

  memory::unique_ptr<function_expression> clone(
      memory::allocator* _allocator) const {
    auto t = memory::make_unique<function_expression>(_allocator, _allocator);
    t->m_expr = clone_node(_allocator, m_expr.get());
    t->m_hand_ownership = m_hand_ownership;
    return t;
  }

  memory::unique_ptr<expression> m_expr;
  bool m_hand_ownership;
};

class arg_list : public node {
public:
  arg_list(memory::allocator* _allocator)
    : node(_allocator, node_type::arg_list), m_expression_list(_allocator) {}
  void add_expression(expression* _expr, bool _hand_ownership = false) {
    m_expression_list.emplace_back(memory::make_unique<function_expression>(
        m_allocator, m_allocator, _expr, _hand_ownership));
  }

  containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() {
    return (m_expression_list);
  }

  const containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() const {
    return (m_expression_list);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Args");
    c->enter_log_scope();
    for (auto& e : m_expression_list) {
      e->m_expr->print_node_internal(c);
    }
    c->leave_log_scope();
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<arg_list>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_expression_list =
        containers::deque<memory::unique_ptr<function_expression>>(_allocator);
    for (auto& i : m_expression_list) {
      t->m_expression_list.push_back(i->clone(_allocator));
    }
    return t;
  }

private:
  containers::deque<memory::unique_ptr<function_expression>> m_expression_list;
};

class resource_expression : public expression {
public:
  resource_expression(
      memory::allocator* _allocator, const char* _type, const char* _str)
    : expression(_allocator, node_type::resource_expression),
      m_resource_type(_allocator, _type),
      m_string(_allocator, _str) {}

  resource_expression(memory::allocator* _allocator, const char* _type,
      const char* _str, arg_list* _list)
    : expression(_allocator, node_type::resource_expression),
      m_resource_type(_allocator, _type),
      m_string(_allocator, _str),
      m_args(_allocator, _list) {}

  explicit resource_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::resource_expression) {}
  containers::string_view get_type() const {
    return m_resource_type;
  }
  containers::string_view get_string() const {
    return m_string;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Resource Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_resource_type, "ResourceType");
    c->print_value(m_string, "String");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<resource_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_resource_type = containers::string(_allocator, m_resource_type);
    t->m_string = containers::string(_allocator, m_string);
    return t;
  }

  const containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() const {
    if (m_args) {
      return m_args->get_expressions();
    }
    static const containers::deque<memory::unique_ptr<function_expression>>
        empty_expressions;
    return empty_expressions;
  }

  containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() {
    if (!m_args) {
      m_args = memory::make_unique<arg_list>(m_allocator, m_allocator);
    }
    return m_args->get_expressions();
  }

  const arg_list* get_args() const {
    return m_args.get();
  }

private:
  containers::string m_resource_type;
  containers::string m_string;
  memory::unique_ptr<arg_list> m_args;
};

class struct_allocation_expression : public expression {
public:
  explicit struct_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::struct_allocation_expression),
      m_copy_initializer(nullptr),
      m_init_mode(struct_initialization_mode::invalid) {}

  void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::unique_ptr<expression>(m_allocator, _expression);
  }

  void set_initialization_mode(struct_initialization_mode _init) {
    m_init_mode = _init;
  }
  struct_initialization_mode set_initialization_mode() const {
    return m_init_mode;
  }

  void set_args(arg_list* _args) {
    m_args = memory::unique_ptr<arg_list>(m_allocator, _args);
  }

  const arg_list* get_args() const {
    return m_args.get();
  };

  void print_node_internal(print_context* c) const override {
    c->print_header("Struct Allocation");
    c->print_value(m_type, "Type");
    c->print_value(m_init_mode, "Init Mode");
    c->print_value(m_copy_initializer, "Copy Initializer");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<struct_allocation_expression>(
        _allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_copy_initializer = clone_node(_allocator, m_copy_initializer.get());
    t->m_init_mode = m_init_mode;
    t->m_args = clone_node(_allocator, m_args.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_copy_initializer;
  struct_initialization_mode m_init_mode;
  memory::unique_ptr<arg_list> m_args;
};

class unary_expression : public expression {
public:
  unary_expression(
      memory::allocator* _allocator, unary_type _type, expression* _expr)
    : expression(_allocator, node_type::unary_expression),
      m_unary_type(_type),
      m_root_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  explicit unary_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::unary_expression) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Unary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_unary_type, "Unary Type");
    c->print_value(m_root_expression, "Root Expression");
  }

  const expression* get_root_expression() const {
    return m_root_expression.get();
  }

  unary_type get_unary_type() const {
    return m_unary_type;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<unary_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_root_expression = clone_node(_allocator, m_root_expression.get());
    t->m_unary_type = m_unary_type;
    return t;
  }

private:
  unary_type m_unary_type;
  memory::unique_ptr<expression> m_root_expression;
};

class builtin_unary_expression : public expression {
public:
  builtin_unary_expression(memory::allocator* _allocator,
      builtin_unary_type _type, expression* _expr)
    : expression(_allocator, node_type::builtin_unary_expression),
      m_unary_type(_type),
      m_root_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  builtin_unary_expression(memory::allocator* _allocator,
      builtin_unary_type _type, memory::unique_ptr<expression> _expr)
    : expression(_allocator, node_type::builtin_unary_expression),
      m_unary_type(_type),
      m_root_expression(core::move(_expr)) {}

  builtin_unary_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::builtin_unary_expression) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Builtin Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_unary_type, "Builtin Type");
    c->print_value(m_root_expression, "Root Expression");
  }

  const expression* root_expression() const {
    return m_root_expression.get();
  }

  builtin_unary_type get_unary_type() const {
    return m_unary_type;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<builtin_unary_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_root_expression = clone_node(_allocator, m_root_expression.get());
    t->m_unary_type = m_unary_type;
    return t;
  }

private:
  builtin_unary_type m_unary_type;
  memory::unique_ptr<expression> m_root_expression;
};

class instruction_list;
class instruction : public node {
public:
  instruction(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type),
      m_breaks(false),
      m_returns(false),
      m_non_linear(false) {}

  instruction(memory::allocator* _allocator, node_type _type, bool _returns,
      bool _is_non_linear = false, bool _breaks = false)
    : instruction(_allocator, _type) {
    m_returns = _returns;
    m_non_linear = _is_non_linear;
    m_breaks = _breaks;
  }

protected:
  void copy_underlying_from(memory::allocator*, const instruction* _other) {
    node::copy_location_from(_other);
    m_breaks = _other->m_breaks;
    m_returns = _other->m_returns;
    m_non_linear = _other->m_non_linear;
  }

  bool m_breaks;
  bool m_returns;
  bool m_non_linear;
};

struct expression_instruction : public instruction {
  expression_instruction(memory::allocator* _allocator, expression* _expr)
    : instruction(_allocator, node_type::expression_instruction),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  expression_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::expression_instruction) {}

  const expression* get_expression() const {
    return m_expression.get();
  }

  expression* get_expression() {
    return m_expression.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Expression Instruction");
    c->print_value(m_expression, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<expression_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_expression = clone_node(_allocator, m_expression.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_expression;
};

class instruction_list : public instruction {
public:
  instruction_list(memory::allocator* _allocator)
    : instruction(_allocator, node_type::instruction_list),
      m_instructions(_allocator) {}
  instruction_list(memory::allocator* _allocator, instruction* inst)
    : instruction(_allocator, node_type::instruction_list),
      m_instructions(_allocator) {
    m_instructions.push_back(
        memory::unique_ptr<instruction>(m_allocator, inst));
  }
  ~instruction_list() {}
  void add_instruction(instruction* inst) {
    m_instructions.push_back(
        memory::unique_ptr<instruction>(m_allocator, inst));
  }

  void add_instruction(memory::unique_ptr<instruction>&& inst) {
    m_instructions.push_back(core::move(inst));
  }

  void prepend_instruction(memory::unique_ptr<instruction>&& inst) {
    m_instructions.push_front(core::move(inst));
  }

  const containers::deque<memory::unique_ptr<instruction>>& get_instructions()
      const {
    return m_instructions;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("InstructionList<", m_instructions.size(), ">");
    c->print_value(m_instructions, "Instructions");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<instruction_list>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_instructions =
        containers::deque<memory::unique_ptr<instruction>>(_allocator);
    for (auto& i : m_instructions) {
      t->m_instructions.push_back(clone_node(_allocator, i.get()));
    }
    return t;
  }

private:
  containers::deque<memory::unique_ptr<instruction>> m_instructions;
};

class function_call_expression : public post_expression {
public:
  function_call_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::function_call_expression) {}
  function_call_expression(memory::allocator* _allocator, arg_list* _list)
    : post_expression(_allocator, node_type::function_call_expression),
      m_args(_allocator, _list) {}

  function_call_expression(
      memory::allocator* _allocator, memory::unique_ptr<arg_list>&& _list)
    : post_expression(_allocator, node_type::function_call_expression),
      m_args(core::move(_list)) {}

  const containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() const {
    if (m_args) {
      return m_args->get_expressions();
    }
    static const containers::deque<memory::unique_ptr<function_expression>>
        empty_expressions;
    return empty_expressions;
  }

  containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() {
    if (!m_args) {
      m_args = memory::make_unique<arg_list>(m_allocator, m_allocator);
    }
    return m_args->get_expressions();
  }

  const arg_list* get_args() const {
    return m_args.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Function Call");
    c->print_value(m_type, "Type");
    c->print_value(m_base_expression, "BaseExpression");
    c->print_value(m_args, "Args");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<function_call_expression>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_args = clone_node(_allocator, m_args.get());
    return t;
  }

private:
  memory::unique_ptr<arg_list> m_args;
};

class parameter : public node {
public:
  parameter(memory::allocator* _allocator, type* _type, const char* _name)
    : node(_allocator, scripting::node_type::parameter),
      m_type(memory::unique_ptr<type>(_allocator, _type)),
      m_name(_allocator, _name),
      m_is_for_empty_function(false) {}

  explicit parameter(memory::allocator* _allocator)
    : node(_allocator, scripting::node_type::parameter) {}

  const containers::string_view get_name() const {
    return m_name.to_string_view();
  }

  void set_name(const containers::string_view& view) {
    m_name = view.to_string(m_allocator);
  }
  void set_type(memory::unique_ptr<type>&& _new_type) {
    m_type = core::move(_new_type);
  }

  const type* get_type() const {
    return m_type.get();
  }
  type* get_type() {
    return m_type.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header(m_name);
    c->print_value(m_type, "Type");
  }

  bool is_for_empty_function() const {
    return m_is_for_empty_function;
  }

  void set_is_for_empty_function(bool is_for_vtable) {
    m_is_for_empty_function = is_for_vtable;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<parameter>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_type = clone_node(_allocator, m_type.get());
    t->m_name = containers::string(_allocator, m_name);
    t->m_is_for_empty_function = m_is_for_empty_function;
    return t;
  }

private:
  friend class declaration;
  memory::unique_ptr<type> m_type;
  containers::string m_name;
  bool m_is_for_empty_function;
};

class declaration : public instruction {
public:
  declaration(memory::allocator* _allocator)
    : instruction(_allocator, node_type::declaration),
      m_is_default_initialization(false),
      m_is_inherited(false) {}
  void set_parameter(parameter* _parameter) {
    m_parameter = memory::unique_ptr<parameter>(m_allocator, _parameter);
  }

  void set_inherited_parameter(const containers::string_view& view) {
    m_is_inherited = true;
    m_parameter = memory::make_unique<parameter>(m_allocator, m_allocator);
    m_parameter->copy_location_from(this);
    m_parameter->set_name(view);
  }

  void set_default_initialization() {
    m_is_default_initialization = true;
  }

  bool is_default_initialized() const {
    return m_is_default_initialization;
  }

  bool is_inherited() const {
    return m_is_inherited;
  }

  void set_parameter(memory::unique_ptr<parameter>&& _parameter) {
    m_parameter = core::move(_parameter);
  }

  void add_expression_initializer(expression* _expr) {
    m_expression = memory::unique_ptr<expression>(m_allocator, _expr);
  }

  const expression* get_expression() const {
    return m_expression.get();
  }

  const type* get_type() const {
    return m_parameter->get_type();
  }

  const containers::string_view get_name() const {
    return m_parameter->get_name();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Declaration:");
    c->print_value(m_parameter, "Parameter");
    c->print_value(m_expression, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<declaration>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_parameter = clone_node(_allocator, m_parameter.get());
    t->m_expression = clone_node(_allocator, m_expression.get());
    t->m_is_default_initialization = m_is_default_initialization;
    t->m_is_inherited = m_is_inherited;
    return t;
  }

private:
  memory::unique_ptr<parameter> m_parameter;
  memory::unique_ptr<expression> m_expression;
  bool m_is_default_initialization;
  bool m_is_inherited;
};

class function;

class parameter_list : public node {
public:
  explicit parameter_list(memory::allocator* _allocator)
    : node(_allocator, node_type::parameter_list), m_parameters(_allocator) {}
  parameter_list(memory::allocator* _allocator, parameter* _param)
    : parameter_list(_allocator) {
    m_parameters.emplace_back(
        memory::unique_ptr<parameter>(m_allocator, _param));
  }

  void add_parameter(parameter* _param) {
    m_parameters.emplace_back(
        memory::unique_ptr<parameter>(m_allocator, _param));
  }

  void prepend_parameter(memory::unique_ptr<parameter>&& _param) {
    m_parameters.emplace_front(core::move(_param));
  }

  const containers::deque<memory::unique_ptr<parameter>>& get_parameters()
      const {
    return m_parameters;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Parameter List");
    c->print_value(m_parameters, "Parameters");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<parameter_list>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);

    t->m_parameters =
        containers::deque<memory::unique_ptr<parameter>>(_allocator);
    for (auto& m : m_parameters) {
      t->m_parameters.push_back(clone_node(_allocator, m.get()));
    }
    return t;
  }

private:
  containers::deque<memory::unique_ptr<parameter>> m_parameters;
};

class struct_definition : public node {
public:
  struct_definition(memory::allocator* _allocator, const char* _name,
      bool _is_class = false, const char* _parent_type = nullptr)
    : node(_allocator, node_type::struct_definition),
      m_name(_allocator, _name),
      m_parent_name(_allocator),
      m_is_class(_is_class),
      m_struct_members(_allocator),
      m_struct_functions(_allocator) {
    if (_parent_type) {
      m_parent_name = _parent_type;
    }
  }

  struct_definition(memory::allocator* _allocator)
    : node(_allocator, node_type::struct_definition),
      m_struct_members(_allocator),
      m_struct_functions(_allocator) {}

  void add_struct_elem(declaration* _decl) {
    m_struct_members.emplace_back(
        memory::unique_ptr<declaration>(m_allocator, _decl));
  }

  void add_function(function* _func) {
    m_struct_functions.emplace_back(
        memory::unique_ptr<function>(m_allocator, _func));
  }

  const containers::deque<memory::unique_ptr<function>>& get_functions() const {
    return m_struct_functions;
  }

  containers::string_view get_name() const {
    return m_name;
  }

  containers::string_view get_parent_name() const {
    return m_parent_name;
  }

  containers::deque<memory::unique_ptr<declaration>>& get_struct_members() {
    return m_struct_members;
  }

  void set_constructor_parameters(parameter_list* _params) {
    m_constructor_parameters =
        memory::unique_ptr<parameter_list>(m_allocator, _params);
  }

  const parameter_list* constructor_params() const {
    return m_constructor_parameters.get();
  }

  const containers::deque<memory::unique_ptr<declaration>>& get_struct_members()
      const {
    return m_struct_members;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Struct [", m_name, "]", ":", m_parent_name);
    c->print_value(m_struct_members, "Struct Members");
    c->print_value(m_struct_functions, "Struct functions");
  }

  bool is_class() const {
    return m_is_class;
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<struct_definition>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_name = containers::string(_allocator, m_name);
    t->m_parent_name = containers::string(_allocator, m_parent_name);
    t->m_is_class = m_is_class;
    t->m_struct_members =
        containers::deque<memory::unique_ptr<declaration>>(_allocator);
    t->m_struct_functions =
        containers::deque<memory::unique_ptr<function>>(_allocator);
    for (auto& m : m_struct_members) {
      t->m_struct_members.push_back(clone_node(_allocator, m.get()));
    }

    for (auto& m : m_struct_functions) {
      t->m_struct_functions.push_back(clone_node(_allocator, m.get()));
    }

    return t;
  }

private:
  containers::string m_name;
  containers::string m_parent_name;
  bool m_is_class;
  containers::deque<memory::unique_ptr<declaration>> m_struct_members;
  containers::deque<memory::unique_ptr<function>> m_struct_functions;
  memory::unique_ptr<parameter_list> m_constructor_parameters;
};

class function : public node {
public:
  function(memory::allocator* _allocator, parameter* _signature,
      parameter_list* _params, instruction_list* _body)
    : node(_allocator, node_type::function),
      m_signature(memory::unique_ptr<parameter>(m_allocator, _signature)),
      m_parameters(memory::unique_ptr<parameter_list>(m_allocator, _params)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)),
      m_is_override(false),
      m_is_virtual(false) {}

  explicit function(memory::allocator* _allocator)
    : node(_allocator, node_type::function) {}

  function(memory::allocator* _allocator,
      memory::unique_ptr<parameter>&& _signature,
      memory::unique_ptr<parameter_list>&& _params,
      memory::unique_ptr<instruction_list>&& _body)
    : node(_allocator, node_type::function),
      m_signature(core::move(_signature)),
      m_parameters(core::move(_params)),
      m_body(core::move(_body)),
      m_is_override(false),
      m_is_virtual(false) {}

  void set_is_virtual(bool is_virtual) {
    m_is_virtual = is_virtual;
  }

  bool is_virtual() const {
    return m_is_virtual;
  }

  bool is_override() const {
    return m_is_override;
  }

  void set_is_override(bool is_override) {
    m_is_override = is_override;
  }
  const parameter* get_signature() const {
    return m_signature.get();
  }
  parameter* get_signature() {
    return m_signature.get();
  }
  const parameter_list* get_parameters() const {
    return m_parameters.get();
  }
  parameter_list* get_parameters() {
    return m_parameters.get();
  }

  parameter_list* get_initialized_parameters() {
    if (!m_parameters) {
      m_parameters =
          memory::make_unique<parameter_list>(m_allocator, m_allocator);
    }
    return m_parameters.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Function");
    c->print_value("<NULL>", "This");
    c->print_value(m_signature, "Signature");
    c->print_value(m_parameters, "Parameters");
    c->print_value(m_body, "Body");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<function>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_signature = clone_node(_allocator, m_signature.get());
    t->m_parameters = clone_node(_allocator, m_parameters.get());
    t->m_body = clone_node(_allocator, m_body.get());
    t->m_is_override = m_is_override;
    t->m_is_virtual = m_is_virtual;
    return t;
  }

private:
  memory::unique_ptr<parameter> m_signature;
  memory::unique_ptr<parameter_list> m_parameters;
  memory::unique_ptr<instruction_list> m_body;
  bool m_is_override;
  bool m_is_virtual;
};

class assignment_instruction;
class lvalue : public node {
public:
  lvalue(memory::allocator* _allocator, expression* _expr)
    : node(_allocator, node_type::lvalue),
      m_expression(memory::unique_ptr<expression>(_allocator, _expr)) {}

  explicit lvalue(memory::allocator* _allocator)
    : node(_allocator, node_type::lvalue) {}

  const expression* get_expression() const {
    return m_expression.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("L-Value");
    c->print_value(m_expression, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<lvalue>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_expression = clone_node(_allocator, m_expression.get());
    return t;
  }

private:
  friend class assignment_instruction;
  memory::unique_ptr<expression> m_expression;
};

class assignment_instruction : public instruction {
public:
  assignment_instruction(memory::allocator* _allocator, lvalue* _lvalue)
    : instruction(_allocator, node_type::assignment_instruction),
      m_lvalue(memory::unique_ptr<lvalue>(m_allocator, _lvalue)),
      m_assign_type(assign_type::max),
      m_in_constructor(false) {}

  explicit assignment_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::assignment_instruction) {}

  assignment_instruction(
      memory::allocator* _allocator, memory::unique_ptr<lvalue>&& _lvalue)
    : instruction(_allocator, node_type::assignment_instruction),
      m_lvalue(core::move(_lvalue)),
      m_assign_type(assign_type::max),
      m_in_constructor(false) {}

  void add_value(assign_type _type, memory::unique_ptr<expression>&& _value) {
    m_assign_type = _type;
    m_assign_expression = core::move(_value);
  }

  void set_value(memory::unique_ptr<expression>&& _value) {
    m_assign_expression = core::move(_value);
  }

  void add_value(assign_type _type, expression* _value) {
    m_assign_type = _type;
    m_assign_expression = memory::unique_ptr<expression>(m_allocator, _value);
  }

  bool is_constructor_assigment() const {
    return m_in_constructor;
  }

  void set_in_constructor(bool _in) {
    m_in_constructor = _in;
  }

  assign_type get_assignment_type() const {
    return m_assign_type;
  }

  const expression* get_expression() const {
    return m_assign_expression.get();
  }
  const lvalue* get_lvalue() const {
    return m_lvalue.get();
  }
  lvalue* get_lvalue() {
    return m_lvalue.get();
  }
  expression* get_expression() {
    return m_assign_expression.get();
  }

  memory::unique_ptr<lvalue> transfer_lvalue() {
    return core::move(m_lvalue);
  }

  void set_lvalue(memory::unique_ptr<lvalue> lvalue) {
    m_lvalue = core::move(lvalue);
  }

  memory::unique_ptr<expression> transfer_expression() {
    return core::move(m_assign_expression);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Assignment");
    c->print_value(m_assign_type, "Assign Type");
    c->print_value(m_lvalue, "L-Value");
    c->print_value(m_assign_expression, "Expression");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t =
        memory::make_unique<assignment_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_lvalue = clone_node(_allocator, m_lvalue.get());
    t->m_assign_type = m_assign_type;
    t->m_assign_expression = clone_node(_allocator, m_assign_expression.get());
    t->m_in_constructor = m_in_constructor;
    return t;
  }

private:
  memory::unique_ptr<lvalue> m_lvalue;
  assign_type m_assign_type;
  memory::unique_ptr<expression> m_assign_expression;
  bool m_in_constructor;
};

class do_instruction;
class break_instruction : public instruction {
public:
  break_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::break_instruction, false, true) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Break Instruction");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<break_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    return t;
  }

private:
};

class continue_instruction : public instruction {
public:
  continue_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::continue_instruction, false, true) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Continue");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<continue_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    return t;
  }

private:
};

class do_instruction : public instruction {
public:
  do_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::do_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  do_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::do_instruction) {}

  void set_condition(memory::unique_ptr<expression>&& _expr) {
    m_condition = core::move(_expr);
  }

  void set_body(memory::unique_ptr<instruction_list>&& _body) {
    m_body = core::move(_body);
  }

  const expression* get_condition() const {
    return m_condition.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Do Loop");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<do_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_condition = clone_node(_allocator, m_condition.get());
    t->m_body = clone_node(_allocator, m_body.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class else_if_instruction : public instruction {
public:
  else_if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::else_if_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  explicit else_if_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::else_if_instruction) {}

  const expression* get_condition() const {
    return m_condition.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Else If");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<else_if_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_condition = clone_node(_allocator, m_condition.get());
    t->m_body = clone_node(_allocator, m_body.get());
    return t;
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class if_instruction : public instruction {
public:
  if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::if_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)),
      m_else_if_nodes(_allocator) {}

  if_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::if_instruction),
      m_else_if_nodes(_allocator) {}

  void add_else_if(else_if_instruction* _elseif) {
    m_else_if_nodes.emplace_back(
        memory::unique_ptr<else_if_instruction>(m_allocator, _elseif));
  }
  void add_else(instruction_list* _else) {
    m_else = memory::unique_ptr<instruction_list>(m_allocator, _else);
  }

  const expression* get_condition() const {
    return m_condition.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  const instruction_list* get_else() const {
    return m_else.get();
  }

  const containers::deque<memory::unique_ptr<else_if_instruction>>&
  get_else_if_instructions() const {
    return m_else_if_nodes;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("If");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
    c->print_value(m_else_if_nodes, "ElseIfs");
    c->print_value(m_else, "Else");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<if_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_condition = clone_node(_allocator, m_condition.get());
    t->m_else = clone_node(_allocator, m_else.get());
    t->m_body = clone_node(_allocator, m_body.get());

    t->m_else_if_nodes =
        containers::deque<memory::unique_ptr<else_if_instruction>>(_allocator);
    for (auto& e : m_else_if_nodes) {
      t->m_else_if_nodes.push_back(clone_node(_allocator, e.get()));
    }

    return t;
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_else;
  memory::unique_ptr<instruction_list> m_body;
  containers::deque<memory::unique_ptr<else_if_instruction>> m_else_if_nodes;
};

class for_instruction : public instruction {
public:
  for_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::for_instruction) {}
  void add_initializer(instruction* _init) {
    m_initializer = memory::unique_ptr<instruction>(m_allocator, _init);
  }
  void add_condition(expression* _cond) {
    m_condition = memory::unique_ptr<expression>(m_allocator, _cond);
  }
  void add_post_op(instruction* _inst) {
    m_post_op = memory::unique_ptr<instruction>(m_allocator, _inst);
  }
  void add_body(instruction_list* _body) {
    m_body = memory::unique_ptr<instruction_list>(m_allocator, _body);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("For");
    c->print_value(m_initializer, "Initializer");
    c->print_value(m_condition, "Condition");
    c->print_value(m_post_op, "PostOp");
    c->print_value(m_body, "Body");
  }

  const instruction* get_initializer() const {
    return m_initializer.get();
  }
  const expression* get_condition() const {
    return m_condition.get();
  }
  const instruction* get_post_inst() const {
    return m_post_op.get();
  }
  const instruction_list* get_body() const {
    return m_body.get();
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<for_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_initializer = clone_node(_allocator, m_initializer.get());
    t->m_condition = clone_node(_allocator, m_condition.get());
    t->m_post_op = clone_node(_allocator, m_post_op.get());
    t->m_body = clone_node(_allocator, m_body.get());

    return t;
  }

private:
  memory::unique_ptr<instruction> m_initializer;
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction> m_post_op;
  memory::unique_ptr<instruction_list> m_body;
};

class return_instruction : public instruction {
public:
  return_instruction(memory::allocator* _allocator, expression* _expr)
    : instruction(_allocator, node_type::return_instruction, true, true),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  return_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::return_instruction, true, true) {}

  const expression* get_expression() const {
    return m_expression.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Return");
    c->print_value(m_expression, "Value");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<return_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_expression = clone_node(_allocator, m_expression.get());

    return t;
  }

private:
  memory::unique_ptr<expression> m_expression;
};

class while_instruction : public instruction {
public:
  while_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::while_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  while_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::while_instruction) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("While");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

  const expression* get_condition() const {
    return m_condition.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<while_instruction>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_condition = clone_node(_allocator, m_condition.get());
    t->m_body = clone_node(_allocator, m_body.get());

    return t;
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class script_file : public node {
public:
  struct referenced_resource {
    containers::string m_type;
    containers::string m_string;
    bool m_instantiated;
  };

  script_file(memory::allocator* _allocator)
    : node(_allocator, node_type::script_file),
      m_functions(_allocator),
      m_external_functions(_allocator),
      m_structs(_allocator),
      m_includes(_allocator),
      m_resources(_allocator),
      m_top_level_resources(_allocator) {}
  void add_function(function* _node) {
    m_functions.emplace_back(memory::unique_ptr<function>(m_allocator, _node));
  }

  void add_struct(struct_definition* _node) {
    m_structs.emplace_back(
        memory::unique_ptr<struct_definition>(m_allocator, _node));
  }

  void add_include(containers::string _node) {
    m_includes.push_back(core::move(_node));
  }

  void add_top_level_resource(top_level_resource* _node) {
    // Do not call add_resource here, it will already be done for you.
    m_top_level_resources.emplace_back(
        memory::unique_ptr<top_level_resource>(m_allocator, _node));
  }

  const containers::deque<memory::unique_ptr<function>>&
  get_external_functions() const {
    return m_external_functions;
  }

  const containers::deque<memory::unique_ptr<function>>& get_functions() const {
    return m_functions;
  }

  const containers::deque<memory::unique_ptr<struct_definition>>& get_structs()
      const {
    return m_structs;
  }

  const containers::deque<containers::string>& get_includes() const {
    return m_includes;
  }

  void add_resource(const char* type, const char* value, bool insantiated) {
    m_resources.push_back(
        referenced_resource{containers::string(m_allocator, type),
            containers::string(m_allocator, value), insantiated});
  }

  const containers::deque<referenced_resource>& get_resources() const {
    return m_resources;
  }

  virtual void print_node_internal(print_context* c) const override {
    c->print_header("Script File");
    c->print_value(m_includes, "Includes");
    c->print_value(m_external_functions, "External Functions");
    c->print_value(m_functions, "Functions");
    c->print_value(m_structs, "Structs");
  }

  memory::unique_ptr<node> clone(memory::allocator* _allocator) const override {
    auto t = memory::make_unique<script_file>(_allocator, _allocator);
    t->copy_underlying_from(_allocator, this);
    t->m_functions =
        containers::deque<memory::unique_ptr<function>>(_allocator);
    for (auto& f : m_functions) {
      t->m_functions.push_back(clone_node(_allocator, f.get()));
    }
    t->m_external_functions =
        containers::deque<memory::unique_ptr<function>>(_allocator);
    for (auto& f : m_external_functions) {
      t->m_external_functions.push_back(clone_node(_allocator, f.get()));
    }
    t->m_structs =
        containers::deque<memory::unique_ptr<struct_definition>>(_allocator);
    for (auto& f : m_structs) {
      t->m_structs.push_back(clone_node(_allocator, f.get()));
    }
    t->m_includes = containers::deque<containers::string>(_allocator);
    for (auto& f : m_includes) {
      t->m_includes.push_back(containers::string(_allocator, f));
    }
    t->m_resources = containers::deque<referenced_resource>(_allocator);
    for (auto& f : m_resources) {
      t->m_resources.push_back(
          referenced_resource{containers::string(_allocator, f.m_type),
              containers::string(_allocator, f.m_string), f.m_instantiated});
    }

    return t;
  }

private:
  containers::deque<memory::unique_ptr<function>> m_functions;
  containers::deque<memory::unique_ptr<function>> m_external_functions;
  containers::deque<memory::unique_ptr<struct_definition>> m_structs;
  containers::deque<containers::string> m_includes;
  containers::deque<referenced_resource> m_resources;
  containers::deque<memory::unique_ptr<top_level_resource>>
      m_top_level_resources;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_NODE_TYPES_H__
