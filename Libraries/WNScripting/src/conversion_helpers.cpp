// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/utilities.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::make_cast(
    memory::unique_ptr<ast_expression> from, const ast_type* _type) {
  if (from->m_type == _type) {
    return from;
  }

  auto ret = memory::make_unique<ast_cast_expression>(m_allocator, nullptr);

  ret->m_source_location = from->m_source_location;
  ret->m_base_expression = core::move(from);
  ret->m_type = _type;

  return core::move(ret);
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::make_implicit_cast(
    memory::unique_ptr<ast_expression> from, const ast_type* _type) {
  if (_type == from->m_type) {
    return from;
  }

  if (!from->m_type->can_implicitly_cast_to(_type)) {
    return from;
  }

  auto ret = memory::make_unique<ast_cast_expression>(m_allocator, nullptr);

  ret->m_source_location = from->m_source_location;
  ret->m_base_expression = core::move(from);
  ret->m_type = _type;

  return core::move(ret);
}
memory::unique_ptr<ast_statement>
parse_ast_convertor::convertor_context::evaluate_expression(
    memory::unique_ptr<ast_expression> _expr) {
  auto ret = memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);

  ret->m_source_location = _expr->m_source_location;
  ret->m_expr = core::move(_expr);

  return core::move(ret);
}

memory::unique_ptr<ast_function_call_expression>
parse_ast_convertor::convertor_context::call_function(const node* _base_node,
    const ast_function* _func,
    containers::contiguous_range<memory::unique_ptr<ast_expression>>
        _expressions) {
  use_function(_func);
  memory::unique_ptr<ast_function_call_expression> function_call =
      memory::make_unique<ast_function_call_expression>(
          m_allocator, _base_node);
  function_call->m_function = _func;

  WN_DEBUG_ASSERT(_expressions.size() == _func->m_parameters.size(),
      "Cannot create a function with an incorrect number of parameters");

  int i = 0;
  (void)i;

  for (auto& p : _expressions) {
    WN_DEBUG_ASSERT(_func->m_parameters[i++].m_type == p->m_type,
        "Trying to create function call with invalid type");

    function_call->initialized_parameters(m_allocator).push_back(core::move(p));
  }

  function_call->m_type = _func->m_return_type;

  return function_call;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::get_id(
    const node* _location, const containers::string& _name, bool _can_fail) {
  type_manager::named_constant _const;
  if (m_type_manager->get_named_constant(_name, &_const)) {
    return get_constant(_location, _const._type, _const._value);
  }

  memory::unique_ptr<ast_id> id_expr =
      memory::make_unique<ast_id>(m_allocator, _location);
  for (auto& scope : m_nested_scopes) {
    for (auto& decl : scope->m_declarations) {
      if (decl->m_name == _name) {
        id_expr->m_declaration = decl;
        id_expr->m_type = decl->m_type;

        return core::move(id_expr);
      }
    }
  }

  for (auto& parameter : m_current_function->m_parameters) {
    if (parameter.m_name == _name) {
      id_expr->m_function_parameter = &parameter;
      id_expr->m_type = parameter.m_type;

      return core::move(id_expr);
    }
  }

  if (m_current_function->m_is_member_function) {
    auto st =
        m_current_function->m_parameters[0].m_type->m_implicitly_contained_type;
    uint32_t num = 0;

    for (auto& m : st->m_structure_members) {
      if (m->m_name == _name) {
        auto mae = memory::make_unique<ast_member_access_expression>(
            m_allocator, _location);

        id_expr->m_function_parameter = &m_current_function->m_parameters[0];
        id_expr->m_type = m_current_function->m_parameters[0].m_type;

        mae->m_base_expression = core::move(id_expr);
        mae->m_member_offset = num;
        mae->m_member_name = core::move(_name);
        mae->m_type = m->m_type;

        return core::move(mae);
      }

      num++;
    }

    for (auto& f : st->m_member_functions) {
      if (f->m_name == _name) {
        auto mae = memory::make_unique<ast_member_access_expression>(
            m_allocator, _location);
        id_expr->m_function_parameter = &m_current_function->m_parameters[0];
        id_expr->m_type = m_current_function->m_parameters[0].m_type;

        mae->m_base_expression = core::move(id_expr);
        mae->m_member_name = core::move(_name);
        mae->m_type = m_type_manager->function_t(&m_used_types);

        return core::move(mae);
      }
    }
  }

  if (m_named_functions.find(_name) != m_named_functions.end()) {
    id_expr->m_function_name = core::move(_name);
    id_expr->m_type = m_type_manager->function_t(&m_used_types);

    return core::move(id_expr);
  }

  if (!_can_fail) {
    _location->log_line(m_log, logging::log_level::error);

    m_log->log_error("Could not find function with name ", _name);
  }

  return nullptr;
}

memory::unique_ptr<ast_declaration>
parse_ast_convertor::convertor_context::make_temp_declaration(
    const node* _location, const containers::string& _name,
    const ast_type* _type) {
  memory::unique_ptr<ast_declaration> decl =
      memory::make_unique<ast_declaration>(m_allocator, _location);
  decl->m_name = containers::string(m_allocator, _name);
  decl->m_type = _type;
  return core::move(decl);
}

memory::unique_ptr<ast_id> parse_ast_convertor::convertor_context::id_to(
    const node* _location, ast_declaration* _decl) {
  memory::unique_ptr<ast_id> id =
      memory::make_unique<ast_id>(m_allocator, _location);
  id->m_declaration = _decl;
  id->m_type = _decl->m_type;
  return core::move(id);
}

}  // namespace scripting
}  // namespace wn
