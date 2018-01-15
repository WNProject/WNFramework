// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_NODE_TYPES_H__
#define __WN_SCRIPTING_NODE_TYPES_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"
#include "WNScripting/inc/source_location.h"

namespace wn {
namespace scripting {

// clang-format off
enum class node_type {
  arg_list,
  array_type,
  concretized_array_type,
  dynamic_array_type,
  parameter_list,
  parameter,
  expression,
    replaced_expression,
    array_allocation_expression,
    dynamic_array_allocation_expression,
    binary_expression,
    cast_expression,
    cond_expression,
    constant_expression,
    id_expression,
    null_allocation_expression,
    post_expression,
      array_access_expression,
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
    set_array_length,
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
  type
};
// clang-format on
class declaration;
class expression;
class function;
class instruction;
class node;
class parameter;

template <typename T>
const T* cast_to(const node* _node) {
  return static_cast<const T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
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
    : m_allocator(_allocator), m_type(_type), m_is_dead(false) {
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

protected:
  // Location of the first character of the first token contributing
  // to this node.
  source_location m_source_location;
  memory::allocator* m_allocator;
  node_type m_type;
  bool m_is_dead;
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
            static_cast<uint32_t>(type_classification::dynamic_array_type)) {
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

private:
  memory::unique_ptr<type> m_subtype;
};

class concretized_array_type : public type {
public:
  concretized_array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type,
          node_type::concretized_array_type),
      m_array_sizes(_allocator) {}

  void add_array_size(uint32_t size) {
    m_array_sizes.push_back(size);
  }

  const containers::dynamic_array<uint32_t>& get_sizes() const {
    return m_array_sizes;
  }

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("SizedArray");
    c->print_value(m_array_sizes, "NumElements");
    c->print_value(m_type, "Type Value");
    c->print_value(m_subtype, "Subtype");
  }

private:
  containers::dynamic_array<uint32_t> m_array_sizes;
  memory::unique_ptr<type> m_subtype;
};

class dynamic_array_type : public type {
public:
  dynamic_array_type(memory::allocator* _allocator, type* _sub_type)
    : type(_allocator, type_classification::array_type,
          node_type::dynamic_array_type),
      m_subtype(memory::unique_ptr<type>(_allocator, _sub_type)) {}

  explicit dynamic_array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type,
          node_type::dynamic_array_type) {}

  dynamic_array_type(memory::allocator* _allocator, node_type _node_type)
    : type(_allocator, type_classification::array_type, _node_type) {}

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array");
    c->print_value(m_type, "Type Value");
    c->print_value(m_subtype, "SubType");
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
  memory::unique_ptr<type> m_type;
};

enum class struct_initialization_mode {
  invalid,
  unspecified,
  unique,
  strong,
  max
};

class array_allocation_expression : public expression {
public:
  explicit array_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::array_allocation_expression),
      m_array_initializers(_allocator),
      m_init_mode(struct_initialization_mode::invalid),
      m_levels(0) {}

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

  const containers::dynamic_array<uint32_t>& get_static_array_initializers()
      const {
    return m_static_array_initializers;
  }

  bool is_static_sized() const {
    return m_static_array_initializers.size() > 0;
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
    c->print_value(m_static_array_initializers, "Static Initializers");
    c->print_value(m_copy_initializer, "Copy Initializer");
  }

private:
  containers::deque<memory::unique_ptr<expression>> m_array_initializers;
  containers::dynamic_array<uint32_t> m_static_array_initializers;
  memory::unique_ptr<expression> m_copy_initializer;
  containers::string m_constructor_name;
  containers::string m_destructor_name;
  struct_initialization_mode m_init_mode;
  size_t m_levels;
};

class dynamic_array_allocation_expression : public expression {
public:
  explicit dynamic_array_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::dynamic_array_allocation_expression),
      m_array_initializers(_allocator),
      m_init_mode(struct_initialization_mode::invalid),
      m_levels(0) {}

  containers::deque<memory::unique_ptr<expression>>& get_array_initializers() {
    return m_array_initializers;
  }

  struct_initialization_mode set_initialization_mode() const {
    return m_init_mode;
  }

  void set_initialization_mode(struct_initialization_mode _mode) {
    m_init_mode = _mode;
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

  containers::string constructor_name() {
    return m_constructor_name;
  }

  void set_constructor_name(containers::string_view name) {
    m_constructor_name = name.to_string(m_allocator);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Dynamic Array Initializer");
    c->print_value(m_type, "Type");
    c->print_value(m_array_initializers, "Initializers");
    c->print_value(m_copy_initializer, "Copy Initializer");
  }

private:
  containers::deque<memory::unique_ptr<expression>> m_array_initializers;
  memory::unique_ptr<expression> m_copy_initializer;
  containers::string m_constructor_name;
  containers::string m_destructor_name;
  struct_initialization_mode m_init_mode;
  size_t m_levels;
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

private:
};

class post_expression : public expression {
public:
  post_expression(memory::allocator* _allocator, node_type _type)
    : expression(_allocator, _type) {}
  void add_base_expression(expression* _expr) {
    m_base_expression = memory::unique_ptr<expression>(m_allocator, _expr);
  }

  const expression* get_base_expression() const {
    return m_base_expression.get();
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

private:
  memory::unique_ptr<expression> m_array_access;
  bool m_is_construction;
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

private:
  memory::unique_ptr<expression> m_expression;
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

  void print_node_internal(print_context* c) const override {
    c->print_header("Struct Allocation");
    c->print_value(m_type, "Type");
    c->print_value(m_init_mode, "Init Mode");
    c->print_value(m_copy_initializer, "Copy Initializer");
  }

private:
  memory::unique_ptr<expression> m_copy_initializer;
  struct_initialization_mode m_init_mode;
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
      m_is_dead(false),
      m_returns(false),
      m_non_linear(false) {}

  instruction(memory::allocator* _allocator, node_type _type, bool _returns,
      bool _is_non_linear = false, bool _breaks = false)
    : instruction(_allocator, _type) {
    m_returns = _returns;
    m_non_linear = _is_non_linear;
    m_breaks = _breaks;
  }

  bool is_dead() const {
    return m_is_dead;
  }

protected:
  bool m_breaks;
  bool m_is_dead;
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

private:
  containers::deque<memory::unique_ptr<instruction>> m_instructions;
};

struct function_expression {
  function_expression(
      memory::allocator* _allocator, expression* _expr, bool _hand_ownership)
    : m_expr(_allocator, _expr), m_hand_ownership(_hand_ownership) {}
  function_expression(
      memory::unique_ptr<expression>&& _expr, bool _hand_ownership)
    : m_expr(core::move(_expr)), m_hand_ownership(_hand_ownership) {}

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

private:
  containers::deque<memory::unique_ptr<function_expression>> m_expression_list;
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

private:
  memory::unique_ptr<parameter> m_parameter;
  memory::unique_ptr<expression> m_expression;
  bool m_is_default_initialization;
  bool m_is_inherited;
};

class function;
class struct_definition : public node {
public:
  struct_definition(memory::allocator* _allocator, const char* _name,
      bool _is_class = false, const char* _parent_type = nullptr)
    : node(_allocator, node_type::struct_definition),
      m_name(_allocator, _name),
      m_parent_name(_allocator),
      m_is_class(_is_class),
      m_has_virtual(false),
      m_own_vtable(false),
      m_type_index(0),
      m_parent_definition(nullptr),
      m_vtable_slot(0),
      m_struct_members(_allocator),
      m_struct_functions(_allocator),
      m_virtual_functions(_allocator),
      m_initialization_order(_allocator) {
    if (_parent_type) {
      m_parent_name = _parent_type;
    }
  }

  struct_definition(memory::allocator* _allocator)
    : node(_allocator, node_type::struct_definition),
      m_struct_members(_allocator),
      m_struct_functions(_allocator),
      m_virtual_functions(_allocator) {}

  void add_struct_elem(declaration* _decl) {
    m_struct_members.emplace_back(
        memory::unique_ptr<declaration>(m_allocator, _decl));
  }

  void add_function(function* _func) {
    m_struct_functions.emplace_back(
        memory::unique_ptr<function>(m_allocator, _func));
  }

  containers::deque<memory::unique_ptr<function>> take_functions() {
    return core::move(m_struct_functions);
  }

  const containers::deque<memory::unique_ptr<function>>& get_functions() const {
    return m_struct_functions;
  }

  containers::deque<memory::unique_ptr<function>>& get_virtual_functions() {
    return m_virtual_functions;
  }

  containers::deque<memory::unique_ptr<function>> take_virtual_functions() {
    return core::move(m_virtual_functions);
  }

  const containers::deque<memory::unique_ptr<function>>& get_virtual_functions()
      const {
    return m_virtual_functions;
  }

  void set_type_index(uint32_t _index) {
    m_type_index = _index;
  }

  uint32_t get_type_index() const {
    return m_type_index;
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

  const containers::deque<memory::unique_ptr<declaration>>& get_struct_members()
      const {
    return m_struct_members;
  }

  containers::deque<uint32_t>& get_initialization_order() {
    return m_initialization_order;
  }

  const containers::deque<uint32_t>& get_initialization_order() const {
    return m_initialization_order;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header(
        "Struct [", m_name, "]", "(", m_type_index, ")", ":", m_parent_name);
    c->print_value(m_struct_members, "Struct Members");
    c->print_value(m_struct_functions, "Struct functions");
    c->print_value(m_initialization_order, "Initialization Order");
  }

  bool has_virtual() const {
    return m_has_virtual;
  }

  void set_has_virtual(bool has_virtual) {
    m_has_virtual = has_virtual;
  }

  bool has_own_vtable() const {
    return m_own_vtable;
  }

  void set_has_own_vtable() {
    m_own_vtable = true;
  }

  struct_definition* get_parent_definition() {
    return m_parent_definition;
  }

  void set_parent_definition(struct_definition* _def) {
    m_parent_definition = _def;
  }

  void set_vtable_index(uint32_t index) {
    m_vtable_slot = index;
  }

  uint32_t vtable_index() const {
    return m_vtable_slot;
  }

  bool is_class() const {
    return m_is_class;
  }

private:
  containers::string m_name;
  containers::string m_parent_name;
  bool m_is_class;
  bool m_has_virtual;
  bool m_own_vtable;
  uint32_t m_type_index;
  struct_definition* m_parent_definition;
  uint32_t m_vtable_slot;
  containers::deque<memory::unique_ptr<declaration>> m_struct_members;
  containers::deque<memory::unique_ptr<function>> m_struct_functions;
  containers::deque<memory::unique_ptr<function>> m_virtual_functions;
  containers::deque<uint32_t> m_initialization_order;
};

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

private:
  containers::deque<memory::unique_ptr<parameter>> m_parameters;
};

class function : public node {
public:
  function(memory::allocator* _allocator, parameter* _signature,
      parameter_list* _params, instruction_list* _body)
    : node(_allocator, node_type::function),
      m_signature(memory::unique_ptr<parameter>(m_allocator, _signature)),
      m_parameters(memory::unique_ptr<parameter_list>(m_allocator, _params)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)),
      m_this_pointer(nullptr),
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
      m_this_pointer(nullptr),
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
    c->print_value(m_mangled_name, "Mangled Name");
    if (m_this_pointer) {
      // WE don't use the normal print_value, because this
      // would result in a circular reference.
      c->print_value(static_cast<const void*>(m_this_pointer), "This");
    } else {
      c->print_value("<NULL>", "This");
    }
    c->print_value(m_signature, "Signature");
    c->print_value(m_parameters, "Parameters");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<parameter> m_signature;
  memory::unique_ptr<parameter_list> m_parameters;
  memory::unique_ptr<instruction_list> m_body;
  struct_definition* m_this_pointer;
  containers::string m_mangled_name;
  bool m_is_override;
  bool m_is_virtual;
};

class assignment_instruction;
class set_array_length;
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

private:
  friend class assignment_instruction;
  friend class set_array_length;
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

private:
};

class continue_instruction : public instruction {
public:
  continue_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::continue_instruction, false, true) {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Continue");
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

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class script_file : public node {
public:
  script_file(memory::allocator* _allocator)
    : node(_allocator, node_type::script_file),
      m_functions(_allocator),
      m_external_functions(_allocator),
      m_structs(_allocator),
      m_includes(_allocator) {}
  void add_function(function* _node) {
    m_functions.emplace_back(memory::unique_ptr<function>(m_allocator, _node));
  }

  void add_struct(struct_definition* _node) {
    m_structs.emplace_back(
        memory::unique_ptr<struct_definition>(m_allocator, _node));
  }

  void add_include(const char* _node) {
    m_includes.emplace_back(m_allocator, _node);
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

  virtual void print_node_internal(print_context* c) const override {
    c->print_header("Script File");
    c->print_value(m_includes, "Includes");
    c->print_value(m_external_functions, "External Functions");
    c->print_value(m_functions, "Functions");
    c->print_value(m_structs, "Structs");
  }

private:
  containers::deque<memory::unique_ptr<function>> m_functions;
  containers::deque<memory::unique_ptr<function>> m_external_functions;
  containers::deque<memory::unique_ptr<struct_definition>> m_structs;
  containers::deque<containers::string> m_includes;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_NODE_TYPES_H__
