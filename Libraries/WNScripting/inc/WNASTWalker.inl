// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_WALKER_INL__
#define __WN_SCRIPTING_AST_WALKER_INL__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

template <typename T, bool Const>
void ast_walker<T, Const>::walk_script_file(script_file_type _file) {
  _file->walk_children(
      walk_scope(&ast_walker<T, Const>::enter_scope_block, this),
      walk_scope(&ast_walker<T, Const>::leave_scope_block, this),
      walk_ftype<instruction_type>(
          &ast_walker<T, Const>::walk_instruction, this),
      get_expr()(this),
      walk_ftype<function_type>(&ast_walker<T, Const>::walk_function, this),
      walk_ftype<struct_definition_type>(
          &ast_walker<T, Const>::walk_struct_definition, this));
  m_walker->walk_script_file(_file);
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_parameter(parameter_type _parameter) {
  _parameter->walk_children(
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this));
  m_walker->walk_parameter(_parameter);
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_struct_definition(struct_definition_type _def) {
  _def->walk_children(
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this));
  m_walker->walk_struct_definition(_def);
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_type(type_type _type) {
  m_walker->walk_type(_type);
}

template <typename T, bool Const>
void ast_walker<T, Const>::enter_scope_block() {
  m_walker->enter_scope_block();
}

template <typename T, bool Const>
void ast_walker<T, Const>::leave_scope_block() {
  m_walker->leave_scope_block();
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_function(function_type _function) {
  _function->walk_children(
      walk_scope(&ast_walker<T, Const>::enter_scope_block, this),
      walk_scope(&ast_walker<T, Const>::leave_scope_block, this),
      walk_ftype<parameter_type>(&ast_walker<T, Const>::walk_parameter, this),
      walk_ftype<instruction_list_type>(
          &ast_walker<T, Const>::walk_instruction_list, this),
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this));
  m_walker->walk_function(_function);
}

template <typename T, bool Const>
memory::unique_ptr<expression> ast_walker<T, Const>::walk_mut_expression(
    expression* _expression) {
  _expression->walk_children(get_expr()(this),
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this));
  switch (_expression->get_node_type()) {
    case node_type::array_allocation_expression:
      return m_walker->walk_expression(
          cast_to<array_allocation_expression>(_expression));
    case node_type::binary_expression:
      return m_walker->walk_expression(cast_to<binary_expression>(_expression));
    case node_type::cast_expression:
      return m_walker->walk_expression(cast_to<cast_expression>(_expression));
    case node_type::cond_expression:
      return m_walker->walk_expression(cast_to<cond_expression>(_expression));
    case node_type::constant_expression:
      return m_walker->walk_expression(
          cast_to<constant_expression>(_expression));
    case node_type::id_expression:
      return m_walker->walk_expression(cast_to<id_expression>(_expression));
    case node_type::null_allocation_expression:
      return m_walker->walk_expression(
          cast_to<null_allocation_expression>(_expression));
    case node_type::array_access_expression:
      return m_walker->walk_expression(
          cast_to<array_access_expression>(_expression));
    case node_type::function_call_expression:
      return m_walker->walk_expression(
          cast_to<function_call_expression>(_expression));
    case node_type::member_access_expression:
      return m_walker->walk_expression(
          cast_to<member_access_expression>(_expression));
    case node_type::post_unary_expression:
      return m_walker->walk_expression(
          cast_to<post_unary_expression>(_expression));
    case node_type::short_circuit_expression:
      return m_walker->walk_expression(
          cast_to<short_circuit_expression>(_expression));
    case node_type::struct_allocation_expression:
      return m_walker->walk_expression(
          cast_to<struct_allocation_expression>(_expression));
    case node_type::unary_expression:
      return m_walker->walk_expression(cast_to<unary_expression>(_expression));
    case node_type::replaced_expression:
      // Intentionally do nothing for replaced expressions. Once replaced,
      // these should be completely transparent to anything walking the AST.
      return nullptr;
    default:
      WN_RELEASE_ASSERT_DESC(false,
          "You added a new expression"
          " type but did not handle it here");
      return nullptr;
  }
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_expression(const expression* _expression) {
  _expression->walk_children(get_expr()(this),
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this));
  switch (_expression->get_node_type()) {
    case node_type::array_allocation_expression:
      m_walker->walk_expression(
          cast_to<array_allocation_expression>(_expression));
      break;
    case node_type::binary_expression:
      m_walker->walk_expression(cast_to<binary_expression>(_expression));
      break;
    case node_type::cast_expression:
      m_walker->walk_expression(cast_to<cast_expression>(_expression));
      break;
    case node_type::cond_expression:
      m_walker->walk_expression(cast_to<cond_expression>(_expression));
      break;
    case node_type::constant_expression:
      m_walker->walk_expression(cast_to<constant_expression>(_expression));
      break;
    case node_type::id_expression:
      m_walker->walk_expression(cast_to<id_expression>(_expression));
      break;
    case node_type::null_allocation_expression:
      m_walker->walk_expression(
          cast_to<null_allocation_expression>(_expression));
      break;
    case node_type::array_access_expression:
      m_walker->walk_expression(cast_to<array_access_expression>(_expression));
      break;
    case node_type::function_call_expression:
      m_walker->walk_expression(cast_to<function_call_expression>(_expression));
      break;
    case node_type::member_access_expression:
      m_walker->walk_expression(cast_to<member_access_expression>(_expression));
      break;
    case node_type::post_unary_expression:
      m_walker->walk_expression(cast_to<post_unary_expression>(_expression));
      break;
    case node_type::short_circuit_expression:
      m_walker->walk_expression(cast_to<short_circuit_expression>(_expression));
      break;
    case node_type::struct_allocation_expression:
      m_walker->walk_expression(
          cast_to<struct_allocation_expression>(_expression));
      break;
    case node_type::unary_expression:
      m_walker->walk_expression(cast_to<unary_expression>(_expression));
      break;
    default:
      WN_DEBUG_ASSERT_DESC(false, "Invalid expression type");
      break;
  }
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_instruction_list(instruction_list_type _list) {
  _list->walk_children(walk_ftype<instruction_type>(
                           &ast_walker<T, Const>::walk_instruction, this),
      get_expr()(this),
      walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this),
      walk_ftype<instruction_list_type>(
                           &ast_walker<T, Const>::walk_instruction_list, this),
      walk_scope(&ast_walker<T, Const>::enter_scope_block, this),
      walk_scope(&ast_walker<T, Const>::leave_scope_block, this));
  m_walker->walk_instruction_list(_list);
}

template <typename T, bool Const>
void ast_walker<T, Const>::walk_instruction(instruction_type _instruction) {
  if (_instruction->get_node_type() != node_type::instruction_list) {
    _instruction->walk_children(
        walk_ftype<instruction_type>(
            &ast_walker<T, Const>::walk_instruction, this),
        get_expr()(this),
        walk_ftype<type_type>(&ast_walker<T, Const>::walk_type, this),
        walk_ftype<instruction_list_type>(
            &ast_walker<T, Const>::walk_instruction_list, this),
        walk_scope(&ast_walker<T, Const>::enter_scope_block, this),
        walk_scope(&ast_walker<T, Const>::leave_scope_block, this));
  }

  switch (_instruction->get_node_type()) {
    case node_type::assignment_instruction:
      return m_walker->walk_instruction(
          cast_to<assignment_instruction>(_instruction));
    case node_type::declaration:
      return m_walker->walk_instruction(cast_to<declaration>(_instruction));
    case node_type::do_instruction:
      return m_walker->walk_instruction(cast_to<do_instruction>(_instruction));
    case node_type::else_if_instruction:
      return m_walker->walk_instruction(
          cast_to<else_if_instruction>(_instruction));
    case node_type::expression_instruction:
      return m_walker->walk_instruction(
          cast_to<expression_instruction>(_instruction));
    case node_type::for_instruction:
      return m_walker->walk_instruction(cast_to<for_instruction>(_instruction));
    case node_type::if_instruction:
      return m_walker->walk_instruction(cast_to<if_instruction>(_instruction));
    case node_type::return_instruction:
      return m_walker->walk_instruction(
          cast_to<return_instruction>(_instruction));
    case node_type::while_instruction:
      return m_walker->walk_instruction(
          cast_to<while_instruction>(_instruction));
    case node_type::instruction_list:
      return walk_instruction_list(cast_to<instruction_list>(_instruction));
    default:
      WN_DEBUG_ASSERT_DESC(false, "Invalid instruction type");
  }
}

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_AST_WALKER_INL__
