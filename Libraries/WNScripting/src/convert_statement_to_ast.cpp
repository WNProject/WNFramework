// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNDeque.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

bool parse_ast_convertor::convertor_context::resolve_return(
    const return_instruction* _instruction) {
  memory::unique_ptr<ast_return_instruction> ret_expr =
      memory::make_unique<ast_return_instruction>(m_allocator, _instruction);

  m_nested_scopes.back()->m_returns = true;
  if (_instruction->get_expression() != nullptr) {
    memory::unique_ptr<ast_expression> expr =
        resolve_expression(_instruction->get_expression());
    if (!expr) {
      return false;
    }
    if (m_current_function->m_return_type->m_classification ==
            ast_type_classification::shared_reference ||
        m_current_function->m_return_type->m_classification ==
            ast_type_classification::actor_type) {
      if (expr->m_type == m_type_manager->nullptr_t(&m_used_types)) {
        expr = make_cast(core::move(expr), m_current_function->m_return_type);
      } else {
        auto etype = expr->m_type;
        auto const_null =
            memory::make_unique<ast_constant>(m_allocator, _instruction);
        const_null->m_string_value = containers::string(m_allocator, "");
        const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
        auto null_as_vptr = make_cast(
            core::move(const_null), m_type_manager->void_ptr_t(&m_used_types));

        containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
            m_allocator);
        params.push_back(core::move(null_as_vptr));
        params.push_back(core::move(make_cast(
            core::move(expr), m_type_manager->void_ptr_t(&m_used_types))));
        if (m_current_function->m_return_type->m_classification ==
            ast_type_classification::actor_type) {
          expr = make_cast(call_function(_instruction,
                               m_type_manager->assign_actor(&m_used_builtins),
                               core::move(params)),
              etype);
        } else {
          expr = make_cast(call_function(_instruction,
                               m_type_manager->assign_shared(&m_used_builtins),
                               core::move(params)),
              etype);
        }
      }
    }

    if (!expr->m_type->can_transparently_cast_to(
            m_current_function->m_return_type)) {
      _instruction->log_line(m_log, logging::log_level::error);
      m_log->log_error("Returned value must match the function");
      return false;
    }
    if (expr->m_type != m_current_function->m_return_type) {
      expr = make_cast(core::move(expr), m_current_function->m_return_type);
    }

    auto id_expr = memory::make_unique<ast_id>(m_allocator, _instruction);
    id_expr->m_declaration = m_return_decl;
    id_expr->m_function_parameter = m_return_parameter;
    id_expr->m_type = m_current_function->m_return_type;

    if (!id_expr->m_type->m_pass_by_reference) {
      ret_expr->m_return_expr = clone_ast_node(m_allocator, id_expr.get());
    }

    auto assign =
        memory::make_unique<ast_assignment>(m_allocator, _instruction);
    assign->m_lhs = core::move(id_expr);
    assign->m_rhs = core::move(expr);

    m_current_statements->push_back(core::move(assign));
    for (auto& s : m_temporary_cleanup) {
      m_current_statements->push_back(core::move(s));
    }
    m_temporary_cleanup.clear();
  } else if (m_return_decl || m_return_parameter) {
    _instruction->log_line(m_log, logging::log_level::error);
    m_log->log_error("Must return a value that matches the function");
  }

  // Append the correct instructions if this scope block has not been
  // prematurely terminated.
  clean_scopes();
  m_current_statements->push_back(core::move(ret_expr));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_if(
    const if_instruction* _if) {
  bool may_return = false;
  bool may_break = false;

  auto const_true = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_true->m_string_value = containers::string(m_allocator, "true");
  const_true->m_node_value.m_bool = true;
  const_true->m_type = m_type_manager->bool_t(&m_used_types);

  auto const_false = clone_ast_node(m_allocator, const_true.get());
  const_false->m_string_value = containers::string(m_allocator, "false");
  const_false->m_node_value.m_bool = false;

  memory::unique_ptr<ast_declaration> unentered =
      make_temp_declaration(_if, get_next_temporary_name("unentered"),
          m_type_manager->bool_t(&m_used_types));
  unentered->m_initializer = clone_ast_node(m_allocator, const_true.get());

  memory::unique_ptr<ast_declaration> enter = make_temp_declaration(_if,
      get_next_temporary_name("enter"), m_type_manager->bool_t(&m_used_types));
  enter->m_initializer = clone_ast_node(m_allocator, const_false.get());

  auto unentered_id = id_to(_if, unentered.get());
  auto enter_id = id_to(_if, enter.get());

  m_current_statements->push_back(core::move(unentered));
  m_current_statements->push_back(core::move(enter));

  //  bool unentered = true;
  //  bool enter = false;
  //     // Technically we can get away without this outer loop,
  //     //  but it maintains consistency
  //  if (unentered) {
  //    enter = <condition>
  //    if (enter) {
  //       do_stuff
  //    }
  //  }
  {
    memory::unique_ptr<ast_scope_block> m_scope =
        memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
    memory::unique_ptr<ast_if_block> m_if =
        memory::make_unique<ast_if_block>(m_allocator, _if->get_body());
    m_if->m_condition = clone_ast_node(m_allocator, unentered_id.get());
    auto& body = m_scope->initialized_statements(m_allocator);
    push_scope(m_scope.get());
    memory::unique_ptr<ast_expression> cond =
        resolve_expression(_if->get_condition());
    if (!cond) {
      return false;
    }

    auto assign =
        memory::make_unique<ast_assignment>(m_allocator, _if->get_condition());
    assign->m_lhs = clone_ast_node(m_allocator, enter_id.get());
    assign->m_rhs = core::move(cond);
    body.push_back(core::move(assign));
    for (auto& c : m_temporary_cleanup) {
      body.push_back(core::move(c));
    }
    m_temporary_cleanup.clear();

    {
      memory::unique_ptr<ast_scope_block> m_inner_scope =
          memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
      memory::unique_ptr<ast_if_block> m_inner_if =
          memory::make_unique<ast_if_block>(m_allocator, _if->get_body());
      m_inner_if->m_condition = clone_ast_node(m_allocator, enter_id.get());
      auto& inner_body = m_inner_scope->initialized_statements(m_allocator);
      push_scope(m_inner_scope.get());

      auto inner_assign = memory::make_unique<ast_assignment>(
          m_allocator, _if->get_condition());
      inner_assign->m_lhs = clone_ast_node(m_allocator, unentered_id.get());
      inner_assign->m_rhs = clone_ast_node(m_allocator, const_false.get());
      inner_body.push_back(core::move(inner_assign));
      for (auto& i : _if->get_body()->get_instructions()) {
        if (!resolve_statement(i.get())) {
          return false;
        }
      }
      pop_scope();

      may_return = m_inner_scope->m_returns;
      may_break = m_inner_scope->m_breaks;
      m_inner_if->m_body = core::move(m_inner_scope);
      body.push_back(core::move(m_inner_if));
    }
    pop_scope();
    m_if->m_body = core::move(m_scope);
    m_current_statements->push_back(core::move(m_if));
  }

  for (auto& i : _if->get_else_if_instructions()) {
    memory::unique_ptr<ast_scope_block> m_scope =
        memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
    memory::unique_ptr<ast_if_block> m_if =
        memory::make_unique<ast_if_block>(m_allocator, _if->get_body());
    m_if->m_condition = clone_ast_node(m_allocator, unentered_id.get());
    auto& body = m_scope->initialized_statements(m_allocator);
    push_scope(m_scope.get());
    memory::unique_ptr<ast_expression> cond =
        resolve_expression(i->get_condition());
    if (!cond) {
      return false;
    }

    auto assign =
        memory::make_unique<ast_assignment>(m_allocator, _if->get_condition());
    assign->m_lhs = clone_ast_node(m_allocator, enter_id.get());
    assign->m_rhs = core::move(cond);
    body.push_back(core::move(assign));
    for (auto& c : m_temporary_cleanup) {
      body.push_back(core::move(c));
    }
    m_temporary_cleanup.clear();

    {
      memory::unique_ptr<ast_scope_block> m_inner_scope =
          memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
      memory::unique_ptr<ast_if_block> m_inner_if =
          memory::make_unique<ast_if_block>(m_allocator, _if->get_body());
      m_inner_if->m_condition = clone_ast_node(m_allocator, enter_id.get());
      auto& inner_body = m_inner_scope->initialized_statements(m_allocator);
      push_scope(m_inner_scope.get());

      auto inner_assign = memory::make_unique<ast_assignment>(
          m_allocator, _if->get_condition());
      inner_assign->m_lhs = clone_ast_node(m_allocator, unentered_id.get());
      inner_assign->m_rhs = clone_ast_node(m_allocator, const_false.get());
      inner_body.push_back(core::move(inner_assign));
      for (auto& in : i->get_body()->get_instructions()) {
        if (!resolve_statement(in.get())) {
          return false;
        }
      }
      pop_scope();
      may_return &= m_inner_scope->m_returns;
      may_break &= m_inner_scope->m_breaks;
      m_inner_if->m_body = core::move(m_inner_scope);
      body.push_back(core::move(m_inner_if));
    }
    pop_scope();
    m_if->m_body = core::move(m_scope);
    m_current_statements->push_back(core::move(m_if));
  }

  if (_if->get_else()) {
    memory::unique_ptr<ast_scope_block> m_scope =
        memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
    memory::unique_ptr<ast_if_block> m_if =
        memory::make_unique<ast_if_block>(m_allocator, _if->get_body());
    m_if->m_condition = clone_ast_node(m_allocator, unentered_id.get());
    m_scope->initialized_statements(m_allocator);
    push_scope(m_scope.get());

    for (auto& i : _if->get_else()->get_instructions()) {
      if (!resolve_statement(i.get())) {
        return false;
      }
    }
    pop_scope();
    may_return &= m_scope->m_returns;
    may_break &= m_scope->m_breaks;
    m_if->m_body = core::move(m_scope);
    m_if->m_returns = may_return;
    m_current_statements->push_back(core::move(m_if));
  } else {
    may_return = false;
    may_break = false;
  }
  m_nested_scopes.back()->m_returns = may_return;
  m_nested_scopes.back()->m_breaks = may_break;
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_declaration(
    const declaration* _declaration) {
  const ast_type* type = resolve_type(_declaration->get_type());
  if (!type) {
    return false;
  }

  if (get_id(_declaration, _declaration->get_name().to_string(m_allocator),
          true)) {
    _declaration->log_line(m_log, logging::log_level::error);
    m_log->log_error("Error: ", _declaration->get_name(),
        " hides previous member with the same name");
    return false;
  }

  const bool is_object =
      type->m_classification == ast_type_classification::reference;

  // If this is a raw struct, we can forgo creating an extra variable.
  if (is_object && !type->m_implicitly_contained_type->m_struct_is_class &&
      type->m_implicitly_contained_type->m_classification !=
          ast_type_classification::extern_type) {
    type = type->m_implicitly_contained_type;
  }
  bool is_actor = type->m_classification == ast_type_classification::actor_type;

  const bool is_shared =
      type->m_classification == ast_type_classification::shared_reference;

  memory::unique_ptr<ast_declaration> decl = make_temp_declaration(
      _declaration, _declaration->get_name().to_string(m_allocator), type);

  if (!_declaration->get_expression()) {
    m_current_statements->push_back(core::move(decl));
    return true;
  }
  if (_declaration->is_synchronized()) {
    decl->m_is_synchronized = true;
  }

  const bool is_obj_init = (is_object || is_shared || is_actor) &&
                           (_declaration->get_expression()->get_node_type() ==
                               node_type::struct_allocation_expression);

  const bool is_array =
      type->m_classification == ast_type_classification::static_array ||
      type->m_classification == ast_type_classification::runtime_array;

  const bool is_array_init =
      is_array && (_declaration->get_expression()->get_node_type() ==
                      node_type::array_allocation_expression);

  if (((is_object || is_shared || is_actor) && is_obj_init) ||
      (is_array && is_array_init)) {
    auto id_expr = memory::make_unique<ast_id>(m_allocator, _declaration);
    id_expr->m_declaration = decl.get();
    id_expr->m_type = decl->m_type;

    m_assigned_declaration = core::move(id_expr);

    m_nested_scopes.back()
        ->initialized_declarations(m_allocator)
        .push_back(decl.get());
    m_current_statements->push_back(core::move(decl));
    // We don't actually expect something to get returned here.
    // Normally we take a nullptr return as an error.
    // TODO(awoloszyn): Figure out what to do here on error.
    m_declaration_succeeded = false;
    resolve_expression(_declaration->get_expression());

    return m_declaration_succeeded;
  }

  memory::unique_ptr<ast_expression> init;

  if (!(init = resolve_expression(_declaration->get_expression()))) {
    return false;
  }
  bool is_null_assign =
      init->m_type == m_type_manager->nullptr_t(&m_used_types);

  if (type->m_classification == ast_type_classification::slice_type &&
      is_null_assign) {
    init = make_implicit_cast(core::move(init), type);
  }

  if (init && init->m_type != type &&
      !init->m_type->can_implicitly_cast_to(type)) {
    _declaration->log_line(m_log, logging::log_level::error);
    m_log->log_error("You must initialize a type with the RHS type");
    return false;
  }

  decl->m_initializer = make_cast(core::move(init), type);
  decl->m_is_synchronized = _declaration->is_synchronized();
  decl->m_name = _declaration->get_name().to_string(m_allocator);
  decl->m_type = type;
  m_nested_scopes.back()
      ->initialized_declarations(m_allocator)
      .push_back(decl.get());

  if (decl) {
    m_current_statements->push_back(core::move(decl));
  }

  return true;
}

bool parse_ast_convertor::convertor_context::resolve_evaluate_expression(
    const expression_instruction* _inst) {
  auto expr = memory::make_unique<ast_evaluate_expression>(m_allocator, _inst);

  expr->m_expr = resolve_expression(_inst->get_expression());
  if (!expr->m_expr) {
    return false;
  }
  m_current_statements->push_back(core::move(expr));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_assignment(
    const assignment_instruction* _assign) {
  auto lhs = resolve_expression(_assign->get_lvalue()->get_expression());
  if (!lhs) {
    return false;
  }
  auto rhs = resolve_expression(_assign->get_expression());
  if (!rhs) {
    return false;
  }

  if ((lhs->m_type->m_classification ==
              ast_type_classification::shared_reference ||
          lhs->m_type->m_classification ==
              ast_type_classification::actor_type) &&
      rhs->m_type == m_type_manager->nullptr_t(&m_used_types)) {
    rhs = make_cast(core::move(rhs), lhs->m_type);
  }

  if (lhs->m_type->m_classification == ast_type_classification::struct_type &&
      rhs->m_type->m_classification == ast_type_classification::reference) {
    auto type = rhs->m_type->m_implicitly_contained_type;
    rhs = make_cast(core::move(rhs), type);
  }

  if (lhs->m_type->m_classification == ast_type_classification::reference &&
      rhs->m_type->m_classification == ast_type_classification::struct_type) {
    auto type = m_type_manager->get_reference_of(
        rhs->m_type, ast_type_classification::reference, &m_used_types);
    rhs = make_cast(core::move(rhs), type);
  }

  if (lhs->m_type != rhs->m_type) {
    _assign->log_line(m_log, logging::log_level::error);
    m_log->log_error("Attempting to assign unequal types");
    return false;
  }

  if (lhs->m_type->m_classification ==
          ast_type_classification::shared_reference ||
      lhs->m_type->m_classification == ast_type_classification::actor_type) {
    containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
        m_allocator);
    params.push_back(make_cast(clone_ast_node(m_allocator, lhs.get()),
        m_type_manager->void_ptr_t(&m_used_types)));
    params.push_back(
        make_cast(core::move(rhs), m_type_manager->void_ptr_t(&m_used_types)));
    if (lhs->m_type->m_classification == ast_type_classification::actor_type) {
      rhs = make_cast(
          call_function(_assign, m_type_manager->assign_actor(&m_used_builtins),
              core::move(params)),
          lhs->m_type);
    } else {
      rhs = make_cast(call_function(_assign,
                          m_type_manager->assign_shared(&m_used_builtins),
                          core::move(params)),
          lhs->m_type);
    }
  }

  memory::unique_ptr<ast_assignment> assign =
      memory::make_unique<ast_assignment>(m_allocator, _assign);
  assign->m_lhs = core::move(lhs);

  switch (_assign->get_assignment_type()) {
    case assign_type::equal:
      if (assign->m_lhs->m_type->m_classification ==
              ast_type_classification::reference ||
          assign->m_lhs->m_type->m_classification ==
              ast_type_classification::struct_type) {
        const ast_type* struct_type =
            assign->m_lhs->m_type->m_classification ==
                    ast_type_classification::struct_type
                ? assign->m_lhs->m_type
                : assign->m_lhs->m_type->m_implicitly_contained_type;

        if (struct_type->m_struct_is_class) {
          _assign->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot assign class types.");
          return false;
        }
        if (struct_type->m_assignment) {
          auto ref_type = m_type_manager->get_reference_of(
              struct_type, ast_type_classification::reference, &m_used_types);
          containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
              m_allocator);
          params.push_back(make_cast(core::move(assign->m_lhs), ref_type));
          params.push_back(make_cast(core::move(rhs), ref_type));

          auto const_false =
              memory::make_unique<ast_constant>(m_allocator, _assign);
          const_false->m_type = m_type_manager->bool_t(&m_used_types);
          const_false->m_string_value =
              containers::string(m_allocator, "false");
          const_false->m_node_value.m_bool = false;

          params.push_back(core::move(const_false));
          m_current_statements->push_back(evaluate_expression(call_function(
              _assign, struct_type->m_assignment, core::move(params))));
          return true;
        } else {
          // If these are non-assigned struct types, then do the assignment
          // by value
          assign->m_lhs = make_cast(core::move(assign->m_lhs), struct_type);
          rhs = make_cast(core::move(rhs), struct_type);
        }
      }
      assign->m_rhs = core::move(rhs);

      break;
    case assign_type::add_equal:
    case assign_type::div_equal:
    case assign_type::mod_equal:
    case assign_type::mult_equal:
    case assign_type::sub_equal: {
      if (!assign->m_lhs->m_type->m_is_arithmetic_type) {
        _assign->log_line(m_log, logging::log_level::error);
        m_log->log_error(
            "Attempting to perform arithmetic on non-arithmetic types");
        return false;
      }
      auto arithmetic =
          memory::make_unique<ast_binary_expression>(m_allocator, _assign);
      arithmetic->m_lhs = clone_ast_node(m_allocator, assign->m_lhs.get());
      arithmetic->m_rhs = core::move(rhs);
      switch (_assign->get_assignment_type()) {
        case assign_type::add_equal:
          arithmetic->m_binary_type = ast_binary_type::add;
          break;
        case assign_type::div_equal:
          arithmetic->m_binary_type = ast_binary_type::div;
          break;
        case assign_type::mod_equal:
          arithmetic->m_binary_type = ast_binary_type::mod;
          break;
        case assign_type::mult_equal:
          arithmetic->m_binary_type = ast_binary_type::mult;
          break;
        case assign_type::sub_equal:
          arithmetic->m_binary_type = ast_binary_type::sub;
          break;
        default:
          WN_RELEASE_ASSERT(false, "Unhandled statement type");
      }
      transfer_temporaries(arithmetic.get(), arithmetic->m_lhs.get());
      transfer_temporaries(arithmetic.get(), arithmetic->m_rhs.get());
      assign->m_rhs = core::move(arithmetic);
    } break;
    default:
      WN_RELEASE_ASSERT(false, "Unimplemented assignment type");
  }
  m_current_statements->push_back(core::move(assign));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_while_loop(
    const while_instruction* _inst) {
  auto expr = memory::make_unique<ast_loop>(m_allocator, _inst);
  ast_loop* old_loop = m_current_loop;

  {
    memory::unique_ptr<ast_scope_block> scope =
        memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
    push_scope(scope.get());

    auto pre_cond_expr = resolve_expression(_inst->get_condition());
    if (!pre_cond_expr) {
      return false;
    }
    auto td = make_temp_declaration(_inst, get_next_temporary_name(),
        m_type_manager->bool_t(&m_used_types));
    auto* decl = td.get();
    td->m_initializer = core::move(pre_cond_expr);
    m_current_statements->push_back(core::move(td));

    for (auto& tc : m_temporary_cleanup) {
      scope->initialized_statements(m_allocator).push_back(core::move(tc));
    }

    auto st = memory::make_unique<ast_builtin_statement>(m_allocator, nullptr);
    st->m_builtin_type = builtin_statement_type::break_if_not;
    st->m_break_loop = expr.get();
    auto id = memory::make_unique<ast_id>(m_allocator, _inst);
    id->m_declaration = decl;
    id->m_type = decl->m_type;

    st->initialized_expressions(m_allocator).push_back(core::move(id));
    scope->initialized_statements(m_allocator).push_back(core::move(st));

    m_temporary_cleanup.clear();
    pop_scope();
    expr->m_condition_scope_block = core::move(scope);
  }

  memory::unique_ptr<ast_scope_block> scope =
      memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
  push_scope(scope.get());
  expr->m_body = core::move(scope);

  m_current_loop = expr.get();

  for (auto& i : _inst->get_body()->get_instructions()) {
    if (!resolve_statement(i.get())) {
      return false;
    }
  }
  pop_scope();

  m_current_loop = old_loop;
  m_current_statements->push_back(core::move(expr));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_for_loop(
    const for_instruction* _inst) {
  memory::unique_ptr<ast_scope_block> outer_scope;
  auto loop = memory::make_unique<ast_loop>(m_allocator, _inst);
  ast_loop* old_loop = m_current_loop;
  m_current_loop = nullptr;
  if (_inst->get_initializer()) {
    outer_scope = memory::make_unique<ast_scope_block>(m_allocator, _inst);
    push_scope(outer_scope.get());
    if (!resolve_statement(_inst->get_initializer())) {
      return false;
    }
  }

  if (_inst->get_condition()) {
    memory::unique_ptr<ast_scope_block> scope =
        memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
    push_scope(scope.get());

    auto pre_cond_expr = resolve_expression(_inst->get_condition());
    if (!pre_cond_expr) {
      return false;
    }
    auto td = make_temp_declaration(_inst, get_next_temporary_name(),
        m_type_manager->bool_t(&m_used_types));
    auto* decl = td.get();
    td->m_initializer = core::move(pre_cond_expr);
    m_current_statements->push_back(core::move(td));

    for (auto& tc : m_temporary_cleanup) {
      scope->initialized_statements(m_allocator).push_back(core::move(tc));
    }

    auto st = memory::make_unique<ast_builtin_statement>(m_allocator, nullptr);
    st->m_builtin_type = builtin_statement_type::break_if_not;
    st->m_break_loop = loop.get();

    auto id = memory::make_unique<ast_id>(m_allocator, _inst);
    id->m_declaration = decl;
    id->m_type = decl->m_type;

    st->initialized_expressions(m_allocator).push_back(core::move(id));
    m_current_statements->push_back(core::move(st));
    m_temporary_cleanup.clear();
    pop_scope();
    loop->m_condition_scope_block = core::move(scope);
  }

  memory::unique_ptr<ast_scope_block> scope =
      memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
  scope = memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
  push_scope(scope.get());
  loop->m_body = core::move(scope);

  m_current_loop = loop.get();
  for (auto& i : _inst->get_body()->get_instructions()) {
    if (!resolve_statement(i.get())) {
      return false;
    }
  }
  pop_scope();

  m_current_loop = nullptr;
  if (_inst->get_post_inst()) {
    memory::unique_ptr<ast_scope_block> inner_scope =
        memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
    push_scope(inner_scope.get());
    loop->m_increment_statements = core::move(inner_scope);

    if (!resolve_statement(_inst->get_post_inst())) {
      return false;
    }

    pop_scope();
  }

  bool body_returns = loop->m_body->m_returns;
  m_current_statements->push_back(core::move(loop));
  if (outer_scope) {
    pop_scope();
  }

  m_current_loop = old_loop;
  m_nested_scopes.back()->m_returns =
      body_returns || (outer_scope ? outer_scope->m_returns : false);
  if (outer_scope) {
    m_current_statements->push_back(core::move(outer_scope));
  }
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_do_loop(
    const do_instruction* _inst) {
  auto loop = memory::make_unique<ast_loop>(m_allocator, _inst);
  ast_loop* old_loop = m_current_loop;

  memory::unique_ptr<ast_scope_block> scope =
      memory::make_unique<ast_scope_block>(m_allocator, _inst->get_body());
  push_scope(scope.get());
  loop->m_body = core::move(scope);
  m_current_loop = loop.get();
  for (auto& i : _inst->get_body()->get_instructions()) {
    if (!resolve_statement(i.get())) {
      return false;
    }
  }

  loop->m_post_condition = resolve_expression(_inst->get_condition());
  if (!loop->m_post_condition) {
    return false;
  }
  for (auto& tc : m_temporary_cleanup) {
    loop->m_post_condition->initialized_destroy_expressions(m_allocator)
        .push_back(core::move(tc));
  }
  m_temporary_cleanup.clear();

  pop_scope();
  m_current_loop = old_loop;
  m_current_statements->push_back(core::move(loop));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_statement(
    const instruction* _instruction) {
  bool b = [this, _instruction]() {
    if (m_nested_scopes.back()->m_returns) {
      _instruction->log_line(m_log, logging::log_level::error);
      m_log->log_error("Instruction after return from block");
      return false;
    }
    switch (_instruction->get_node_type()) {
      case node_type::return_instruction:
        return resolve_return(cast_to<return_instruction>(_instruction));
      case node_type::assignment_instruction:
        return resolve_assignment(
            cast_to<assignment_instruction>(_instruction));
      case node_type::break_instruction:
        return resolve_break(cast_to<break_instruction>(_instruction));
      case node_type::continue_instruction:
        return resolve_continue(cast_to<continue_instruction>(_instruction));
      case node_type::declaration:
        return resolve_declaration(cast_to<declaration>(_instruction));
      case node_type::do_instruction:
        return resolve_do_loop(cast_to<do_instruction>(_instruction));
      case node_type::else_if_instruction:
        WN_RELEASE_ASSERT(false, "Unimplemented instruction type");
      case node_type::expression_instruction:
        return resolve_evaluate_expression(
            cast_to<expression_instruction>(_instruction));
      case node_type::for_instruction:
        return resolve_for_loop(cast_to<for_instruction>(_instruction));
      case node_type::if_instruction:
        return resolve_if(cast_to<if_instruction>(_instruction));
      case node_type::while_instruction:
        return resolve_while_loop(cast_to<while_instruction>(_instruction));
      default:
        WN_RELEASE_ASSERT(false, "Unknown instruction type");
    }
    return false;
  }();
  if (b) {
    for (auto& it : m_temporary_cleanup) {
      m_current_statements->push_back(core::move(it));
    }
    m_temporary_cleanup.clear();
  }
  return b;
}

bool parse_ast_convertor::convertor_context::resolve_break(
    const break_instruction* _inst) {
  auto break_inst = memory::make_unique<ast_break>(m_allocator, _inst);
  if (m_current_loop == nullptr) {
    _inst->log_line(m_log, logging::log_level::error);
    m_log->log_error("Cannot break from this location");
    return false;
  }

  break_inst->m_break_loop = m_current_loop;

  clean_scopes(m_current_loop->m_body.get());
  m_current_statements->push_back(core::move(break_inst));
  m_nested_scopes.back()->m_breaks = true;
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_continue(
    const continue_instruction* _inst) {
  auto cont_inst =
      memory::make_unique<ast_continue_instruction>(m_allocator, _inst);
  if (m_current_loop == nullptr) {
    _inst->log_line(m_log, logging::log_level::error);
    m_log->log_error("Cannot continue from this location");
    return false;
  }

  cont_inst->m_continue_loop = m_current_loop;

  clean_scopes(m_current_loop->m_body.get());
  m_current_statements->push_back(core::move(cont_inst));
  m_nested_scopes.back()->m_breaks = true;
  return true;
}

}  // namespace scripting
}  // namespace wn
