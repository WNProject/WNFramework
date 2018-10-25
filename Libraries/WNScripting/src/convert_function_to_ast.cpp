// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

memory::unique_ptr<ast_function>
parse_ast_convertor::convertor_context::pre_resolve_function(
    const function* _function, const ast_type* _implicit_this) {
  memory::unique_ptr<ast_function> function =
      memory::make_unique<ast_function>(m_allocator, _function);

  const ast_type* ret_ty = resolve_type(_function->get_signature()->get_type());

  function->m_return_type = ret_ty;
  if (!function->m_return_type) {
    return nullptr;
  }
  function->m_name =
      _function->get_signature()->get_name().to_string(m_allocator);

  containers::string mangled_name(m_allocator);
  containers::dynamic_array<const ast_type*> types(m_allocator);
  types.push_back(ret_ty);
  if (!_implicit_this) {
    auto it = m_named_functions.find(function->m_name);
    if (it == m_named_functions.end()) {
      it = m_named_functions
               .insert(core::make_pair(function->m_name,
                   containers::deque<const ast_function*>(m_allocator)))
               .first;
    }
    it->second.push_back(function.get());
  }

  containers::deque<ast_function::parameter>& function_params =
      function->initialized_parameters(m_allocator);
  if (_implicit_this) {
    types.push_back(_implicit_this);
    function_params.push_back(ast_function::parameter{
        containers::string(m_allocator, "_this"), _implicit_this});
  }

  if (_function->get_parameters() &&
      _function->get_parameters()->get_parameters().size()) {
    for (auto& param : _function->get_parameters()->get_parameters()) {
      const ast_type* param_type = resolve_type(param->get_type());
      if (!param_type) {
        return nullptr;
      }
      types.push_back(param_type);
      function_params.push_back(ast_function::parameter{
          containers::string(m_allocator, param->get_name()), param_type});
    }
  }

  if (ret_ty->m_pass_by_reference) {
    function_params.push_back(ast_function::parameter{
        containers::string(m_allocator, "_return"), ret_ty});
  }

  function->m_is_virtual = _function->is_virtual();
  function->m_is_override = _function->is_override();
  function->m_is_member_function = _implicit_this != nullptr;
  function->calculate_mangled_name(m_allocator);
  function->m_function_pointer_type = m_type_manager->resolve_function_ptr_type(
      core::move(types), &m_used_types);
  return function;
}

bool parse_ast_convertor::convertor_context::resolve_function(
    const function* _function, ast_function* _resolved_function) {
  ast_function* function = _resolved_function;

  if (!_function->get_body()) {
    return true;
  }

  memory::unique_ptr<ast_scope_block> scope =
      memory::make_unique<ast_scope_block>(m_allocator, _function);
  push_scope(scope.get());
  m_current_function = function;

  if (function->m_return_type != m_type_manager->void_t(&m_used_types) &&
      !function->m_return_type->m_pass_by_reference) {
    memory::unique_ptr<ast_declaration> return_decl;

    return_decl = make_temp_declaration(_function,
        containers::string(m_allocator, "_return"), function->m_return_type);
    m_return_decl = return_decl.get();
    scope->initialized_declarations(m_allocator).push_back(return_decl.get());
    m_current_statements->push_back(core::move(return_decl));
    m_return_parameter = nullptr;
  } else if (function->m_return_type->m_pass_by_reference) {
    m_return_parameter =
        &function->m_parameters[function->m_parameters.size() - 1];
    m_return_decl = nullptr;

  } else {
    m_return_decl = nullptr;
  }

  for (auto& instruction : _function->get_body()->get_instructions()) {
    if (!resolve_statement(instruction.get())) {
      return false;
    }
  }

  if (!scope->m_returns) {
    if (function->m_return_type == m_type_manager->void_t(&m_used_types)) {
      m_current_statements->push_back(
          memory::make_unique<ast_return_instruction>(m_allocator, _function));
    } else {
      _function->log_line(m_log, logging::log_level::error);
      m_log->log_error("Error function must return");
      return false;
    }
  }
  pop_scope();

  function->m_scope = core::move(scope);

  return true;
}

}  // namespace scripting
}  // namespace wn