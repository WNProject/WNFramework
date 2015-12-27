// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_NODE_TYPES_H__
#define __WN_NODE_TYPES_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"

namespace wn {
namespace scripting {
struct source_location {
  wn_int64 m_start_index;
  wn_int64 m_end_index;
  const wn_byte* m_line_start;
  wn_size_t m_line_number;
  wn_size_t m_char_number;
};

// clang-format off
enum class node_type {
  arg_list,
  parameter_list,
  parameter,
  expression,
    array_allocation_expression,
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
    short_circuit_expression,
    struct_allocation_expression,
    unary_expression,
  function,
  instruction,
    assignment_instruction,
    declaration,
    do_instruction,
    else_if_instruction,
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
T* cast_to(const node* _node) {
  return static_cast<T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
}
template <typename T>
using walk_ftype = containers::function<void(T)>;
using walk_scope = containers::function<void()>;
// Base class for all nodes in AST.
class node {
public:
  node(memory::allocator* _allocator, node_type _type)
    : m_allocator(_allocator), m_type(_type), m_is_dead(false) {}
  virtual ~node() {}

  wn_void copy_location_from(const node* _node) {
    m_source_location = _node->m_source_location;
  }

  // Sets the start location of first character representing this node.
  wn_void set_start_location(const source_location& _location) {
    m_source_location = _location;
  }

  // Modifies the source_location with the given source index of the last
  // character that represents this node.
  wn_void set_end_index(wn_int64 _idx) {
    m_source_location.m_end_index = _idx;
  }

  // Returns the source location representing this node.
  const source_location& get_start_location() const {
    return (m_source_location);
  }

  // Writes the first line of this expression to the given log.
  wn_void log_line(WNLogging::WNLog& _log, WNLogging::WNLogLevel _level) const {
    wn_size_t line_length = 1023;
    const wn_char* c = memory::strnchr(
        reinterpret_cast<const wn_char*>(m_source_location.m_line_start), '\n',
        1023);
    if (c == wn_nullptr) {
      // If there is no \n then we simply print everything
      c = reinterpret_cast<const wn_char*>(m_source_location.m_line_start) +
          memory::strnlen(
              reinterpret_cast<const wn_char*>(m_source_location.m_line_start),
              1023);
    }
    line_length =
        (c - reinterpret_cast<const wn_char*>(m_source_location.m_line_start));
    wn_char* data_buffer =
        static_cast<wn_char*>(WN_STACK_ALLOC(line_length + 1));
    memcpy(data_buffer, m_source_location.m_line_start, line_length);
    data_buffer[line_length] = '\0';
    _log.Log(_level, 0, "Line: ", m_source_location.m_line_number, "\n",
        data_buffer);
  }

  node_type get_node_type() const {
    return m_type;
  }
  memory::allocator* get_allocator() const {
    return m_allocator;
  }

protected:
  // Location of the first character of the first token contributing
  // to this node.
  source_location m_source_location;
  memory::allocator* m_allocator;
  node_type m_type;
  bool m_is_dead;
};

class type : public node {
public:
  type(memory::allocator* _allocator, const char* _custom_type)
    : node(_allocator, node_type::type),
      m_type(type_classification::custom_type),
      m_num_array_levels(0),
      m_custom_type(_custom_type, _allocator) {}

  type(memory::allocator* _allocator, type_classification _type)
    : node(_allocator, node_type::type),
      m_type(_type),
      m_custom_type(_allocator),
      m_num_array_levels(0) {}

  type(const type& _other)
    : node(_other.m_allocator, node_type::type),
      m_type(_other.m_type),
      m_custom_type(get_allocator()),
      m_num_array_levels(_other.m_num_array_levels) {
    m_custom_type = _other.m_custom_type;
  }

  bool operator==(const type& _other) const {
    if (m_type != _other.m_type) {
      return false;
    }
    if (m_type != type_classification::custom_type) {
      return true;
    }
    if (m_custom_type != _other.m_custom_type) {
      return false;
    }
    return m_num_array_levels == _other.m_num_array_levels;
  }

  bool operator!=(const type& _other) const {
    return !(*this == _other);
  }

  wn_uint32 get_type_value() const {
    return static_cast<wn_uint32>(m_type);
  }
  void add_array_level() {
    m_num_array_levels += 1;
  }
  type_classification get_classification() const {
    return m_type;
  }
  containers::string_view custom_type_name() const {
    return m_custom_type;
  }

private:
  type_classification m_type;
  containers::string m_custom_type;
  wn_size_t m_num_array_levels;
};

class array_type : public type {
public:
  array_type(memory::allocator* _allocator, type* _sub_type,
      wn_int32 _num_array_levels = -1)
    : type(_allocator, type_classification::array_type),
      m_subtype(memory::default_allocated_ptr(_allocator, _sub_type)),
      m_num_array_levels(_num_array_levels) {}

  const type* get_subtype() const {
     return m_subtype.get();
  }

private:
  wn_int32 m_num_array_levels;
  memory::allocated_ptr<type> m_subtype;
};

// Base class for all expression nodes in the AST.
class expression : public node {
public:
  expression(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type),
      m_type(wn_nullptr),
      m_force_use(wn_false),
      m_newly_created(wn_false) {}

  expression(memory::allocator* _allocator, node_type _node_type, type* _type)
    : node(_allocator, _node_type),
      m_type(memory::default_allocated_ptr(_allocator, _type)),
      m_force_use(wn_false),
      m_newly_created(wn_false) {}

  // Returns true if the value of this expression cannot be ignored.
  wn_bool required_use() const {
    return m_force_use;
  }

  wn_bool is_newly_created() {
    return m_newly_created;
  }

  const type* get_type() const {
    return m_type.get();
  }

  void set_type(type* _type) {
    m_type = memory::default_allocated_ptr(m_allocator, _type);
  }

  virtual void walk_children(
      const walk_ftype<expression*>&, const walk_ftype<type*>&) {
    WN_RELEASE_ASSERT_DESC(false, "Not implemented yet");
  }
  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>&) const {
    WN_RELEASE_ASSERT_DESC(false, "Not implemented yet");
  }

protected:
  memory::allocated_ptr<type> m_type;

private:
  wn_bool m_force_use;
  wn_bool m_newly_created;
};

class array_allocation_expression : public expression {
public:
  array_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::array_allocation_expression),
      m_array_initializers(_allocator),
      m_levels(0) {}

  wn_void add_expression(expression* _expr) {
    m_array_initializers.emplace_back(
        memory::default_allocated_ptr(m_allocator, _expr));
    m_levels++;
  }
  wn_void add_level() {
    m_levels++;
  }
  wn_void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::default_allocated_ptr(m_allocator, _expression);
  }

private:
  containers::deque<memory::allocated_ptr<expression>> m_array_initializers;
  memory::allocated_ptr<expression> m_copy_initializer;
  wn_size_t m_levels;
};

class binary_expression : public expression {
public:
  binary_expression(memory::allocator* _allocator, arithmetic_type _type,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(_type),
      m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
      m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}
  const expression* get_lhs() const {
    return m_lhs.get();
  }
  const expression* get_rhs() const {
    return m_rhs.get();
  }
  const arithmetic_type get_arithmetic_type() const {
    return m_arith_type;
  }

  virtual void walk_children(
      const walk_ftype<expression*>& _func, const walk_ftype<type*>&) {
    _func(m_lhs.get());
    _func(m_rhs.get());
  }
  virtual void walk_children(const walk_ftype<const expression*>& _func,
      const walk_ftype<const type*>&) const {
    _func(m_lhs.get());
    _func(m_rhs.get());
  }

private:
  arithmetic_type m_arith_type;
  memory::allocated_ptr<expression> m_lhs;
  memory::allocated_ptr<expression> m_rhs;
};

class cond_expression : public expression {
public:
  cond_expression(memory::allocator* _allocator, expression* _cond,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::cond_expression),
      m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
      m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
      m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}

private:
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<expression> m_lhs;
  memory::allocated_ptr<expression> m_rhs;
};
class constant_expression : public expression {
public:
  constant_expression(memory::allocator* _allocator, type_classification _type,
      const char* _text)
    : expression(_allocator, node_type::constant_expression),
      m_type_classification(_type),
      m_text(_text, _allocator) {}

  constant_expression(
      memory::allocator* _allocator, type* _type, const char* _text)
    : expression(_allocator, node_type::constant_expression, _type),
      m_type_classification(_type->get_classification()),
      m_text(_text, _allocator) {}

  type_classification get_classification() const {
    return m_type_classification;
  }
  const containers::string& get_type_text() const {
    return m_text;
  }

  virtual void walk_children(
      const walk_ftype<expression*>&, const walk_ftype<type*>& _type) {
    _type(m_type.get());
  }
  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const {
    _type(m_type.get());
  }

private:
  type_classification m_type_classification;
  containers::string m_text;
};

class id_expression : public expression {
public:
  id_expression(memory::allocator* _allocator, const char* _name)
    : expression(_allocator, node_type::id_expression),
      m_source({wn_nullptr, wn_nullptr}),
      m_name(_name, _allocator) {}
  containers::string_view get_name() const {
    return m_name;
  }

  virtual void walk_children(
      const walk_ftype<expression*>&, const walk_ftype<type*>&) {}

  virtual void walk_children(
      const walk_ftype<const expression*>&, const walk_ftype<const type*>&) {}

  struct id_source {
    parameter* param_source;
    declaration* declaration_source;
  };
  void set_id_source(const id_source& _source) {
    WN_RELEASE_ASSERT_DESC(
        (_source.param_source == 0) ^ (_source.declaration_source == 0),
        "Either the source must be a parameter or a declaration");
    m_source = _source;
  };
  const id_source& get_id_source() const {
    return m_source;
  }

private:
  id_source m_source;
  containers::string m_name;
};

class null_allocation_expression : public expression {
public:
  null_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::null_allocation_expression) {}

private:
};

class post_expression : public expression {
public:
  post_expression(memory::allocator* _allocator, node_type _type)
    : expression(_allocator, _type) {}
  void add_base_expression(expression* _expr) {
    m_base_expression = memory::default_allocated_ptr(m_allocator, _expr);
  }

protected:
  memory::allocated_ptr<expression> m_base_expression;
};

class array_access_expression : public post_expression {
public:
  array_access_expression(memory::allocator* _allocator, expression* _expr)
    : post_expression(_allocator, node_type::array_access_expression),
      m_array_access(memory::default_allocated_ptr(m_allocator, _expr)) {}

private:
  memory::allocated_ptr<expression> m_array_access;
};

class short_circuit_expression : public expression {
public:
  short_circuit_expression(memory::allocator* _allocator,
      short_circuit_type _type, expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::short_circuit_expression),
      m_ss_type(_type),
      m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
      m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}

private:
  short_circuit_type m_ss_type;
  memory::allocated_ptr<expression> m_lhs;
  memory::allocated_ptr<expression> m_rhs;
};

class member_access_expression : public post_expression {
public:
  member_access_expression(memory::allocator* _allocator, const char* _member)
    : post_expression(_allocator, node_type::member_access_expression),
      m_member(_member, _allocator) {}

private:
  containers::string m_member;
};

class post_unary_expression : public post_expression {
public:
  post_unary_expression(memory::allocator* _allocator, post_unary_type _type)
    : post_expression(_allocator, node_type::post_unary_expression),
      m_unary_type(_type) {}

private:
  post_unary_type m_unary_type;
};

class cast_expression : public expression {
public:
  cast_expression(memory::allocator* _allocator, expression* _expression)
    : expression(_allocator, node_type::cast_expression),
      m_expression(memory::default_allocated_ptr(m_allocator, _expression)) {}

private:
  memory::allocated_ptr<expression> m_expression;
};

class struct_allocation_expression : public expression {
public:
  struct_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::struct_allocation_expression),
      m_copy_initializer(wn_nullptr) {}
  wn_void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::default_allocated_ptr(m_allocator, _expression);
  }

private:
  memory::allocated_ptr<expression> m_copy_initializer;
};

class unary_expression : public expression {
public:
  unary_expression(
      memory::allocator* _allocator, unary_type _type, expression* _expr)
    : expression(_allocator, node_type::unary_expression),
      m_unary_type(_type),
      m_root_expression(memory::default_allocated_ptr(m_allocator, _expr)) {}

private:
  unary_type m_unary_type;
  memory::allocated_ptr<expression> m_root_expression;
};

class instruction_list;
class instruction : public node {
public:
  instruction(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type), m_returns(wn_false), m_is_dead(wn_false) {}
  instruction(memory::allocator* _allocator, node_type _type, wn_bool _returns)
    : instruction(_allocator, _type) {
    m_returns = _returns;
  }
  // Returns true if this instruction causes the function to return.
  wn_bool returns() const {
    return (m_returns);
  }

  virtual void walk_children(const walk_ftype<instruction*>&,
      const walk_ftype<expression*>&, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&) {
    WN_RELEASE_ASSERT_DESC(false, "Not Implemented");
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&, const walk_ftype<type*>&,
      const walk_ftype<const instruction_list*>&) const {
    WN_RELEASE_ASSERT_DESC(false, "Not Implemented");
  }

  void set_is_dead() {
    m_is_dead = true;
  }
  void set_returns(wn_bool returns) {
    m_returns = returns;
  }
  wn_bool is_dead() const {
    return m_is_dead;
  }

protected:
  wn_bool m_is_dead;
  wn_bool m_returns;
};

struct function_expression {
  function_expression(expression* _expr, wn_bool _hand_ownership)
    : m_expr(_expr), m_hand_ownership(_hand_ownership) {}
  memory::allocated_ptr<expression> m_expr;
  wn_bool m_hand_ownership;
};

class instruction_list : public node {
public:
  instruction_list(memory::allocator* _allocator)
    : node(_allocator, node_type::instruction_list),
      m_instructions(_allocator),
      m_returns(wn_false) {}
  instruction_list(memory::allocator* _allocator, instruction* inst)
    : node(_allocator, node_type::instruction_list),
      m_instructions(_allocator),
      m_returns(wn_false) {
    m_instructions.emplace_back(
        memory::default_allocated_ptr(m_allocator, inst));
  }
  ~instruction_list() {}
  void add_instruction(instruction* inst) {
    m_instructions.push_back(memory::default_allocated_ptr(m_allocator, inst));
  }

  wn_bool returns() const {
    return (m_returns);
  }
  void set_returns() {
    m_returns = true;
  }

  const containers::deque<memory::allocated_ptr<instruction>>&
  get_instructions() const {
    return m_instructions;
  }

  void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope,
      const walk_ftype<instruction*>& _instruction) {
    _enter_scope();
    for (auto& inst : m_instructions) {
      _instruction(inst.get());
    }
    _leave_scope();
  }

  void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope,
      const walk_ftype<const instruction*>& _instruction) const {
    _enter_scope();
    for (auto& inst : m_instructions) {
      _instruction(inst.get());
    }
    _leave_scope();
  }

  void remove_dead_instructions() {
    containers::deque<memory::allocated_ptr<instruction>> instructions;
    while (!m_instructions.empty()) {
      if (!m_instructions.front()->is_dead()) {
        instructions.push_back(std::move(*m_instructions.begin()));
      }
      m_instructions.pop_front();
    }
    m_instructions = std::move(instructions);
  }

private:
  containers::deque<memory::allocated_ptr<instruction>> m_instructions;
  wn_bool m_returns;
};

class arg_list : public node {
public:
  arg_list(memory::allocator* _allocator)
    : node(_allocator, node_type::arg_list), m_expression_list(_allocator) {}
  void add_expression(expression* _expr, wn_bool _hand_ownership = wn_false) {
    m_expression_list.emplace_back(
        m_allocator->make_allocated<function_expression>(
            _expr, _hand_ownership));
  }

  containers::deque<memory::allocated_ptr<function_expression>>&
  get_expressions() {
    return (m_expression_list);
  }

private:
  containers::deque<memory::allocated_ptr<function_expression>>
      m_expression_list;
};

class function_call_expression : public post_expression {
public:
  function_call_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::function_call_expression) {}
  function_call_expression(memory::allocator* _allocator, arg_list* _list)
    : post_expression(_allocator, node_type::function_call_expression),
      m_args(_list) {}

private:
  memory::allocated_ptr<arg_list> m_args;
};

class parameter : public node {
public:
  parameter(memory::allocator* _allocator, type* _type, const char* _name)
    : node(_allocator, scripting::node_type::parameter),
      m_type(memory::default_allocated_ptr(_allocator, _type)),
      m_name(_name, _allocator) {}
  const containers::string_view get_name() const {
    return (m_name.c_str());
  }
  const type* get_type() const {
    return m_type.get();
  }

  virtual void walk_children(const walk_ftype<type*>& _type_func) {
    _type_func(m_type.get());
  }
  virtual void walk_children(const walk_ftype<const type*>& _type_func) {
    _type_func(m_type.get());
  }

private:
  friend class declaration;
  containers::string m_name;
  memory::allocated_ptr<type> m_type;
};

class declaration : public instruction {
public:
  declaration(memory::allocator* _allocator)
    : instruction(_allocator, node_type::declaration),
      m_sized_array_initializers(_allocator),
      m_unsized_array_initializers(0) {}
  void set_parameter(parameter* _parameter) {
    m_parameter = memory::default_allocated_ptr(m_allocator, _parameter);
  }
  void add_expression_initializer(expression* _expr) {
    m_expression = memory::default_allocated_ptr(m_allocator, _expr);
  }

  void add_sized_array_initializer(expression* _expr) {
    m_sized_array_initializers.emplace_back(
        memory::default_allocated_ptr(m_allocator, _expr));
  }

  void add_unsized_array_intitializer() {
    m_unsized_array_initializers += 1;
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
  const containers::deque<memory::allocated_ptr<expression>>& get_array_sizes()
      const {
    return m_sized_array_initializers;
  }
  wn_size_t get_num_unsized_array_initializers() const {
    return m_unsized_array_initializers;
  }

  virtual void walk_children(const walk_ftype<instruction*>&,
      const walk_ftype<expression*>& expr, const walk_ftype<type*>& type,
      const walk_ftype<instruction_list*>&) {
    if (type) {
      type(m_parameter->m_type.get());
    }
    if (m_expression) {
      expr(m_expression.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr,
      const walk_ftype<const type*>& type,
      const walk_ftype<const instruction_list*>&) const {
    if (type) {
      type(m_parameter->m_type.get());
    }
    if (m_expression) {
      expr(m_expression.get());
    }
  }

private:
  memory::allocated_ptr<parameter> m_parameter;
  memory::allocated_ptr<expression> m_expression;
  containers::deque<memory::allocated_ptr<expression>>
      m_sized_array_initializers;
  wn_size_t m_unsized_array_initializers;
};

class function;
class struct_definition : public node {
public:
  struct_definition(memory::allocator* _allocator, const char* _name,
      bool _is_class = false, const char* _parent_type = wn_nullptr)
    : node(_allocator, node_type::struct_definition),
      m_name(_name, _allocator),
      m_parent_name(_allocator),
      m_is_class(_is_class),
      m_struct_members(_allocator),
      m_struct_functions(_allocator) {
    if (_parent_type) {
      m_parent_name = _parent_type;
    }
  }

  void add_struct_elem(declaration* _decl) {
    m_struct_members.emplace_back(
        memory::default_allocated_ptr(m_allocator, _decl));
  }

  void add_function(function* _func) {
    m_struct_functions.emplace_back(
        memory::default_allocated_ptr(m_allocator, _func));
  }

  const wn_char* get_name() const {
    return (m_name.c_str());
  }

private:
  containers::string m_name;
  containers::string m_parent_name;
  bool m_is_class;
  containers::deque<memory::allocated_ptr<declaration>> m_struct_members;
  containers::deque<memory::allocated_ptr<function>> m_struct_functions;
};

class parameter_list : public node {
public:
  parameter_list(memory::allocator* _allocator, parameter* _param)
    : node(_allocator, node_type::parameter_list), m_parameters(_allocator) {
    m_parameters.emplace_back(
        memory::default_allocated_ptr(m_allocator, _param));
  }
  void add_parameter(parameter* _param) {
    m_parameters.emplace_back(
        memory::default_allocated_ptr(m_allocator, _param));
  }

  const containers::deque<memory::allocated_ptr<parameter>>& get_parameters()
      const {
    return m_parameters;
  }

private:
  containers::deque<memory::allocated_ptr<parameter>> m_parameters;
};

class function : public node {
public:
  function(memory::allocator* _allocator, parameter* _signature,
      parameter_list* _params, instruction_list* _body)
    : node(_allocator, node_type::function),
      m_signature(memory::default_allocated_ptr(m_allocator, _signature)),
      m_parameters(memory::default_allocated_ptr(m_allocator, _params)),
      m_body(memory::default_allocated_ptr(m_allocator, _body)),
      m_is_override(wn_false),
      m_is_virtual(wn_false) {}
  void set_is_virtual(bool is_virtual) {
    m_is_virtual = is_virtual;
  }
  void set_is_override(bool is_override) {
    m_is_virtual = is_override;
  }
  const parameter* get_signature() const {
    return m_signature.get();
  }
  const parameter_list* get_parameters() const {
    return m_parameters.get();
  }
  const instruction_list* get_body() const {
    return m_body.get();
  }

  virtual void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope, const walk_ftype<parameter*>& _parameter,
      const walk_ftype<instruction_list*>& _instructions,
      const walk_ftype<type*>& _type) {
    m_signature->walk_children(_type);
    _enter_scope();
    if (m_parameters) {
      for (auto& param : m_parameters->get_parameters()) {
        _parameter(param.get());
      }
    }
    _instructions(m_body.get());
    _leave_scope();
  }

  virtual void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope,
      const walk_ftype<const parameter*>& _parameter,
      const walk_ftype<const instruction_list*>& _instructions,
      const walk_ftype<const type*>& _type) const {
    m_signature->walk_children(_type);
    _enter_scope();
    for (auto& param : m_parameters->get_parameters()) {
      _parameter(param.get());
    }
    _instructions(m_body.get());
    _leave_scope();
  }

private:
  memory::allocated_ptr<parameter> m_signature;
  memory::allocated_ptr<parameter_list> m_parameters;
  memory::allocated_ptr<instruction_list> m_body;

  bool m_is_override;
  bool m_is_virtual;
};

class assignment_instruction;
class lvalue : public node {
public:
  lvalue(memory::allocator* _allocator, expression* _expr)
    : node(_allocator, node_type::lvalue),
      m_expression(memory::default_allocated_ptr(_allocator, _expr)) {}
  wn_bool required_use() {
    return m_expression->required_use();
  }
  const expression* get_expression() const {
    return m_expression.get();
  }

private:
  friend class assignment_instruction;
  memory::allocated_ptr<expression> m_expression;
};

class assignment_instruction : public instruction {
public:
  assignment_instruction(memory::allocator* _allocator, lvalue* _lvalue)
    : instruction(_allocator, node_type::assignment_instruction),
      m_lvalue(memory::default_allocated_ptr(m_allocator, _lvalue)),
      m_assign_type(assign_type::max) {}

  void add_value(assign_type _type, expression* _value) {
    m_assign_type = _type;
    m_assign_expression = memory::default_allocated_ptr(m_allocator, _value);
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

private:
  virtual void walk_children(const walk_ftype<instruction*>&,
      const walk_ftype<expression*>& expr, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&) {
    if (expr) {
      expr(m_lvalue->m_expression.get());
      expr(m_assign_expression.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&) const {
    if (expr) {
      expr(m_lvalue->m_expression.get());
      expr(m_assign_expression.get());
    }
  }

  assign_type m_assign_type;
  memory::allocated_ptr<lvalue> m_lvalue;
  memory::allocated_ptr<expression> m_assign_expression;
};

class do_instruction : public instruction {
public:
  do_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::do_instruction),
      m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
      m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

private:
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<instruction_list> m_body;
};

class else_if_instruction : public instruction {
public:
  else_if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::else_if_instruction),
      m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
      m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

  const expression* get_condition() const {
    return m_condition.get();
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  virtual void walk_children(const walk_ftype<instruction*>&,
      const walk_ftype<expression*>& _cond, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>& _body) {
    _cond(m_condition.get());
    _body(m_body.get());
  }

  virtual void walk_children(const walk_ftype<const instruction*>,
      const walk_ftype<const expression*>& _cond,
      const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>& _body) const {
    _cond(m_condition.get());
    _body(m_body.get());
  }

private:
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<instruction_list> m_body;
};

class if_instruction : public instruction {
public:
  if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::if_instruction),
      m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
      m_body(memory::default_allocated_ptr(m_allocator, _body)),
      m_else_if_nodes(_allocator) {}

  void add_else_if(else_if_instruction* _elseif) {
    m_else_if_nodes.emplace_back(
        memory::default_allocated_ptr(m_allocator, _elseif));
  }
  void add_else(instruction_list* _else) {
    m_else = memory::default_allocated_ptr(m_allocator, _else);
  }

  virtual void walk_children(const walk_ftype<instruction*>& _inst,
      const walk_ftype<expression*>& _cond, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>& _body) {
    _cond(m_condition.get());
    _body(m_body.get());
    for (auto& else_inst : m_else_if_nodes) {
      _inst(else_inst.get());
    }
    if (m_else) {
      _body(m_else.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>& _inst,
      const walk_ftype<const expression*>& _cond, const walk_ftype<type*>&,
      const walk_ftype<const instruction_list*>& _body) const {
    _cond(m_condition.get());
    _body(m_body.get());
    for (auto& else_inst : m_else_if_nodes) {
      _inst(else_inst.get());
    }
    if (m_else) {
      _body(m_else.get());
    }
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

  const containers::deque<memory::allocated_ptr<else_if_instruction>>&
  get_else_if_instructions() const {
    return m_else_if_nodes;
  }

private:
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<instruction_list> m_else;
  memory::allocated_ptr<instruction_list> m_body;
  containers::deque<memory::allocated_ptr<else_if_instruction>> m_else_if_nodes;
};

class for_instruction : public instruction {
public:
  for_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::for_instruction) {}
  void add_initializer(instruction* _init) {
    m_initializer = memory::default_allocated_ptr(m_allocator, _init);
  }
  void add_condition(expression* _cond) {
    m_condition = memory::default_allocated_ptr(m_allocator, _cond);
  }
  void add_post_op(instruction* _inst) {
    m_post_op = memory::default_allocated_ptr(m_allocator, _inst);
  }
  void add_body(instruction_list* _body) {
    m_body = memory::default_allocated_ptr(m_allocator, _body);
  }

private:
  memory::allocated_ptr<instruction> m_initializer;
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<instruction> m_post_op;
  memory::allocated_ptr<instruction_list> m_body;
};

class return_instruction : public instruction {
public:
  return_instruction(memory::allocator* _allocator, expression* _expr,
      bool _change_ownership = false)
    : instruction(_allocator, node_type::return_instruction, true),
      m_expression(memory::default_allocated_ptr(m_allocator, _expr)),
      m_change_ownership(_change_ownership) {}

  return_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::return_instruction, true) {
    m_change_ownership = wn_false;
  }

  const expression* get_expression() const {
    return m_expression.get();
  }
  virtual void walk_children(const walk_ftype<instruction*>&,
      const walk_ftype<expression*>& expr, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&) {
    if (m_expression) {
      expr(m_expression.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr, const walk_ftype<type*>&,
      const walk_ftype<const instruction_list*>&) const {
    if (m_expression) {
      expr(m_expression.get());
    }
  }

private:
  memory::allocated_ptr<expression> m_expression;
  wn_bool m_change_ownership;
};

class while_instruction : public instruction {
public:
  while_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::while_instruction),
      m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
      m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

private:
  memory::allocated_ptr<expression> m_condition;
  memory::allocated_ptr<instruction_list> m_body;
};

class script_file : public node {
public:
  script_file(memory::allocator* _allocator)
    : node(_allocator, node_type::script_file),
      m_functions(_allocator),
      m_structs(_allocator),
      m_includes(_allocator) {}
  void add_function(function* _node) {
    m_functions.emplace_back(memory::default_allocated_ptr(m_allocator, _node));
  }

  void add_struct(struct_definition* _node) {
    m_structs.emplace_back(memory::default_allocated_ptr(m_allocator, _node));
  }

  void add_include(const char* _node) {
    m_includes.emplace_back(_node, m_allocator);
  }

  const containers::deque<memory::allocated_ptr<function>>& get_functions()
      const {
    return m_functions;
  }

  const containers::deque<memory::allocated_ptr<struct_definition>>&
  get_structs() const {
    return m_structs;
  }
  const containers::deque<containers::string>& get_includes() const {
    return m_includes;
  }

  virtual void walk_children(const walk_scope&, const walk_scope&,
      const walk_ftype<instruction*>&, const walk_ftype<expression*>&,
      const walk_ftype<function*>& f) {
    for (auto& function : m_functions) {
      f(function.get());
    }
  }

  virtual void walk_children(const walk_scope&, const walk_scope&,
      const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&,
      const walk_ftype<const function*>& f) const {
    for (auto& function : m_functions) {
      f(function.get());
    }
  }

private:
  containers::deque<memory::allocated_ptr<function>> m_functions;
  containers::deque<memory::allocated_ptr<struct_definition>> m_structs;
  containers::deque<containers::string> m_includes;
};

}  // namespace scripting
}  // namespace wn
#endif  //__WN_NODE_TYPES_H__
