// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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
    bool is_shared = false;
    if (m_return_decl->m_type->m_classification ==
        ast_type_classification::shared_reference) {
      if (expr->m_type == m_type_manager->m_nullptr_t.get()) {
        expr = make_cast(core::move(expr), m_return_decl->m_type);
      } else {
        auto etype = expr->m_type;
        is_shared = true;
        auto const_null =
            memory::make_unique<ast_constant>(m_allocator, _instruction);
        const_null->m_string_value = containers::string(m_allocator, "");
        const_null->m_type = m_type_manager->m_nullptr_t.get();
        auto null_as_vptr = make_cast(
            core::move(const_null), m_type_manager->m_void_ptr_t.get());

        containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
            m_allocator);
        params.push_back(core::move(null_as_vptr));
        params.push_back(core::move(
            make_cast(core::move(expr), m_type_manager->m_void_ptr_t.get())));
        expr = make_cast(
            call_function(_instruction, m_assign_shared, core::move(params)),
            etype);
      }
    }

    if (expr->m_type != m_return_decl->m_type) {
      _instruction->log_line(m_log, logging::log_level::error);
      m_log->log_error("Returned value must match the function");
      return false;
    }

    auto id_expr = memory::make_unique<ast_id>(m_allocator, _instruction);
    id_expr->m_declaration = m_return_decl;
    id_expr->m_type = m_return_decl->m_type;

    if (!is_shared) {
      ret_expr->m_return_expr = clone_node(m_allocator, id_expr.get());
    } else {
      containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
          m_allocator);
      params.push_back(make_cast(clone_node(m_allocator, id_expr.get()),
          m_type_manager->m_void_ptr_t.get()));
      ret_expr->m_return_expr = make_cast(
          call_function(_instruction, m_return_shared, core::move(params)),
          m_return_decl->m_type);
    }

    auto assign =
        memory::make_unique<ast_assignment>(m_allocator, _instruction);
    assign->m_lhs = core::move(id_expr);
    assign->m_rhs = core::move(expr);

    m_current_statements->push_back(core::move(assign));
  } else if (m_return_decl) {
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
  memory::unique_ptr<ast_if_chain> if_chain =
      memory::make_unique<ast_if_chain>(m_allocator, _if);
  containers::deque<ast_if_chain::conditional_block>& conditions =
      if_chain->initialized_conditionals(m_allocator);

  bool may_return = false;
  bool may_break = false;
  {
    memory::unique_ptr<ast_expression> expr =
        resolve_expression(_if->get_condition());
    if (!expr) {
      return false;
    }

    memory::unique_ptr<ast_scope_block> scope =
        memory::make_unique<ast_scope_block>(m_allocator, _if->get_body());
    push_scope(scope.get());

    for (auto& i : _if->get_body()->get_instructions()) {
      if (!resolve_statement(i.get())) {
        return false;
      }
    }
    pop_scope();
    may_return = scope->m_returns;
    may_break = scope->m_breaks;
    conditions.push_back(
        ast_if_chain::conditional_block(core::move(expr), core::move(scope)));
  }

  for (auto& i : _if->get_else_if_instructions()) {
    memory::unique_ptr<ast_expression> expr =
        resolve_expression(i->get_condition());

    memory::unique_ptr<ast_scope_block> scope =
        memory::make_unique<ast_scope_block>(m_allocator, i->get_body());
    push_scope(scope.get());
    for (auto& inst : i->get_body()->get_instructions()) {
      if (!resolve_statement(inst.get())) {
        return false;
      }
    }
    pop_scope();
    may_return &= scope->m_returns;
    may_break &= scope->m_breaks;
    conditions.push_back(
        ast_if_chain::conditional_block(core::move(expr), core::move(scope)));
  }

  if (_if->get_else()) {
    memory::unique_ptr<ast_scope_block> scope =
        memory::make_unique<ast_scope_block>(m_allocator, _if->get_else());
    push_scope(scope.get());
    for (auto& i : _if->get_else()->get_instructions()) {
      if (!resolve_statement(i.get())) {
        return false;
      }
    }
    pop_scope();
    may_return &= scope->m_returns;
    may_break &= scope->m_breaks;
    if_chain->m_else_block = core::move(scope);
  } else {
    may_return = false;
    may_break = false;
  }
  m_nested_scopes.back()->m_returns = may_return;
  m_nested_scopes.back()->m_breaks = may_break;
  m_current_statements->push_back(core::move(if_chain));
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

  memory::unique_ptr<ast_expression> init = nullptr;
  memory::unique_ptr<ast_statement> init_statement = nullptr;
  bool has_temporary_declaration = false;
  bool is_shared_struct = false;
  bool is_raw_struct =
      type->m_classification == ast_type_classification::reference &&
      !type->m_implicitly_contained_type->m_struct_is_class;
  if (is_raw_struct) {
    type = type->m_implicitly_contained_type;
  }
  memory::unique_ptr<ast_declaration> decl =
      memory::make_unique<ast_declaration>(m_allocator, _declaration);

  if (_declaration->get_expression()) {
    const expression* expr = _declaration->get_expression();

    if (!(init = resolve_expression(_declaration->get_expression()))) {
      return false;
    }
    bool is_null_assign = init->m_type == m_type_manager->m_nullptr_t.get();

    if (expr->get_node_type() == node_type::struct_allocation_expression) {
      has_temporary_declaration =
          type->m_classification == ast_type_classification::reference;

      if (is_raw_struct) {
        WN_DEBUG_ASSERT(
            !has_temporary_declaration, "We should not have a temporary decl");
        WN_DEBUG_ASSERT(
            init->m_node_type == ast_node_type::ast_function_call_expression,
            "This should be a function call");
        ast_function_call_expression* fn =
            cast_to<ast_function_call_expression>(init.get());

        memory::unique_ptr<ast_id> id =
            memory::make_unique<ast_id>(m_allocator, _declaration);
        id->m_declaration = decl.get();
        id->m_type = type;

        WN_DEBUG_ASSERT(fn->m_parameters.size() == 1,
            "Constructor must have at least one parameters");

        fn->m_parameters[0] =
            make_cast(core::move(id), m_type_manager->get_reference_of(type,
                                          ast_type_classification::reference));
        init->m_temporaries.clear();
        auto in = init.get();
        init_statement = evaluate_expression(core::move(init));
        if (!in->m_destroy_expressions.empty()) {
          auto destructor = type->m_destructor;
          WN_DEBUG_ASSERT(destructor,
              "The initializer had a destructor, therefor the type must have");
          in->m_destroy_expressions.clear();

          memory::unique_ptr<ast_id> dest_id =
              memory::make_unique<ast_id>(m_allocator, _declaration);
          dest_id->m_declaration = decl.get();
          dest_id->m_type = type;
          auto params =
              containers::dynamic_array<memory::unique_ptr<ast_expression>>(
                  m_allocator);
          params.push_back(make_cast(
              core::move(dest_id), m_type_manager->get_reference_of(type,
                                       ast_type_classification::reference)));

          auto& scope_expr =
              m_nested_scopes.back()->initialized_cleanup(m_allocator);
          scope_expr.push_back(call_function(_declaration, destructor, params));
        }
      } else {
        init = make_implicit_cast(core::move(init), type);
      }
    }

    if (init &&
        init->m_type->m_classification ==
            ast_type_classification::static_array &&
        type->m_classification == ast_type_classification::static_array) {
      if (init->m_type->m_static_array_size != 0 &&
          type->m_static_array_size != 0) {
        if (type->m_static_array_size != init->m_type->m_static_array_size) {
          _declaration->log_line(m_log, logging::log_level::error);
          m_log->log_error(
              "Cannot initialize array with an array of different size");
        }

        if (init->m_temporaries.size() == 2 &&
            init->m_temporaries[0]->m_node_type ==
                ast_node_type::ast_declaration &&
            init->m_temporaries[1]->m_node_type ==
                ast_node_type::ast_array_allocation) {
          // We could leave this, but it would not be ideal. It would look
          // something like:
          // array4_int __wns_temp0;
          // for (int i = 0; i < 4; ++i;) { __wns_temp0[i] = 4; }
          // array4_int foo = __wns_temp0;
          // Instead of this we can transform it to just initialize the
          // declaration
          init->m_temporaries.pop_front();
          ast_array_allocation* alloc =
              cast_to<ast_array_allocation>(init->m_temporaries[0].get());
          memory::unique_ptr<ast_id> id =
              memory::make_unique<ast_id>(m_allocator, _declaration);
          id->m_declaration = decl.get();
          id->m_type = type;

          alloc->m_initializee = core::move(id);
          init_statement = core::move(init->m_temporaries[0]);
          init.reset();
        }
      }

      if (type->m_static_array_size == 0 &&
          init->m_type->m_static_array_size != 0) {
        memory::unique_ptr<ast_cast_expression> cast =
            memory::make_unique<ast_cast_expression>(m_allocator, _declaration);
        cast->m_base_expression = core::move(init);
        cast->m_type = get_array_of(type->m_implicitly_contained_type, 0);
        transfer_temporaries(cast.get(), cast->m_base_expression.get());
        init = core::move(cast);
      }
    }

    if (type->m_classification == ast_type_classification::shared_reference) {
      if (!is_null_assign) {
        auto const_null =
            memory::make_unique<ast_constant>(m_allocator, _declaration);
        const_null->m_string_value = containers::string(m_allocator, "");
        const_null->m_type = m_type_manager->m_nullptr_t.get();
        auto null_as_vptr = make_cast(
            core::move(const_null), m_type_manager->m_void_ptr_t.get());

        containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
            m_allocator);
        params.push_back(core::move(null_as_vptr));
        params.push_back(core::move(
            make_cast(core::move(init), m_type_manager->m_void_ptr_t.get())));
        init = make_cast(
            call_function(_declaration, m_assign_shared, core::move(params)),
            type);
      }

      has_temporary_declaration = false;
      is_shared_struct = true;
    }
  }

  if (init && init->m_type != type) {
    _declaration->log_line(m_log, logging::log_level::error);
    m_log->log_error("You must initialize a type with the RHS type");
    return false;
  }

  if (has_temporary_declaration) {
    auto& scope_expr = m_nested_scopes.back()->initialized_cleanup(m_allocator);
    for (auto& temp_decl : init->m_destroy_expressions) {
      scope_expr.push_back(core::move(temp_decl));
    }
    init->m_destroy_expressions.clear();
  }

  decl->is_ephemeral = false;
  decl->m_initializer = core::move(init);
  decl->m_name = _declaration->get_name().to_string(m_allocator);
  decl->m_type = type;
  m_nested_scopes.back()
      ->initialized_declarations(m_allocator)
      .push_back(decl.get());

  if (is_shared_struct) {
    memory::unique_ptr<ast_function_call_expression> function_call =
        memory::make_unique<ast_function_call_expression>(
            m_allocator, _declaration);
    function_call->m_function = m_release_shared;
    auto id = memory::make_unique<ast_id>(m_allocator, _declaration);
    id->m_type = decl->m_type;
    id->m_declaration = decl.get();
    memory::unique_ptr<ast_cast_expression> cast =
        memory::make_unique<ast_cast_expression>(m_allocator, _declaration);
    cast->m_base_expression = core::move(id);
    cast->m_type = m_type_manager->m_void_ptr_t.get();

    function_call->initialized_parameters(m_allocator)
        .push_back(core::move(cast));
    function_call->m_type = m_type_manager->m_void_t.get();
    auto& scope_expr = m_nested_scopes.back()->initialized_cleanup(m_allocator);
    scope_expr.push_back(core::move(function_call));
  }
  if (decl) {
    m_current_statements->push_back(core::move(decl));
  }
  if (init_statement) {
    m_current_statements->push_back(core::move(init_statement));
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

  if (lhs->m_type->m_classification ==
          ast_type_classification::shared_reference &&
      rhs->m_type == m_type_manager->m_nullptr_t.get()) {
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
        rhs->m_type, ast_type_classification::reference);
    rhs = make_cast(core::move(rhs), type);
  }

  if (lhs->m_type != rhs->m_type) {
    _assign->log_line(m_log, logging::log_level::error);
    m_log->log_error("Attempting to assign unequal types");
    return false;
  }

  if (lhs->m_type->m_classification ==
      ast_type_classification::shared_reference) {
    containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
        m_allocator);
    params.push_back(make_cast(clone_node(m_allocator, lhs.get()),
        m_type_manager->m_void_ptr_t.get()));
    params.push_back(
        make_cast(core::move(rhs), m_type_manager->m_void_ptr_t.get()));
    rhs = make_cast(call_function(_assign, m_assign_shared, core::move(params)),
        lhs->m_type);
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
              struct_type, ast_type_classification::reference);
          containers::dynamic_array<memory::unique_ptr<ast_expression>> params(
              m_allocator);
          params.push_back(make_cast(core::move(assign->m_lhs), ref_type));
          params.push_back(make_cast(core::move(rhs), ref_type));

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
      arithmetic->m_lhs = clone_node(m_allocator, assign->m_lhs.get());
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

  expr->m_pre_condition = resolve_expression(_inst->get_condition());
  if (!expr->m_pre_condition) {
    return false;
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
    loop->m_pre_condition = resolve_expression(_inst->get_condition());
    if (!loop->m_pre_condition) {
      return false;
    }
  }

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

  loop->m_post_condition = resolve_expression(_inst->get_condition());
  if (!loop->m_post_condition) {
    return false;
  }

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
  pop_scope();
  m_current_loop = old_loop;
  m_current_statements->push_back(core::move(loop));
  return true;
}

bool parse_ast_convertor::convertor_context::resolve_statement(
    const instruction* _instruction) {
  if (m_nested_scopes.back()->m_returns) {
    _instruction->log_line(m_log, logging::log_level::error);
    m_log->log_error("Instruction after return from block");
    return false;
  }
  switch (_instruction->get_node_type()) {
    case node_type::return_instruction:
      return resolve_return(cast_to<return_instruction>(_instruction));
    case node_type::assignment_instruction:
      return resolve_assignment(cast_to<assignment_instruction>(_instruction));
    case node_type::break_instruction:
      return resolve_break(cast_to<break_instruction>(_instruction));
    case node_type::continue_instruction:
      return resolve_continue(cast_to<continue_instruction>(_instruction));
    case node_type::declaration:
      return resolve_declaration(cast_to<declaration>(_instruction));
    case node_type::set_array_length:
      WN_RELEASE_ASSERT(false, "Unimplemented instruction type");
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