// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_WALKER_H__
#define __WN_SCRIPTING_AST_WALKER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

// ast_walker description.
// The ast_walker expects a class containing callbacks
// with at least the following signatures.
// Note for all types the pointer is first cast to
// the exact type of the node. This means that if you
// declare traits::expression_type walk_expression(binary_expression*,
//    contiguous_range<contiguous_range<traits::expression_type>>,
//    traits::type_type)
// it will be called instead of the generic one.

// void pre_walk_expression(expression*);
// traits::expression_type walk_expression(expression*,
//         contiguous_range<contiguous_range<traits::expression_type>>,
//         wn_uint32,
//         );

// void pre_walk_function(function*);
// traits::function_type walk_function(function*,
//    traits::declaration*,
//    contiguous_range<traits::declaration>,
//    contiguous_range<traits::instruction>);

// void pre_walk_assignment_instruction(assignment_instruction*);
// traits::assignment_instruction_type walk_assignment_instruction
//         (assignment_instruction*, traits::lvalue, traits::expression);

// void pre_walk_declaration(declaration*);
// traits::instruction_type walk_declaration(declaration*);

// void loop_instruction(instruction*)
// traits::instruction_type loop_instruction(instruction*,
//     traits::expression, // start of loop condition
//     traits::expression, // end of loop condition
//     contiguous_range<traits::instruction>, // instructions
//     traits::instruction, // pre-entry instruction
//     traits::instruction); // loop continuation instruction

// void pre_walk_if_instruction(if_instruction*)
// For each condition there is a matching block, there may be an extra
// block at the end, if there is a default case.
// traits::instruction_type walk_if_instruction(instruction*,
//     contiguous_range<traits::expression>, // conditions
//     contiguous_range<contiguous_range<traits::instruction>>) blocks

// void pre_walk_return_instruction(return_instruction*)
// traits::return_instruction_type  walk_return_instruction(
//          return_instruction*)
// traits::return_instruction_type  walk_return_instruction(
//          return_instruction*, traits::expression)

// void pre_walk_lvalue(lvalue*);
// traits::lvalue_type walk_lvalue(lvalue*, traits::expression*);

// void pre_walk_script_file(script_file*);
// traits::script_file_type walk_script_file(script_file*,
//                       contiguous_range<traits::function_type>,
//                       contiguous_range<traits::struct_definition_type>);

// void pre_walk_struct_definition(struct_definition*);
// traits::struct_definition_type walk_struct_definition(struct_definition*,
//    contiguous_range<traits::parameter_type>,
//    contiguous_range<traits::function>);

// void pre_walk_type(type*);
// traits::type_type walk_type(type*);

class default_ast_walker_traits {
  typedef expression* expression_type;
  typedef function* function_type;
  typedef void* function_header_type;
  typedef parameter* parameter_type;
  typedef instruction* instruction_type;
  typedef lvalue* lvalue_type;
  typedef script_file* script_file_type;
  typedef struct_definition* struct_definition_type;
  typedef type* type_type;
  typedef containers::string include_type;
};

template <typename T>
T* cast_to(const node* _node) {
  return static_cast<T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
}

template <typename T, typename Traits>
class ast_walker {
 public:
  // Convenience types.
  using expression_type = typename Traits::expression_type;
  using function_type = typename Traits::function_type;
  using function_header_type = typename Traits::function_header_type;
  using parameter_type = typename Traits::parameter_type;
  using instruction_type = typename Traits::instruction_type;
  using lvalue_type = typename Traits::lvalue_type;
  using script_file_type = typename Traits::script_file_type;
  using struct_definition_type = typename Traits::struct_definition_type;
  using type_type = typename Traits::type_type;
  using include_type = typename Traits::include_type;

  ast_walker(T* _consumer, WNLogging::WNLog* _log,
             scripting::type_validator* _validator,
             memory::allocator* _allocator)
      : m_consumer(_consumer),
        m_allocator(_allocator),
        m_log(_log),
        m_validator(_validator) {}

  type_type walk_type(const type* _type);

  instruction_type walk_instruction(const instruction* _instruction);
  instruction_type walk_assignment_instruction(const assignment_instruction*);
  instruction_type walk_do_instruction(const do_instruction*);
  instruction_type walk_for_instruction(const for_instruction*);
  instruction_type walk_if_instruction(const if_instruction*);
  instruction_type walk_return_instruction(
      const return_instruction* _instruction);
  instruction_type walk_while_instruction(const while_instruction*);
  instruction_type walk_declaration(const declaration* _declaration);

  expression_type handle_walk_expression(const expression* _node);
  expression_type walk_array_allocation_expression(
      const array_allocation_expression*);
  expression_type walk_binary_expression(const binary_expression*);
  expression_type walk_cast_expression(const cast_expression*);
  expression_type walk_cond_expression(const cond_expression*);
  expression_type walk_constant_expression(
      const constant_expression* _const_expr);
  expression_type walk_id_expression(const id_expression*);
  expression_type walk_null_allocation_expression(
      const null_allocation_expression* _expression);
  expression_type walk_array_access_expression(const array_access_expression*);
  expression_type walk_function_call_expression(
      const function_call_expression*);
  expression_type walk_member_access_expression(
      const member_access_expression*);
  expression_type walk_post_unary_expression(const post_unary_expression*);
  expression_type walk_short_circuit_expression(
      const short_circuit_expression*);
  expression_type walk_struct_allocation_expression(
      const struct_allocation_expression*);
  expression_type walk_unary_expression(const unary_expression*);

  parameter_type walk_parameter(const parameter* _parameter);
  function_type walk_function(const function* _function);
  script_file_type walk_script_file(const script_file* _file);

 private:
  T* m_consumer;
  scripting::type_validator* m_validator;
  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNASTWalker.inl"

#endif  //__WN_SCRIPTING_AST_WALKER_H__