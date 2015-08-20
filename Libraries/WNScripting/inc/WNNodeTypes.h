// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_NODE_TYPES_H__
#define __WN_NODE_TYPES_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNErrors.h"
#include "WNScripting/inc/WNEnums.h"

namespace wn {
namespace scripting {
struct source_location {
  wn_int64 m_start_index;
  wn_int64 m_end_index;
  const wn_byte *m_line_start;
  wn_size_t m_line_number;
  wn_size_t m_char_number;
};

// Base class for all nodes in AST.
class node {
 public:
  node(wn::memory::allocator *_allocator) : m_allocator(_allocator) {}
  virtual ~node() {}

  // Sets the start location of first character representing this node.
  wn_void set_start_location(const source_location &_location) {
    m_source_location = _location;
  }

  // Modifies the source_location with the given source index of the last
  // character that represents this node.
  wn_void set_end_index(wn_int64 _idx) { m_source_location.m_end_index = _idx; }

  // Returns the source location representing this node.
  const source_location &get_start_location() const {
    return (m_source_location);
  }

  // Writes the first line of this expression to the given log.
  wn_void log_line(WNLogging::WNLog &_log, WNLogging::WNLogLevel _level) const {
    wn_size_t line_length = 1023;
    const wn_char *c = wn::memory::strnchr(
        reinterpret_cast<const wn_char *>(m_source_location.m_line_start), '\n',
        1023);
    line_length =
        (c - reinterpret_cast<const wn_char *>(m_source_location.m_line_start));
    wn_char *data_buffer =
        static_cast<wn_char *>(WN_STACK_ALLOC(line_length + 1));
    memcpy(data_buffer, m_source_location.m_line_start, line_length);
    data_buffer[line_length] = '\0';
    _log.Log(_level, 0, "Line: ", m_source_location.m_line_number, "\n",
             data_buffer);
  }

 protected:
  // Location of the first character of the first token contributing
  // to this node.
  source_location m_source_location;
  memory::allocator *m_allocator;
};

class type_node : public node {
 public:
  type_node(wn::memory::allocator *_allocator, const char *_custom_type)
      : node(_allocator),
        m_type(type_classification::custom_type),
        m_num_array_levels(0),
        m_custom_type(_custom_type, _allocator) {}

  void add_array_level() { m_num_array_levels += 1; }

 private:
  type_classification m_type;
  containers::string m_custom_type;
  wn_size_t m_num_array_levels;
};

// Base class for all expression nodes in the AST.
class expression : public node {
 public:
  expression(wn::memory::allocator *_allocator)
      : node(_allocator), m_force_use(wn_false), m_newly_created(wn_false) {}

  // Returns true if the value of this expression cannot be ignored.
  wn_bool required_use() const { return m_force_use; }

  wn_bool is_newly_created() { return m_newly_created; }

 private:
  wn_bool m_force_use;
  wn_bool m_newly_created;
};

class array_allocation_expression : public expression {
 public:
  array_allocation_expression(wn::memory::allocator *_allocator)
      : expression(_allocator), m_array_initializers(_allocator), m_levels(0) {}

  wn_void set_type(type_node *_type_node) {
    m_type = memory::default_allocated_ptr(m_allocator, _type_node);
  }
  wn_void add_expression(expression *_expr) {
    m_array_initializers.emplace_back(
        memory::default_allocated_ptr(m_allocator, _expr));
    m_levels++;
  }
  wn_void add_level() { m_levels++; }
  wn_void set_copy_initializer(expression *_expression) {
    m_copy_initializer =
        memory::default_allocated_ptr(m_allocator, _expression);
  }

 private:
  wn::containers::deque<wn::memory::allocated_ptr<expression>>
      m_array_initializers;
  wn::memory::allocated_ptr<expression> m_copy_initializer;
  wn::memory::allocated_ptr<type_node> m_type;
  wn_size_t m_levels;
};

class binary_expression : public expression {
 public:
  binary_expression(wn::memory::allocator *_allocator, arithmetic_type _type,
                    expression *_lhs, expression *_rhs)
      : expression(_allocator),
        m_type(_type),
        m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
        m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}

 private:
  arithmetic_type m_type;
  wn::memory::allocated_ptr<expression> m_lhs;
  wn::memory::allocated_ptr<expression> m_rhs;
};

class cond_expression : public expression {
 public:
  cond_expression(wn::memory::allocator *_allocator, expression *_cond,
                  expression *_lhs, expression *_rhs)
      : expression(_allocator),
        m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
        m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
        m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}

 private:
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<expression> m_lhs;
  wn::memory::allocated_ptr<expression> m_rhs;
};

class constant_expression : public expression {
 public:
  constant_expression(wn::memory::allocator *_allocator,
                      type_classification _type, const char *_text)
      : expression(_allocator), m_type_name(_type), m_text(_text, _allocator) {}

 private:
  type_classification m_type_name;
  containers::string m_text;
};

class id_expression : public expression {
 public:
  id_expression(wn::memory::allocator *_allocator, const char *_name)
      : expression(_allocator), m_name(_name, _allocator) {}

 private:
  containers::string m_name;
};

class null_allocation_expression : public expression {
 public:
  null_allocation_expression(wn::memory::allocator *_allocator)
      : expression(_allocator) {}

 private:
};

class post_expression : public expression {
 public:
  post_expression(wn::memory::allocator *_allocator) : expression(_allocator) {}
  void add_base_expression(expression *_expr) {
    m_base_expression = memory::default_allocated_ptr(m_allocator, _expr);
  }

 protected:
  wn::memory::allocated_ptr<expression> m_base_expression;
};

class array_access_expression : public post_expression {
 public:
  array_access_expression(wn::memory::allocator *_allocator, expression *_expr)
      : post_expression(_allocator),
        m_array_access(memory::default_allocated_ptr(m_allocator, _expr)) {}

 private:
  wn::memory::allocated_ptr<expression> m_array_access;
};

class ss_expression : public expression {
 public:
  ss_expression(wn::memory::allocator *_allocator, short_circuit_type _type,
                expression *_lhs, expression *_rhs)
      : expression(_allocator),
        m_ss_type(_type),
        m_lhs(memory::default_allocated_ptr(m_allocator, _lhs)),
        m_rhs(memory::default_allocated_ptr(m_allocator, _rhs)) {}

 private:
  short_circuit_type m_ss_type;
  wn::memory::allocated_ptr<expression> m_lhs;
  wn::memory::allocated_ptr<expression> m_rhs;
};

class member_access_expression : public post_expression {
 public:
  member_access_expression(wn::memory::allocator *_allocator,
                           const char *_member)
      : post_expression(_allocator), m_member(_member, _allocator) {}

 private:
  containers::string m_member;
};

class post_unary_expression : public post_expression {
 public:
  post_unary_expression(wn::memory::allocator *_allocator,
                        post_unary_type _type)
      : post_expression(_allocator), m_type(_type) {}

 private:
  post_unary_type m_type;
};

class struct_allocation_expression : public expression {
 public:
  struct_allocation_expression(wn::memory::allocator *_allocator)
      : expression(_allocator),
        m_type(wn_nullptr),
        m_copy_initializer(wn_nullptr) {}
  wn_void set_type(type_node *_type) {
    m_type = memory::default_allocated_ptr(m_allocator, _type);
  }
  wn_void set_copy_initializer(expression *_expression) {
    m_copy_initializer =
        memory::default_allocated_ptr(m_allocator, _expression);
  }

 private:
  wn::memory::allocated_ptr<type_node> m_type;
  wn::memory::allocated_ptr<expression> m_copy_initializer;
};

class unary_expression : public expression {
 public:
  unary_expression(wn::memory::allocator *_allocator, unary_type _type,
                   expression *_expr)
      : expression(_allocator),
        m_type(_type),
        m_root_expression(memory::default_allocated_ptr(m_allocator, _expr)) {}

 private:
  unary_type m_type;
  wn::memory::allocated_ptr<expression> m_root_expression;
};

class instruction : public node {
 public:
  instruction(wn::memory::allocator *_allocator)
      : node(_allocator), m_returns(wn_false) {}
  // Returns true if this instruction causes the function to return.
  wn_bool returns() { return (m_returns); }

 protected:
  wn_bool m_returns;
};

struct function_expression {
  function_expression(expression *_expr, wn_bool _hand_ownership)
      : m_expr(_expr), m_hand_ownership(_hand_ownership) {}
  wn::memory::allocated_ptr<expression> m_expr;
  wn_bool m_hand_ownership;
};

class instruction_list : public node {
 public:
  instruction_list(wn::memory::allocator *_allocator)
      : node(_allocator), m_instructions(_allocator), m_returns(wn_false) {}
  instruction_list(wn::memory::allocator *_allocator, instruction *inst)
      : node(_allocator), m_instructions(_allocator) {
    m_instructions.emplace_back(
        memory::default_allocated_ptr(m_allocator, inst));
  }
  ~instruction_list() {}
  void add_instruction(instruction *inst) {
    m_instructions.emplace_back(
        memory::default_allocated_ptr(m_allocator, inst));
  }

  wn_bool returns() { return (m_returns); }

 private:
  wn::containers::deque<wn::memory::allocated_ptr<instruction>> m_instructions;
  wn_bool m_returns;
};

class arg_list : public node {
 public:
  arg_list(wn::memory::allocator *_allocator)
      : node(_allocator), m_expression_list(_allocator) {}
  void add_expression(expression *_expr, wn_bool _hand_ownership = wn_false) {
    m_expression_list.emplace_back(
        m_allocator->make_allocated<function_expression>(_expr,
                                                         _hand_ownership));
  }

  wn::containers::deque<wn::memory::allocated_ptr<function_expression>>
      &get_expressions() {
    return (m_expression_list);
  }

 private:
  wn::containers::deque<wn::memory::allocated_ptr<function_expression>>
      m_expression_list;
};

class function_call_expression : public post_expression {
 public:
  function_call_expression(wn::memory::allocator *_allocator)
      : post_expression(_allocator) {}
  function_call_expression(wn::memory::allocator *_allocator, arg_list *_list)
      : post_expression(_allocator), m_args(_list) {}

 private:
  wn::memory::allocated_ptr<arg_list> m_args;
};

class declaration : public instruction {
 public:
  declaration(wn::memory::allocator *_allocator, type_node *_type,
              const char *_name)
      : instruction(_allocator),
        m_type(memory::default_allocated_ptr(m_allocator, _type)),
        m_name(_name, _allocator),
        m_sized_array_initializers(_allocator),
        m_unsized_array_initializers(0),
        m_init_assign(wn_false) {}
  const wn_char *get_name() { return (m_name.c_str()); }

  void add_expression_initializer(expression *_expr, bool _assign = false) {
    m_expression = memory::default_allocated_ptr(m_allocator, _expr);
    m_init_assign = _assign;
  }

  void add_sized_array_initializer(expression *_expr) {
    m_sized_array_initializers.emplace_back(
        memory::default_allocated_ptr(m_allocator, _expr));
  }

  void add_unsized_array_intitializer() { m_unsized_array_initializers += 1; }
  expression *GetExpression() { return m_expression.get(); }

 private:
  wn::memory::allocated_ptr<type_node> m_type;
  containers::string m_name;
  wn::memory::allocated_ptr<expression> m_expression;
  wn::memory::allocated_ptr<type_node> m_scalar_type;
  wn::containers::deque<wn::memory::allocated_ptr<expression>>
      m_sized_array_initializers;
  wn_size_t m_unsized_array_initializers;
  wn_bool m_init_assign;
};

class function;
class struct_definition : public node {
 public:
  struct_definition(wn::memory::allocator *_allocator, const char *_name,
                    bool _is_class = false,
                    const char *_parent_type = wn_nullptr)
      : node(_allocator),
        m_name(_name, _allocator),
        m_parent_name(_allocator),
        m_is_class(_is_class),
        m_struct_members(_allocator),
        m_struct_functions(_allocator) {
    if (_parent_type) {
      m_parent_name = _parent_type;
    }
  }

  void add_struct_elem(declaration *_decl) {
    m_struct_members.emplace_back(
        memory::default_allocated_ptr(m_allocator, _decl));
  }

  void add_function(function *_func) {
    m_struct_functions.emplace_back(
        memory::default_allocated_ptr(m_allocator, _func));
  }

  const wn_char *get_name() const { return (m_name.c_str()); }

 private:
  containers::string m_name;
  containers::string m_parent_name;
  bool m_is_class;
  wn::containers::deque<wn::memory::allocated_ptr<declaration>>
      m_struct_members;
  wn::containers::deque<wn::memory::allocated_ptr<function>> m_struct_functions;
};

class decl_list : public node {
 public:
  decl_list(wn::memory::allocator *_allocator, declaration *_decl)
      : node(_allocator), m_declarations(_allocator) {
    m_declarations.emplace_back(
        memory::default_allocated_ptr(m_allocator, _decl));
  }
  void add_declaration(declaration *_decl) {
    m_declarations.emplace_back(
        memory::default_allocated_ptr(m_allocator, _decl));
  }

  wn_void get_declarations(
      const wn::containers::deque<wn::memory::allocated_ptr<declaration>> *
          &_out_declaration) const {
    _out_declaration = &m_declarations;
  }

 private:
  wn::containers::deque<wn::memory::allocated_ptr<declaration>> m_declarations;
};

class function : public node {
 public:
  function(wn::memory::allocator *_allocator, declaration *_decl,
           decl_list *_params, instruction_list *_body)
      : node(_allocator),
        m_declaration(memory::default_allocated_ptr(m_allocator, _decl)),
        m_parameters(memory::default_allocated_ptr(m_allocator, _params)),
        m_body(memory::default_allocated_ptr(m_allocator, _body)),
        m_is_override(wn_false),
        m_is_virtual(wn_false) {}
  void set_is_virtual(bool is_virtual) { m_is_virtual = is_virtual; }
  void set_is_override(bool is_override) { m_is_virtual = is_override; }

 private:
  wn::memory::allocated_ptr<declaration> m_declaration;
  wn::memory::allocated_ptr<decl_list> m_parameters;
  wn::memory::allocated_ptr<instruction_list> m_body;
  bool m_is_override;
  bool m_is_virtual;
};

class lvalue : public node {
 public:
  lvalue(wn::memory::allocator *_allocator, expression *_expr)
      : node(_allocator), m_expression(_expr) {}
  wn_bool required_use() { return m_expression->required_use(); }

 private:
  wn::memory::allocated_ptr<expression> m_expression;
};

class assignment_instruction : public instruction {
 public:
  assignment_instruction(wn::memory::allocator *_allocator, lvalue *_lvalue)
      : instruction(_allocator),
        m_lvalue(memory::default_allocated_ptr(m_allocator, _lvalue)),
        m_assign_type(assign_type::max) {}

  void add_value(assign_type _type, expression *_value) {
    _type = _type;
    m_assign_expression.reset(_value);
  }

 private:
  assign_type m_assign_type;
  wn::memory::allocated_ptr<lvalue> m_lvalue;
  wn::memory::allocated_ptr<expression> m_assign_expression;
};

class do_instruction : public instruction {
 public:
  do_instruction(wn::memory::allocator *_allocator, expression *_cond,
                 instruction_list *_body)
      : instruction(_allocator),
        m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
        m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

 private:
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<instruction_list> m_body;
};

class else_if_instruction : public instruction {
 public:
  else_if_instruction(wn::memory::allocator *_allocator, expression *_cond,
                      instruction_list *_body)
      : instruction(_allocator),
        m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
        m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

 private:
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<instruction_list> m_body;
};

class if_instruction : public instruction {
 public:
  if_instruction(wn::memory::allocator *_allocator, expression *_cond,
                 instruction_list *_body)
      : instruction(_allocator),
        m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
        m_body(memory::default_allocated_ptr(m_allocator, _body)),
        m_else_if_nodes(_allocator) {}
  void add_else_if(else_if_instruction *_elseif) {
    m_else_if_nodes.emplace_back(_elseif);
  }
  void add_else(instruction_list *_else) { m_else.reset(_else); }

 private:
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<instruction_list> m_else;
  wn::memory::allocated_ptr<instruction_list> m_body;
  wn::containers::deque<wn::memory::allocated_ptr<else_if_instruction>>
      m_else_if_nodes;
};

class for_instruction : public instruction {
 public:
  for_instruction(wn::memory::allocator *_allocator)
      : instruction(_allocator) {}
  void add_initializer(instruction *_init) {
    m_initializer = memory::default_allocated_ptr(m_allocator, _init);
  }
  void add_condition(expression *_cond) {
    m_condition = memory::default_allocated_ptr(m_allocator, _cond);
  }
  void add_post_op(instruction *_inst) {
    m_post_op = memory::default_allocated_ptr(m_allocator, _inst);
  }
  void add_body(instruction_list *_body) {
    m_body = memory::default_allocated_ptr(m_allocator, _body);
  }

 private:
  wn::memory::allocated_ptr<instruction> m_initializer;
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<instruction> m_post_op;
  wn::memory::allocated_ptr<instruction_list> m_body;
};

class return_instruction : public instruction {
 public:
  return_instruction(wn::memory::allocator *_allocator, expression *_expr,
                     bool _change_ownership = false)
      : instruction(_allocator),
        m_expression(memory::default_allocated_ptr(m_allocator, _expr)),
        m_change_ownership(_change_ownership) {}
  return_instruction(wn::memory::allocator *_allocator)
      : instruction(_allocator) {
    m_change_ownership = wn_false;
  }

 private:
  wn::memory::allocated_ptr<expression> m_expression;
  wn_bool m_change_ownership;
};

class while_instruction : public instruction {
 public:
  while_instruction(wn::memory::allocator *_allocator, expression *_cond,
                    instruction_list *_body)
      : instruction(_allocator),
        m_condition(memory::default_allocated_ptr(m_allocator, _cond)),
        m_body(memory::default_allocated_ptr(m_allocator, _body)) {}

 private:
  wn::memory::allocated_ptr<expression> m_condition;
  wn::memory::allocated_ptr<instruction_list> m_body;
};

class script_file : public node {
 public:
  script_file(wn::memory::allocator *_allocator)
      : node(_allocator),
        m_functions(_allocator),
        m_structs(_allocator),
        m_includes(_allocator) {}
  void add_function(function *_node) {
    m_functions.emplace_back(memory::default_allocated_ptr(m_allocator, _node));
  }

  void add_struct(struct_definition *_node) {
    m_structs.emplace_back(memory::default_allocated_ptr(m_allocator, _node));
  }

  void add_include(const char *_node) {
    m_includes.emplace_back(_node, m_allocator);
  }

 private:
  wn::containers::deque<wn::memory::allocated_ptr<function>> m_functions;
  wn::containers::deque<wn::memory::allocated_ptr<struct_definition>> m_structs;
  wn::containers::deque<containers::string> m_includes;
};
}  // namespace scripting
}  // namespace wn
#endif  //__WN_NODE_TYPES_H__
