// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_WALKER_INL__
#define __WN_SCRIPTING_AST_WALKER_INL__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::handle_walk_expression(const expression* _node) {
  switch (_node->get_node_type()) {
    case node_type::array_allocation_expression:
      return walk_array_allocation_expression(
          cast_to<const array_allocation_expression>(_node));
    case node_type::binary_expression:
      return walk_binary_expression(cast_to<const binary_expression>(_node));
    case node_type::cast_expression:
      return walk_cast_expression(cast_to<const cast_expression>(_node));
    case node_type::cond_expression:
      return walk_cond_expression(cast_to<const cond_expression>(_node));
    case node_type::constant_expression:
      return walk_constant_expression(
          cast_to<const constant_expression>(_node));
    case node_type::id_expression:
      return walk_id_expression(cast_to<const id_expression>(_node));
    case node_type::null_allocation_expression:
      return walk_null_allocation_expression(
          cast_to<const null_allocation_expression>(_node));
    case node_type::array_access_expression:
      return walk_array_access_expression(
          cast_to<const array_access_expression>(_node));
    case node_type::function_call_expression:
      return walk_function_call_expression(
          cast_to<const function_call_expression>(_node));
    case node_type::member_access_expression:
      return walk_member_access_expression(
          cast_to<const member_access_expression>(_node));
    case node_type::post_unary_expression:
      return walk_post_unary_expression(
          cast_to<const post_unary_expression>(_node));
    case node_type::short_circuit_expression:
      return walk_short_circuit_expression(
          cast_to<const short_circuit_expression>(_node));
    case node_type::struct_allocation_expression:
      return walk_struct_allocation_expression(
          cast_to<const struct_allocation_expression>(_node));
    case node_type::unary_expression:
      return walk_unary_expression(cast_to<const unary_expression>(_node));
    default:
      WN_DEBUG_ASSERT_DESC(wn_false, "Invalid expression type");
      return std::make_pair(expression_type(), 0);
      break;
  }
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_instruction(const instruction* _instruction) {
  switch (_instruction->get_node_type()) {
    case node_type::assignment_instruction:
      return walk_assignment_instruction(
          cast_to<const assignment_instruction>(_instruction));
      break;
    case node_type::declaration:
      return walk_declaration(cast_to<const declaration>(_instruction));
      break;
    case node_type::do_instruction:
      return walk_do_instruction(cast_to<const do_instruction>(_instruction));
      break;
    case node_type::else_if_instruction:
      WN_DEBUG_ASSERT_DESC(wn_false,
                           "You should not have an else_if by itself.");
      break;
    case node_type::for_instruction:
      return walk_for_instruction(cast_to<const for_instruction>(_instruction));
      break;
    case node_type::if_instruction:
      return walk_if_instruction(cast_to<const if_instruction>(_instruction));
      break;
    case node_type::return_instruction:
      return walk_return_instruction(
          cast_to<const return_instruction>(_instruction));
      break;
    case node_type::while_instruction:
      return walk_while_instruction(
          cast_to<const while_instruction>(_instruction));
      break;
    default:
      WN_DEBUG_ASSERT_DESC(wn_false, "Invalid instruction type");
  }
  return instruction_type();
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type ast_walker<
    T, Traits>::walk_assignment_instruction(const assignment_instruction*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return instruction_type();
}
template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_do_instruction(const do_instruction*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return instruction_type();
}
template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_for_instruction(const for_instruction*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return instruction_type();
}
template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_if_instruction(const if_instruction*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return instruction_type();
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_return_instruction(
    const return_instruction* _instruction) {
  if (_instruction->get_expression()) {
    expression_type expr;
    wn_uint32 type;
    std::tie(expr, type) =
        handle_walk_expression(_instruction->get_expression());

    if (type != current_function_return_type) {
      m_log->Log(WNLogging::eError, 0,
                 "Return value does not match function return type");
      _instruction->log_line(*m_log, WNLogging::eError);
      return instruction_type();
    }
    return m_consumer->walk_return_instruction(_instruction, expr);
  }

  return m_consumer->walk_return_instruction(_instruction);
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_while_instruction(const while_instruction*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return instruction_type();
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::type_type, wn_uint32>
ast_walker<T, Traits>::walk_type(const type* _type) {
  WN_RELEASE_ASSERT_DESC(
      _type->get_classification() != type_classification::custom_type,
      "Implement custom types");
  if (!m_validator->is_valid_type(_type->get_type_value())) {
    m_log->Log(WNLogging::eError, 0, "Unknown type: ",
               _type->custom_type_name().to_string(m_allocator).c_str());
    _type->log_line(*m_log, WNLogging::eError);
  }
  wn_uint32 registered_type =
      static_cast<uint32_t>(_type->get_classification());

  if (_type->get_classification() == type_classification::custom_type) {
    registered_type = m_validator->get_type(_type->custom_type_name());
  }
  return std::make_pair(m_consumer->walk_type(_type), registered_type);
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::instruction_type
ast_walker<T, Traits>::walk_declaration(const declaration* _declaration) {
  type_type type;
  std::tie(type, std::ignore) = walk_type(_declaration->get_type());
  WN_RELEASE_ASSERT_DESC(_declaration->get_expression() == wn_nullptr,
                         "Not implemented declaration expressions.");
  WN_RELEASE_ASSERT_DESC(_declaration->get_array_sizes().empty(),
                         "Not implemented sized arrays.");
  WN_RELEASE_ASSERT_DESC(
      _declaration->get_num_unsized_array_initializers() == 0,
      "Not implemented unsized array initializers.");
  return m_consumer->walk_declaration(_declaration, type);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::function_type_name, uint32_t>
ast_walker<T, Traits>::walk_function_name(const parameter* _parameter) {
  type_type type;
  wn_uint32 registered_type;
  std::tie(type, registered_type) = walk_type(_parameter->get_type());
  return std::make_pair(m_consumer->walk_function_name(_parameter, type),
                        registered_type);
}

template <typename T, typename Traits>
std::tuple<typename ast_walker<T, Traits>::parameter_type,
           typename ast_walker<T, Traits>::type_type, uint32_t>
ast_walker<T, Traits>::walk_parameter_name(const parameter* _parameter) {
  type_type type;
  uint32_t registered_type;
  std::tie(type, registered_type) = walk_type(_parameter->get_type());
  return std::make_tuple(m_consumer->walk_parameter_name(_parameter, type),
                         type, registered_type);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_array_allocation_expression(
    const array_allocation_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_binary_expression(const binary_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_cast_expression(const cast_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_cond_expression(const cond_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_constant_expression(
    const constant_expression* _const_expr) {
  bool bad_type = false;
  switch (_const_expr->get_classification()) {
    case wn::scripting::type_classification::int_type: {
      if (_const_expr->get_type_text().length() > 11) {
        bad_type = true;
        break;
      }
      long long val = atoll(_const_expr->get_type_text().c_str());
      if (val > WN_INT32_MAX || val < WN_INT32_MIN) {
        bad_type = true;
      }
    } break;
    default:
      WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
      break;
  }

  if (bad_type) {
    m_log->Log(WNLogging::eError, 0, "Invalid constant: \"",
               _const_expr->get_type_text().c_str(), "\"");
    _const_expr->log_line(*m_log, WNLogging::eError);
    return std::make_pair(expression_type(), 0);
  }

  wn_uint32 registered_type;
  type_type type;
  std::tie(type, registered_type) = walk_type(_const_expr->get_type());
  return std::make_pair(
      m_consumer->walk_expression(
          _const_expr, containers::contiguous_range<
                           containers::contiguous_range<expression_type>>(),
          type),
      registered_type);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_id_expression(const id_expression* _expression) {
  std::tuple<expression_type, type_type, wn_uint32>* p_t =
      m_scope_handler.get_named_value(_expression->get_name());
  if (!p_t) {
    m_log->Log(WNLogging::eError, 0, "Id ",
               _expression->get_name().to_string(m_allocator).c_str(),
               " does not exist");
    _expression->log_line(*m_log, WNLogging::eError);
    return std::make_pair(expression_type(), 0);
  } else {
    expression_type& t = std::get<0>(*p_t);
    type_type& type = std::get<1>(*p_t);
    containers::contiguous_range<expression_type> range(&t, &t + 1);
    return std::make_pair(
        m_consumer->walk_expression(
            _expression, containers::contiguous_range<
                             containers::contiguous_range<expression_type>>(
                             &range, &range + 1),
            type),
        std::get<2>(*p_t));
  }
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_null_allocation_expression(
    const null_allocation_expression* _expression) {
  m_consumer->walk_expression(
      _expression, containers::contiguous_range<
                       containers::contiguous_range<expression_type>>(),
      type_type());
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_array_access_expression(
    const array_access_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_function_call_expression(
    const function_call_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_member_access_expression(
    const member_access_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_post_unary_expression(
    const post_unary_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_short_circuit_expression(
    const short_circuit_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_struct_allocation_expression(
    const struct_allocation_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
std::pair<typename ast_walker<T, Traits>::expression_type, wn_uint32>
ast_walker<T, Traits>::walk_unary_expression(const unary_expression*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
  return std::make_pair(expression_type(), 0);
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::function_type
ast_walker<T, Traits>::walk_function(const function* _function) {
  function_type_name function_sig;
  std::tie(function_sig, current_function_return_type) =
      walk_function_name(_function->get_signature());
  size_t num_scopes = m_scope_handler.push_scope();
  containers::dynamic_array<std::pair<parameter_type, type_type>> parameters(
      m_allocator);
  containers::dynamic_array<wn_uint32> registered_types(m_allocator);
  if (_function->get_parameters()) {
    parameters.reserve(_function->get_parameters()->get_parameters().size());
    registered_types.reserve(
        _function->get_parameters()->get_parameters().size());
    for (const auto& parameter :
         _function->get_parameters()->get_parameters()) {
      parameter_type p;
      type_type t;
      wn_uint32 r;
      std::tie(p, t, r) = walk_parameter_name(parameter.get());
      parameters.push_back(std::make_pair(std::move(p), std::move(t)));
      registered_types.push_back(std::move(r));
    }
  }

  function_header_type header =
      m_consumer->walk_function_header(_function, function_sig, parameters);

  if (_function->get_parameters()) {
    size_t i = 0;
    for (auto& param : _function->get_parameters()->get_parameters()) {
      m_scope_handler.add_value_to_current_scope(
          param->get_name(),
          std::make_tuple(m_consumer->walk_parameter_instantiation(
                              param.get(), header, parameters[i], i),
                          std::get<1>(parameters[i]), registered_types[i]));
      ++i;
    }
  }

  containers::dynamic_array<instruction_type> body(m_allocator);
  body.reserve(_function->get_body()->get_instructions().size());
  for (const auto& instruction : _function->get_body()->get_instructions()) {
    body.emplace_back(walk_instruction(instruction.get()));
  }
  function_type ft = m_consumer->walk_function(_function, header, body);
  size_t new_scopes = m_scope_handler.pop_scope();
  (void)new_scopes;
  (void)num_scopes;
  WN_DEBUG_ASSERT_DESC(new_scopes == num_scopes,
                       "Something did not clean up scope blocks");
  return std::move(ft);
}

template <typename T, typename Traits>
typename ast_walker<T, Traits>::script_file_type
ast_walker<T, Traits>::walk_script_file(const script_file* _file) {
  WN_RELEASE_ASSERT_DESC(_file->get_includes().size() == 0,
                         "Not Implemented: Includes");
  WN_RELEASE_ASSERT_DESC(_file->get_structs().size() == 0,
                         "Not Implemented: Structs");
  containers::dynamic_array<function_type> functions;
  containers::dynamic_array<include_type> includes;
  containers::dynamic_array<struct_definition_type> structs;

  functions.reserve(_file->get_functions().size());
  for (auto& function : _file->get_functions()) {
    functions.push_back(walk_function(function.get()));
  }

  return m_consumer->walk_script_file(
      _file, containers::contiguous_range<function_type>(functions.data(),
                                                         functions.size()),
      containers::contiguous_range<include_type>(includes.data(),
                                                 includes.size()),
      containers::contiguous_range<struct_definition_type>(structs.data(),
                                                           structs.size()));
}

template <typename T>
wn_size_t scope_handler<T>::push_scope() {
  m_scopes.push_back(containers::deque<containers::string_view>());
  return m_scopes.size() - 1;
}

template <typename T>
wn_size_t scope_handler<T>::pop_scope() {
  WN_RELEASE_ASSERT_DESC(m_scopes.size() > 0, "Cannot pop an empty scope");
  for (auto& it : m_scopes.back()) {
    WN_DEBUG_ASSERT_DESC(
        m_scoped_values.find(it) != m_scoped_values.end(),
        "Could not find a value in the scoped value map that should be there");
    WN_DEBUG_ASSERT_DESC(
        m_scoped_values[it].size() > 0,
        "There should be a named value in the map but there is not");
    m_scoped_values[it].pop_back();
  }
  m_scopes.pop_back();
  return m_scopes.size();
}

template <typename T>
added_scope_result scope_handler<T>::add_value_to_current_scope(
    const containers::string_view& _name, T&& _value) {
  WN_RELEASE_ASSERT_DESC(m_scopes.size() > 0,
                         "Cannot add to a non-existent scope");
  added_scope_result success_return_value = added_scope_result::ok;
  auto it = m_scoped_values.find(_name);
  if (it != m_scoped_values.end() && !it->second.empty()) {
    // We have found at least one value with the same name, so either
    // we are hiding a previous value or we are aliasing something
    // in the same scope as us.
    if (std::find(m_scopes.back().begin(), m_scopes.back().end(), _name) !=
        m_scopes.back().end()) {
      return added_scope_result::already_in_this_scope;
    }
    success_return_value = added_scope_result::hides_higher_scope;
  } else {
    it = m_scoped_values.insert(_name, containers::deque<named_value_store<T>>(
                                           m_allocator))
             .first;
  }
  it->second.emplace_back(_name.to_string(m_allocator), std::move(_value));
  return success_return_value;
}

template <typename T>
T* scope_handler<T>::get_named_value(containers::string_view _name) {
  auto it = m_scoped_values.find(_name);
  if (it == m_scoped_values.end() || it->second.empty()) {
    return wn_nullptr;
  } else {
    return &it->second.back().m_parameter_value;
  }
}

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_AST_WALKER_INL__
