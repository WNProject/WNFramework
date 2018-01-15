// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

void parse_ast_convertor::convertor_context::push_scope(
    ast_scope_block* _scope) {
  m_nested_scopes.push_back(_scope);
  (void)_scope->initialized_statements(m_allocator);
  m_current_statements = &m_nested_scopes.back()->m_statements;
}

void parse_ast_convertor::convertor_context::pop_scope() {
  ast_scope_block* scope = m_nested_scopes.back();

  // Append the correct instructions if this scope block has not been
  // prematurely terminated.
  if (!scope->m_returns && !scope->m_breaks) {
    for (auto& expr : scope->m_cleanup_expressions) {
      auto run_expr =
          memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
      run_expr->copy_location_from(expr.get());
      run_expr->m_expr = core::move(expr);
      m_current_statements->push_back(core::move(run_expr));
    }
  }

  m_nested_scopes.pop_back();
  m_current_statements =
      m_nested_scopes.empty() ? nullptr : &m_nested_scopes.back()->m_statements;
}

void parse_ast_convertor::convertor_context::clean_scopes(
    ast_scope_block* _end_scope_block) {
  for (signed_t i = m_nested_scopes.size() - 1; i >= 0; --i) {
    auto& scope = m_nested_scopes[i];
    for (auto& expr : scope->m_cleanup_expressions) {
      auto run_expr =
          memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
      run_expr->copy_location_from(expr.get());
      run_expr->m_expr = clone_node(m_allocator, expr.get());
      m_current_statements->push_back(core::move(run_expr));
    }
    if (scope == _end_scope_block) {
      break;
    }
  }
}

}  // namespace scripting
}  // namespace wn