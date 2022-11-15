// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

memory::unique_ptr<ast_function>
parse_ast_convertor::convertor_context::pre_resolve_function(
    const function* _function, const ast_type* _implicit_this) {
  memory::unique_ptr<ast_function> fn =
      memory::make_unique<ast_function>(m_allocator, _function);

  const ast_type* ret_ty = resolve_type(_function->get_signature()->get_type());

  fn->m_return_type = ret_ty;
  if (!fn->m_return_type) {
    return nullptr;
  }
  fn->m_name = _function->get_signature()->get_name().to_string(m_allocator);

  if (_implicit_this && _implicit_this->is_synchronized()) {
    if (_function->is_action() &&
        ret_ty != m_type_manager->void_t(&m_used_types)) {
      _function->log_line(m_log, logging::log_level::error);
      m_log->log_error("Actions on Actors must not return values");
      return nullptr;
    }
    if (!_function->is_synchronized()) {
      // fn->m_name += "@";
    }
  }

  containers::string mangled_name(m_allocator);
  containers::dynamic_array<const ast_type*> types(m_allocator);
  types.push_back(ret_ty);
  if (!_implicit_this) {
    auto it = m_named_functions.find(fn->m_name);
    if (it == m_named_functions.end()) {
      it = m_named_functions
               .insert(core::make_pair(fn->m_name,
                   containers::deque<const ast_function*>(m_allocator)))
               .first;
    }
    it->second.push_back(fn.get());
  }

  containers::deque<ast_function::parameter>& function_params =
      fn->initialized_parameters(m_allocator);
  if (_implicit_this) {
    types.push_back(_implicit_this);
    function_params.push_back(ast_function::parameter{
        containers::string(m_allocator, "_this"), fn.get(), _implicit_this});
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
          containers::string(m_allocator, param->get_name()), fn.get(),
          param_type});
    }
  }

  if (ret_ty->m_pass_by_reference) {
    function_params.push_back(ast_function::parameter{
        containers::string(m_allocator, "_return"), fn.get(), ret_ty});
  }

  fn->m_is_virtual = _function->is_virtual();
  fn->m_is_override = _function->is_override();
  fn->m_is_synchronized = _function->is_synchronized();
  fn->m_is_member_function = _implicit_this != nullptr;
  fn->calculate_mangled_name(m_allocator);
  fn->m_function_pointer_type = m_type_manager->resolve_function_ptr_type(
      core::move(types), &m_used_types);

  if (_function->is_action()) {
    {  // callee function AND callee struct
      auto callee_def =
          memory::make_unique<struct_definition>(m_allocator, m_allocator,
              (containers::string(m_allocator, "__") + fn->m_mangled_name +
                  "__call_params")
                  .c_str(),
              false, nullptr);
      callee_def->copy_location_from(_function->get_signature());
      callee_def->set_is_actor_callee(true);
      auto constructor_params =
          memory::make_unique<parameter_list>(m_allocator, m_allocator);
      constructor_params->copy_location_from(_function->get_signature());

      if (!m_type_manager->register_struct_definition(callee_def.get())) {
        callee_def->log_line(m_log, logging::log_level::error);
        return nullptr;
      }

      {
        auto decl = memory::make_unique<declaration>(m_allocator, m_allocator);
        decl->copy_location_from(_function->get_signature());
        auto fnp_type = memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::function_ptr_type);
        fnp_type->copy_location_from(_function);
        fnp_type->set_reference_type(reference_type::raw);

        auto this_param = memory::make_unique<parameter>(
            m_allocator, m_allocator, core::move(fnp_type), "_fn");
        this_param->copy_location_from(_function->get_signature());
        decl->set_parameter(core::move(this_param));
        auto construct_param = memory::make_unique<parameter>(m_allocator,
            m_allocator, clone_node(m_allocator, decl->get_type()), "__fn");
        construct_param->copy_location_from(_function->get_signature());
        constructor_params->add_parameter(core::move(construct_param));

        auto ex = memory::make_unique<id_expression>(
            m_allocator, m_allocator, "__fn");
        ex->copy_location_from(_function->get_signature());
        decl->add_expression_initializer(core::move(ex));
        callee_def->add_struct_elem(core::move(decl), false);
      }

      {
        auto decl = memory::make_unique<declaration>(m_allocator, m_allocator);
        decl->copy_location_from(_function->get_signature());
        auto this_type = memory::make_unique<type>(
            m_allocator, m_allocator, _implicit_this->m_name.c_str());
        this_type->copy_location_from(_function);
        this_type->set_reference_type(reference_type::raw);
        auto this_param = memory::make_unique<parameter>(
            m_allocator, m_allocator, core::move(this_type), "__this");
        this_param->copy_location_from(_function->get_signature());
        decl->set_parameter(core::move(this_param));
        auto construct_param = memory::make_unique<parameter>(m_allocator,
            m_allocator, clone_node(m_allocator, decl->get_type()), "___this");

        construct_param->copy_location_from(_function->get_signature());
        constructor_params->add_parameter(core::move(construct_param));

        auto ex = memory::make_unique<id_expression>(
            m_allocator, m_allocator, "___this");
        ex->copy_location_from(_function->get_signature());
        decl->add_expression_initializer(core::move(ex));
        callee_def->add_struct_elem(core::move(decl), false);
      }

      for (auto& it : _function->get_parameters()->get_parameters()) {
        auto decl = memory::make_unique<declaration>(m_allocator, m_allocator);
        decl->copy_location_from(_function->get_signature());
        auto parm = memory::make_unique<parameter>(m_allocator, m_allocator,
            clone_node(m_allocator, it->get_type()),
            (containers::string(m_allocator, "_") +
                it->get_name().to_string(m_allocator))
                .c_str());
        parm->copy_location_from(_function->get_signature());
        decl->set_parameter(core::move(parm));

        auto construct_param = memory::make_unique<parameter>(m_allocator,
            m_allocator, clone_node(m_allocator, it->get_type()),
            (containers::string(m_allocator, "__") +
                it->get_name().to_string(m_allocator))
                .c_str());
        construct_param->copy_location_from(_function->get_signature());
        constructor_params->add_parameter(core::move(construct_param));

        auto ex = memory::make_unique<id_expression>(m_allocator, m_allocator,
            (containers::string(m_allocator, "__") +
                it->get_name().to_string(m_allocator))
                .c_str());
        ex->copy_location_from(_function->get_signature());
        decl->add_expression_initializer(core::move(ex));
        callee_def->add_struct_elem(core::move(decl), false);
      }
      callee_def->set_constructor_parameters(core::move(constructor_params));
      fn->m_action_call_type = walk_struct_definition(callee_def.get());
      if (!fn->m_action_call_type) {
        return nullptr;
      }

      auto insts =
          memory::make_unique<instruction_list>(m_allocator, m_allocator);
      insts->copy_location_from(_function->get_signature());
      {
        // _this
        auto idexpr = memory::make_unique<id_expression>(
            m_allocator, m_allocator, "_this");
        idexpr->copy_location_from(_function->get_signature());
        // _this->__this
        auto macc = memory::make_unique<member_access_expression>(
            m_allocator, m_allocator, "__this");
        macc->copy_location_from(_function->get_signature());
        macc->add_base_expression(clone_node(m_allocator, idexpr.get()));

        // _this->__this-><function>
        auto fnacc = memory::make_unique<member_access_expression>(
            m_allocator, m_allocator, _function->get_signature()->get_name());
        fnacc->copy_location_from(_function->get_signature());
        fnacc->add_base_expression(core::move(macc));

        auto al = memory::make_unique<arg_list>(m_allocator, m_allocator);
        al->copy_location_from(_function->get_signature());
        {
          for (auto& it : _function->get_parameters()->get_parameters()) {
            auto param_expr = memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_this");
            param_expr->copy_location_from(_function->get_signature());

            auto param_macc = memory::make_unique<member_access_expression>(
                m_allocator, m_allocator,
                containers::string(m_allocator, "_") + it->get_name_str());
            param_macc->copy_location_from(_function->get_signature());
            param_macc->add_base_expression(core::move(param_expr));
            al->add_expression(core::move(param_macc));
          }
        }

        auto fn_call = memory::make_unique<function_call_expression>(
            m_allocator, m_allocator, core::move(al));
        fn_call->copy_location_from(_function->get_signature());
        fn_call->add_base_expression(core::move(fnacc));

        auto expr_inst = memory::make_unique<expression_instruction>(
            m_allocator, m_allocator, core::move(fn_call));
        expr_inst->copy_location_from(_function->get_signature());
        insts->add_instruction(core::move(expr_inst));

        auto param_expr = memory::make_unique<id_expression>(
            m_allocator, m_allocator, "_this");
        param_expr->copy_location_from(_function->get_signature());
        al = memory::make_unique<arg_list>(m_allocator, m_allocator);
        al->copy_location_from(_function->get_signature());
        al->add_expression(core::move(param_expr));

        auto new_id_expr = memory::make_unique<id_expression>(
            m_allocator, m_allocator, "_free_actor_call");
        new_id_expr->copy_location_from(_function->get_signature());

        fn_call = memory::make_unique<function_call_expression>(
            m_allocator, m_allocator, core::move(al));
        fn_call->copy_location_from(_function->get_signature());
        fn_call->add_base_expression(core::move(new_id_expr));

        expr_inst = memory::make_unique<expression_instruction>(
            m_allocator, m_allocator, core::move(fn_call));
        expr_inst->copy_location_from(_function->get_signature());
        insts->add_instruction(core::move(expr_inst));
      }

      auto struct_callee_type = memory::make_unique<type>(
          m_allocator, m_allocator, fn->m_action_call_type->m_name.c_str());
      struct_callee_type->copy_location_from(_function);
      struct_callee_type->set_reference_type(reference_type::unique);

      auto param = memory::make_unique<parameter>(
          m_allocator, m_allocator, core::move(struct_callee_type), "_this");
      param->copy_location_from(_function->get_signature());
      auto param_list = memory::make_unique<parameter_list>(
          m_allocator, m_allocator, core::move(param));
      param_list->copy_location_from(_function->get_signature());

      auto sig = memory::make_unique<parameter>(m_allocator, m_allocator,
          clone_node(m_allocator, _function->get_signature()->get_type()),
          (containers::string(m_allocator, "__call_") +
              _function->get_signature()->get_name_str())
              .c_str());

      auto af = memory::make_unique<function>(m_allocator, m_allocator,
          core::move(sig), core::move(param_list), core::move(insts));
      af->copy_location_from(_function->get_signature());

      auto ah = pre_resolve_function(af.get(), nullptr);
      ah->m_is_action_caller = true;
      if (!ah) {
        return nullptr;
      }
      m_action_helpers_parse.push_back(core::move(af));
      fn->m_action_function = ah.get();
      m_action_helpers.push_back(core::move(ah));
    }

    {
      memory::unique_ptr<ast_function> new_fn =
          memory::make_unique<ast_function>(m_allocator, _function);

      // Hopefully still void
      new_fn->m_return_type = ret_ty;
      if (!new_fn->m_return_type) {
        return nullptr;
      }
      new_fn->m_name =
          containers::string(m_allocator, "__call__action__") +
          _function->get_signature()->get_name().to_string(m_allocator);
      containers::string new_mangled_name(m_allocator);
      containers::dynamic_array<const ast_type*> new_types(m_allocator);
      new_types.push_back(ret_ty);
      auto it = m_named_functions.find(new_fn->m_name);
      if (it == m_named_functions.end()) {
        it = m_named_functions
                 .insert(core::make_pair(new_fn->m_name,
                     containers::deque<const ast_function*>(m_allocator)))
                 .first;
      }
      it->second.push_back(new_fn.get());

      containers::deque<ast_function::parameter>& new_function_params =
          new_fn->initialized_parameters(m_allocator);
      new_function_params.push_back(
          ast_function::parameter{containers::string(m_allocator, "_count"),
              fn.get(), m_type_manager->integral(32, &m_used_types)});
      new_function_params.push_back(ast_function::parameter{
          containers::string(m_allocator, "_this"), fn.get(), _implicit_this});

      if (_function->get_parameters() &&
          _function->get_parameters()->get_parameters().size()) {
        for (auto& param : _function->get_parameters()->get_parameters()) {
          const ast_type* param_type = resolve_type(param->get_type());
          if (!param_type) {
            return nullptr;
          }
          new_types.push_back(param_type);
          new_function_params.push_back(ast_function::parameter{
              containers::string(m_allocator, param->get_name()), fn.get(),
              param_type});
        }
      }

      new_fn->m_is_virtual = false;
      new_fn->m_is_override = false;
      new_fn->m_is_synchronized = false;
      new_fn->m_is_member_function = false;
      new_fn->calculate_mangled_name(m_allocator);
      new_fn->m_function_pointer_type =
          m_type_manager->resolve_function_ptr_type(
              core::move(new_types), &m_used_types);

      new_fn->m_scope =
          memory::make_unique<ast_scope_block>(m_allocator, _function);

      auto& statements = new_fn->m_scope->initialized_statements(m_allocator);
      // We are invoking an action on a different actor,
      // then instead of
      // foo->bar(a, b, c);
      // we instead call
      //
      // external_actor_act(__foo__bar__call_params_construct(external_get_call_param_mem(sizeof(__foo__bar__call_params)),
      // foo, bar, a, b, c))

      auto num_bytes =
          memory::make_unique<ast_builtin_expression>(m_allocator, _function);
      num_bytes->m_type = m_type_manager->size_t_t(&m_used_types);
      num_bytes->initialized_extra_types(m_allocator)
          .push_back(fn->m_action_call_type);
      num_bytes->m_builtin_type = builtin_expression_type::size_of;

      auto alloc_call = memory::make_unique<ast_function_call_expression>(
          m_allocator, _function);
      alloc_call->m_function =
          m_type_manager->allocate_actor_call(&m_used_externals);
      alloc_call->initialized_parameters(m_allocator)
          .push_back(core::move(num_bytes));
      alloc_call->m_type = alloc_call->m_function->m_return_type;

      auto constructor_call = memory::make_unique<ast_function_call_expression>(
          m_allocator, _function);
      constructor_call->m_function = fn->m_action_call_type->m_constructor;
      // constructor_call->m_parameters =
      // core::move(function_call->m_parameters);

      auto function_call = memory::make_unique<ast_function_call_expression>(
          m_allocator, _function);
      function_call->m_type = ret_ty;
      WN_DEBUG_ASSERT(ret_ty == m_type_manager->void_t(nullptr),
          "Expected return type to be void for action");

      auto& params = constructor_call->initialized_parameters(m_allocator);

      {
        // The count doesn't go down to the allocator.
        bool first = true;
        for (auto& x : new_function_params) {
          if (first) {
            first = false;
            continue;
          }
          auto ide = memory::make_unique<ast_id>(m_allocator, _function);
          ide->m_type = x.m_type;
          ide->m_function_parameter = &x;
          params.push_back(core::move(ide));
        }
      }

      auto fn_ptr = memory::make_unique<ast_function_pointer_expression>(
          m_allocator, _function);
      fn_ptr->m_function = fn->m_action_function;
      fn_ptr->m_type = fn->m_action_function->m_function_pointer_type;

      auto fn_ptr_as_void =
          memory::make_unique<ast_cast_expression>(m_allocator, _function);
      fn_ptr_as_void->m_type = m_type_manager->function_t(&m_used_types);
      fn_ptr_as_void->m_base_expression = core::move(fn_ptr);

      constructor_call->m_parameters.push_front(core::move(fn_ptr_as_void));
      constructor_call->m_parameters.push_front(
          make_cast(core::move(alloc_call),
              m_type_manager->get_reference_of(fn->m_action_call_type,
                  ast_type_classification::reference, &m_used_types)));
      constructor_call->m_type = constructor_call->m_function->m_return_type;

      function_call->m_function =
          m_type_manager->call_actor_function(&m_used_builtins);

      auto& nps = function_call->initialized_parameters(m_allocator);
      {
        auto ide = memory::make_unique<ast_id>(m_allocator, _function);
        ide->m_type = new_function_params[0].m_type;
        ide->m_function_parameter = &new_function_params[0];
        nps.push_back(core::move(ide));
      }

      {
        nps.push_back(make_cast(core::move(constructor_call),
            function_call->m_function->m_parameters[1].m_type));
      }

      auto statement =
          memory::make_unique<ast_evaluate_expression>(m_allocator, _function);
      statement->m_expr = core::move(function_call);
      statements.push_back(core::move(statement));

      statements.push_back(
          memory::make_unique<ast_return_instruction>(m_allocator, _function));
      fn->m_action_call_function = new_fn.get();
      m_action_helpers.push_back(core::move(new_fn));
    }
  }

  return fn;
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
      scope->m_returns = true;
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
