// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

struct overriden_type {
  uint32_t m_member_offset;
  const declaration* m_decl;
  const type* m_type;
};

containers::hash_map<containers::string_view, overriden_type>
resolve_value_override(memory::allocator* _allocator,
    const containers::deque<const struct_definition*>& _defs) {
  containers::hash_map<containers::string_view, overriden_type>
      m_value_overrides(_allocator);

  for (auto st = _defs.rbegin(); st != _defs.rend(); ++st) {
    for (auto& it : (*st)->get_struct_members()) {
      auto valit = m_value_overrides.find(it->get_name());
      if (valit == m_value_overrides.end()) {
        if (it->is_inherited()) {
          m_value_overrides[it->get_name()] =
              overriden_type{0, it.get(), nullptr};
        }
      }
    }
  }

  return m_value_overrides;
}

ast_type* parse_ast_convertor::convertor_context::walk_struct_definition(
    const struct_definition* _def) {
  ast_type* resolved_type =
      m_type_manager->struct_definition_resolved(_def->get_name());
  if (resolved_type) {
    return resolved_type;
  }

  containers::deque<const struct_definition*> type_chain(m_allocator);
  ast_type* parent_type = nullptr;

  const struct_definition* d = _def;
  while (d->get_parent_name() != "") {
    auto pd = m_type_manager->get_struct_definition(d->get_parent_name());
    if (pd == nullptr) {
      _def->log_line(m_log, logging::log_level::error);
      m_log->log_error("Could not find parent type: ", _def->get_parent_name());
      return nullptr;
    }

    auto ancestor = walk_struct_definition(pd);
    if (!ancestor) {
      return nullptr;
    }
    if (!parent_type) {
      parent_type = ancestor;
    }
    type_chain.push_front(pd);
    d = pd;
  }

  ast_type* struct_type = m_type_manager->get_or_register_struct(
      _def->get_name(), &m_used_types, &m_used_externals);
  struct_type->m_parent_type = parent_type ? parent_type : nullptr;
  struct_type->m_struct_is_class = _def->is_class();
  type_chain.push_back(_def);

  containers::string name = _def->get_name().to_string(m_allocator);

  if (struct_type->m_struct_is_defined) {
    _def->log_line(m_log, logging::log_level::error);
    m_log->log_error("Double or recursive struct definition");
    return nullptr;
  }
  struct_type->m_struct_is_defined = true;

  containers::hash_set<containers::string> existing_member(m_allocator);
  // bool has_overloaded_construction = false;

  ast_vtable* vt = nullptr;
  uint32_t vtable_idx = 0;

  // These declarations do not have their initializers associated.
  containers::deque<memory::unique_ptr<ast_declaration>>
      class_order_declarations(m_allocator);

  // These will be used to create the constructor + destructor.
  containers::deque<memory::unique_ptr<declaration>>
      constructor_order_declarations(m_allocator);

  for (auto& t : type_chain) {
    // We push temporarily into this deque, so that we can push in reverse order
    // to the previous one.
    containers::deque<memory::unique_ptr<declaration>>
        temp_constructor_order_declarations(m_allocator);

    // Leave this in. It is needed to register the parent types
    // usage.
    m_type_manager->get_or_register_struct(
        t->get_name(), &m_used_types, &m_used_externals);
    auto& functions = t->get_functions();
    for (size_t i = 0; i < t->get_functions().size(); ++i) {
      if (!vt && functions[i]->is_virtual()) {
        memory::unique_ptr<ast_declaration> decl = make_temp_declaration(_def,
            containers::string(m_allocator, "__vtable"),
            m_type_manager->vtable_t(&m_used_types));

        struct_type->m_vtable_index =
            static_cast<uint32_t>(class_order_declarations.size());
        vtable_idx = struct_type->m_vtable_index;
        class_order_declarations.push_back(core::move(decl));
        ast_vtable* vtable = m_type_manager->make_vtable(name);
        vt = vtable;
        struct_type->m_vtable = vt;
        m_script_file->m_all_vtables.push_back(vt);
      }
    }

    for (auto& it : t->get_struct_members()) {
      ast_declaration* decl = nullptr;
      if (!it->is_inherited()) {
        if (!t->is_class() && t->get_parent_name() != "") {
          it->log_line(m_log, logging::log_level::error);
          m_log->log_error(
              "Inherited structs can only modify constructors, not add new "
              "members");
          return nullptr;
        }

        memory::unique_ptr<ast_declaration> u_decl = make_temp_declaration(
            it.get(), containers::string(m_allocator, it->get_name()),
            resolve_type(it->get_type()));
        decl = u_decl.get();
        if (u_decl->m_type == nullptr) {
          return nullptr;
        }
        for (auto& de : class_order_declarations) {
          if (de->m_name == it->get_name()) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error(
                "Member ", de->m_name, " hides declaration in parent");
            return nullptr;
          }
        }

        auto del = clone_node(m_allocator, it.get());
        temp_constructor_order_declarations.push_back(core::move(del));
        class_order_declarations.push_back(core::move(u_decl));
      } else {
        for (auto& de : class_order_declarations) {
          if (de->m_name == it->get_name()) {
            decl = de.get();
            break;
          }
        }
        if (!decl) {
          it->log_line(m_log, logging::log_level::error);
          m_log->log_error("Tried to override ", it->get_name(),
              " which does not", " exist in the parent class");
        }
        memory::unique_ptr<type> old_type;
        for (auto ii = constructor_order_declarations.begin();
             ii != constructor_order_declarations.end(); ++ii) {
          if (it->get_name() == (*ii)->get_name()) {
            old_type = clone_node(m_allocator, (*ii)->get_type());
            constructor_order_declarations.erase(ii);
            break;
          }
        }

        auto param = memory::make_unique<parameter>(m_allocator, m_allocator);
        param->set_name(it->get_name());
        param->set_type(core::move(old_type));
        auto del = clone_node(m_allocator, it.get());
        del->set_parameter(param.release());
        temp_constructor_order_declarations.push_back(core::move(del));
      }

      // If a unique value is actually a struct, (as opposed to a class)
      // we can inline the whole thing, no need to deal with indirection.
      if (decl->m_type->m_classification ==
          ast_type_classification::reference) {
        if (!decl->m_type->m_implicitly_contained_type->m_struct_is_class) {
          decl->m_type = decl->m_type->m_implicitly_contained_type;
        }
      }
    }

    for (auto ri = temp_constructor_order_declarations.rbegin();
         ri != temp_constructor_order_declarations.rend(); ++ri) {
      constructor_order_declarations.push_front(core::move(*ri));
    }
  }

  if (class_order_declarations.empty()) {
    memory::unique_ptr<ast_declaration> decl =
        make_temp_declaration(_def, containers::string(m_allocator, "__dummy"),
            m_type_manager->integral(8, &m_used_types));
    class_order_declarations.push_back(core::move(decl));
  }

  struct_type->m_structure_members = core::move(class_order_declarations);
  m_type_manager->set_struct_definition_resolved(_def->get_name(), struct_type);

  containers::deque<memory::unique_ptr<ast_statement>> destructor_values(
      m_allocator);
  ast_type* child = struct_type;

  {
    auto fn = memory::make_unique<ast_function>(m_allocator, _def);
    fn->m_name =
        containers::string(m_allocator, "_construct_") + struct_type->m_name;

    // Child might get over-written later.
    fn->initialized_parameters(m_allocator)
        .emplace_back(
            ast_function::parameter(containers::string(m_allocator, "_this"),
                m_type_manager->get_reference_of(
                    child, ast_type_classification::reference, &m_used_types)));
    if (_def->constructor_params()) {
      for (auto& param : _def->constructor_params()->get_parameters()) {
        const ast_type* param_type = resolve_type(param->get_type());
        if (!param_type) {
          return nullptr;
        }
        fn->initialized_parameters(m_allocator)
            .push_back(ast_function::parameter{
                containers::string(m_allocator, param->get_name()),
                param_type});
      }
    }

    auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
    scope->m_returns = true;
    auto& body = scope->initialized_statements(m_allocator);
    fn->m_scope = core::move(scope);

    ast_function* cf = m_current_function;
    m_current_function = fn.get();
    push_scope(fn->m_scope.get());

    for (auto& decl : constructor_order_declarations) {
      if (!resolve_declaration(decl.get())) {
        return nullptr;
      }
    }

    if (!m_nested_scopes.back()->m_cleanup_statements.empty()) {
      for (auto& s : m_nested_scopes.back()->m_cleanup_statements) {
        destructor_values.push_front(core::move(s));
      }
    }

    pop_scope();

    containers::deque<memory::unique_ptr<ast_declaration>> child_decls(
        m_allocator);

    for (auto it = body.begin(); it != body.end(); ++it) {
      if ((*it)->m_node_type == ast_node_type::ast_declaration) {
        ast_declaration* decl = cast_to<ast_declaration>(it->get());
        bool existing_decl = false;
        size_t index = 0;
        for (auto& a : struct_type->m_structure_members) {
          if (a->m_name == decl->m_name) {
            existing_decl = true;
            break;
          }
          ++index;
        }

        if (decl->m_is_scope_bound || existing_decl) {
          decl->m_indirected_on_this = true;
          if (existing_decl) {
            decl->m_indirected_offset = static_cast<uint32_t>(index);
          }

          if (decl->m_initializer) {
            auto id = memory::make_unique<ast_id>(m_allocator, _def);
            id->m_type = decl->m_type;
            id->m_declaration = decl;

            auto assign =
                memory::make_unique<ast_assignment>(m_allocator, _def);
            assign->m_lhs = core::move(id);
            assign->m_rhs = core::move(decl->m_initializer);
            it = body.insert(it, core::move(assign));
          }
          if (decl->m_is_scope_bound) {
            if (!_def->is_class()) {
              decl->log_line(m_log, logging::log_level::error);
              m_log->log_error("All structure definitions must be fixed-sized");
              m_log->log_error(
                  "So all arrays must be of a specified size, and it cannot "
                  "contain classes");
              return nullptr;
            }
            child_decls.push_back(clone_ast_node(m_allocator, decl));
            decl->m_indirected_offset =
                static_cast<uint32_t>(struct_type->m_structure_members.size() +
                                      child_decls.size() - 1);
            child_decls.back()->m_initializer = nullptr;
          }
        }
      }
    }

    if (!child_decls.empty()) {
      containers::string child_name =
          containers::string(m_allocator, "_") + struct_type->m_name;

      child = m_type_manager->register_child_struct(
          child_name, struct_type, &m_used_types);
      for (auto& decl : child_decls) {
        decl->m_indirected_on_this = false;
        child->m_structure_members.push_back(core::move(decl));
      }
    }

    fn->m_parameters[0].m_type = m_type_manager->get_reference_of(
        child, ast_type_classification::reference, &m_used_types);
    fn->m_return_type = m_type_manager->get_reference_of(
        struct_type, ast_type_classification::reference, &m_used_types);

    auto ret = memory::make_unique<ast_return_instruction>(m_allocator, _def);

    if (vt) {
      auto this_id = memory::make_unique<ast_id>(m_allocator, _def);
      this_id->m_type = fn->m_parameters[0].m_type;
      this_id->m_function_parameter = &(fn->m_parameters[0]);

      auto member =
          memory::make_unique<ast_member_access_expression>(m_allocator, _def);
      member->m_base_expression = clone_ast_node(m_allocator, this_id.get());
      member->m_member_name = containers::string(m_allocator, "__vtable");
      member->m_member_offset = vtable_idx;
      member->m_type = m_type_manager->vtable_t(&m_used_types);

      auto vt_const = memory::make_unique<ast_constant>(m_allocator, _def);
      vt_const->m_type = m_type_manager->vtable_t(&m_used_types);
      vt_const->m_node_value.m_vtable = vt;
      vt_const->m_string_value = containers::string(m_allocator, vt->m_name);

      auto assign = memory::make_unique<ast_assignment>(m_allocator, _def);
      assign->m_lhs = core::move(member);
      assign->m_rhs = core::move(vt_const);
      body.push_front(core::move(assign));
    }

    auto id = memory::make_unique<ast_id>(m_allocator, _def);
    id->m_function_parameter = &fn->m_parameters[0];
    id->m_type = id->m_function_parameter[0].m_type;
    if (id->m_type != fn->m_return_type) {
      ret->m_return_expr = make_cast(core::move(id), fn->m_return_type);
    } else {
      ret->m_return_expr = core::move(id);
    }

    body.push_back(core::move(ret));

    struct_type->m_overloaded_construction_child = child;
    child->m_overloaded_construction_parent = struct_type;

    fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());
    child->m_constructor = fn.get();

    m_current_function = cf;
    fn->calculate_mangled_name(m_allocator);
    m_constructor_destructors.push_back(core::move(fn));
  }

  // We actually have a destructor
  if (!destructor_values.empty()) {
    auto fn = memory::make_unique<ast_function>(m_allocator, _def);
    fn->m_name =
        containers::string(m_allocator, "_destruct_") + struct_type->m_name;

    auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
    scope->m_returns = true;
    auto& body = scope->initialized_statements(m_allocator);
    fn->m_scope = core::move(scope);

    ast_function* cf = m_current_function;
    m_current_function = fn.get();
    push_scope(fn->m_scope.get());

    for (auto& dv : destructor_values) {
      body.push_back(core::move(dv));
    }

    body.push_back(
        memory::make_unique<ast_return_instruction>(m_allocator, _def));
    pop_scope();

    fn->initialized_parameters(m_allocator)
        .emplace_back(
            ast_function::parameter(containers::string(m_allocator, "_this"),
                m_type_manager->get_reference_of(
                    child, ast_type_classification::reference, &m_used_types)));
    fn->m_return_type = m_type_manager->void_t(&m_used_types);

    fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());
    child->m_destructor = fn.get();

    m_current_function = cf;
    fn->calculate_mangled_name(m_allocator);
    m_constructor_destructors.push_back(core::move(fn));
  }

  if (!_def->is_class() && !create_struct_assign(_def, struct_type)) {
    return nullptr;
  }

  return struct_type;
}

bool parse_ast_convertor::convertor_context::create_struct_assign(
    const struct_definition* _def, ast_type* _initialized_type) {
  WN_DEBUG_ASSERT(
      _initialized_type == _initialized_type->m_overloaded_construction_parent,
      "Invalid assignment type");
  WN_DEBUG_ASSERT(
      !_initialized_type->m_struct_is_class, "Invalid assignment of class");

  containers::string assign_name(m_allocator);
  assign_name += "_assign_";
  assign_name += _initialized_type->m_name;

  auto fn = memory::make_unique<ast_function>(m_allocator, _def);
  fn->m_return_type = m_type_manager->void_t(&m_used_types);
  fn->m_name = containers::string(m_allocator, assign_name);

  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      m_type_manager->get_reference_of(
          _initialized_type, ast_type_classification::reference, &m_used_types),
  });
  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_other"),
      m_type_manager->get_reference_of(
          _initialized_type, ast_type_classification::reference, &m_used_types),
  });
  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_uninitialized_this"),
      m_type_manager->bool_t(&m_used_types)});

  char count[11] = {
      0,
  };

  memory::writeuint32(count, static_cast<uint32_t>(assign_name.size()), 10);

  containers::string mangled_name(m_allocator);
  mangled_name = containers::string(m_allocator, "_ZN3wns");
  mangled_name += count;
  mangled_name += assign_name;
  mangled_name += "EvP";
  memory::writeuint32(
      count, static_cast<uint32_t>(_initialized_type->m_name.size()), 10);
  mangled_name += count;
  mangled_name += _initialized_type->m_name;
  memory::writeuint32(
      count, static_cast<uint32_t>(_initialized_type->m_name.size()), 10);
  mangled_name += count;
  mangled_name += _initialized_type->m_name;

  fn->m_mangled_name = core::move(mangled_name);
  fn->m_scope = memory::make_unique<ast_scope_block>(m_allocator, _def);
  auto* statements = &fn->m_scope->initialized_statements(m_allocator);

  auto this_id = memory::make_unique<ast_id>(m_allocator, _def);
  this_id->m_type = m_type_manager->get_reference_of(
      _initialized_type, ast_type_classification::reference, &m_used_types);
  this_id->m_function_parameter = &(params[0]);

  auto other_id = memory::make_unique<ast_id>(m_allocator, _def);
  other_id->m_type = m_type_manager->get_reference_of(
      _initialized_type, ast_type_classification::reference, &m_used_types);
  other_id->m_function_parameter = &(params[1]);

  bool has_copyable_type = false;
  uint32_t pos = 0;

  for (auto& it : _initialized_type->m_structure_members) {
    const ast_type* t = it->m_type;

    auto copy_source =
        memory::make_unique<ast_member_access_expression>(m_allocator, _def);
    copy_source->m_base_expression =
        clone_ast_node(m_allocator, other_id.get());
    copy_source->m_base_expression->m_type = m_type_manager->get_reference_of(
        _initialized_type, ast_type_classification::reference, &m_used_types);
    copy_source->m_member_name = containers::string(m_allocator, it->m_name);
    copy_source->m_member_offset = pos;
    copy_source->m_type = t;
    memory::unique_ptr<ast_expression> source = core::move(copy_source);

    auto copy_dest =
        memory::make_unique<ast_member_access_expression>(m_allocator, _def);
    copy_dest->m_base_expression = clone_ast_node(m_allocator, this_id.get());
    copy_dest->m_base_expression->m_type = m_type_manager->get_reference_of(
        _initialized_type, ast_type_classification::reference, &m_used_types);
    copy_dest->m_member_name = containers::string(m_allocator, it->m_name);
    copy_dest->m_member_offset = pos;
    copy_dest->m_type = t;
    memory::unique_ptr<ast_expression> dest = core::move(copy_dest);
    auto* old_statements = statements;

    bool is_runtime_array =
        t->m_classification == ast_type_classification::runtime_array;
    if (is_runtime_array) {
      auto everything_is_new = memory::make_unique<ast_id>(m_allocator, _def);
      everything_is_new->m_function_parameter = &(params[2]);
      everything_is_new->m_type = m_type_manager->bool_t(&m_used_types);

      auto not_new =
          memory::make_unique<ast_unary_expression>(m_allocator, _def);
      not_new->m_unary_type = ast_unary_type::invert;
      not_new->m_type = m_type_manager->bool_t(&m_used_types);
      not_new->m_base_expression = core::move(everything_is_new);

      auto* temp_st = statements;

      auto if_not_new = memory::make_unique<ast_if_block>(m_allocator, _def);
      if_not_new->m_condition = core::move(not_new);
      if_not_new->m_body =
          memory::make_unique<ast_scope_block>(m_allocator, _def);

      memory::unique_ptr<ast_declaration> decl = make_temp_declaration(_def,
          get_next_temporary_name(), m_type_manager->bool_t(&m_used_types));

      auto should_create_new =
          memory::make_unique<ast_constant>(m_allocator, _def);
      should_create_new->m_type = m_type_manager->bool_t(&m_used_types);
      should_create_new->m_string_value =
          containers::string(m_allocator, "true");
      should_create_new->m_node_value.m_bool = true;
      decl->m_initializer = core::move(should_create_new);

      auto create_new_id = memory::make_unique<ast_id>(m_allocator, _def);
      create_new_id->m_type = m_type_manager->bool_t(&m_used_types);
      create_new_id->m_declaration = decl.get();
      statements->push_back(core::move(decl));

      statements = &if_not_new->m_body->initialized_statements(m_allocator);
      {
        // If this is a runtime array, we should destruct the original
        // free the memory, and reset the memory after that.
        // This especially means that if this is a runtime array we
        // need to construct shared objects rather than move them,
        // since the memory will be undefined.
        const ast_type* internal_type = t->m_implicitly_contained_type;
        memory::unique_ptr<ast_array_destruction> destruction =
            memory::make_unique<ast_array_destruction>(m_allocator, _def);
        if (internal_type->m_destructor) {
          destruction->m_destructor = internal_type->m_destructor;
        } else if (internal_type->m_classification ==
                   ast_type_classification::shared_reference) {
          destruction->m_destructor =
              m_type_manager->release_shared(&m_used_builtins);
          destruction->m_shared = true;
        }
        if (destruction->m_destructor) {
          use_function(destruction->m_destructor);
          destruction->m_target = clone_ast_node(m_allocator, dest.get());
          statements->push_back(core::move(destruction));
        }

        // Only actually free if the sizes do not match.
        auto source_size =
            memory::make_unique<ast_builtin_expression>(m_allocator, _def);
        source_size->m_type = m_type_manager->integral(32, &m_used_types);
        source_size->m_builtin_type = builtin_expression_type::array_length;
        source_size->initialized_expressions(m_allocator)
            .push_back(clone_ast_node(m_allocator, source.get()));

        auto dest_size =
            memory::make_unique<ast_builtin_expression>(m_allocator, _def);
        dest_size->m_type = m_type_manager->integral(32, &m_used_types);
        dest_size->m_builtin_type = builtin_expression_type::array_length;
        dest_size->initialized_expressions(m_allocator)
            .push_back(clone_ast_node(m_allocator, source.get()));

        auto size_doesnt_match =
            memory::make_unique<ast_binary_expression>(m_allocator, _def);
        size_doesnt_match->m_binary_type = ast_binary_type::neq;
        size_doesnt_match->m_type = m_type_manager->bool_t(&m_used_types);
        size_doesnt_match->m_lhs =
            clone_ast_node(m_allocator, source_size.get());
        size_doesnt_match->m_rhs = clone_ast_node(m_allocator, dest_size.get());

        auto if_size_doesnt_match =
            memory::make_unique<ast_if_block>(m_allocator, _def);
        if_size_doesnt_match->m_condition = core::move(size_doesnt_match);
        if_size_doesnt_match->m_body =
            memory::make_unique<ast_scope_block>(m_allocator, _def);

        auto array_free = memory::make_unique<ast_function_call_expression>(
            m_allocator, _def);
        array_free->m_function =
            m_external_functions[containers::string(m_allocator, "_free")];

        array_free->initialized_parameters(m_allocator)
            .push_back(
                core::move(make_cast(clone_ast_node(m_allocator, dest.get()),
                    m_type_manager->void_ptr_t(&m_used_types))));
        array_free->m_type = m_type_manager->void_t(&m_used_types);
        auto dest_c =
            memory::make_unique<ast_evaluate_expression>(m_allocator, _def);
        dest_c->m_expr = core::move(array_free);
        if_size_doesnt_match->m_body->initialized_statements(m_allocator)
            .push_back(core::move(dest_c));

        auto size_matches =
            memory::make_unique<ast_binary_expression>(m_allocator, _def);
        size_matches->m_binary_type = ast_binary_type::eq;
        size_matches->m_type = m_type_manager->bool_t(&m_used_types);
        size_matches->m_lhs = core::move(source_size);
        size_matches->m_rhs = core::move(dest_size);

        auto if_size_matches =
            memory::make_unique<ast_if_block>(m_allocator, _def);
        if_size_matches->m_condition = core::move(size_matches);
        if_size_matches->m_body =
            memory::make_unique<ast_scope_block>(m_allocator, _def);

        auto const_false = memory::make_unique<ast_constant>(m_allocator, _def);
        const_false->m_type = m_type_manager->bool_t(&m_used_types);
        const_false->m_string_value = containers::string(m_allocator, "false");
        const_false->m_node_value.m_bool = false;

        auto assign = memory::make_unique<ast_assignment>(m_allocator, _def);
        assign->m_lhs = clone_ast_node(m_allocator, create_new_id.get());
        assign->m_rhs = core::move(const_false);
        if_size_matches->m_body->initialized_statements(m_allocator)
            .push_back(core::move(assign));
        statements->push_back(core::move(if_size_doesnt_match));
        statements->push_back(core::move(if_size_matches));
      }
      old_statements->push_back(core::move(if_not_new));

      auto if_create_new = memory::make_unique<ast_if_block>(m_allocator, _def);
      if_create_new->m_condition = core::move(create_new_id);
      if_create_new->m_body =
          memory::make_unique<ast_scope_block>(m_allocator, _def);
      statements = &if_create_new->m_body->initialized_statements(m_allocator);

      auto allocate_runtime =
          memory::make_unique<ast_function_call_expression>(m_allocator, _def);
      allocate_runtime->m_function = m_external_functions[containers::string(
          m_allocator, "_allocate_runtime_array")];

      auto num_bytes =
          memory::make_unique<ast_builtin_expression>(m_allocator, _def);
      num_bytes->m_type = m_type_manager->size_t_t(&m_used_types);
      num_bytes->initialized_extra_types(m_allocator)
          .push_back(t->m_implicitly_contained_type);
      num_bytes->m_builtin_type = builtin_expression_type::size_of;

      auto array_count =
          memory::make_unique<ast_builtin_expression>(m_allocator, _def);
      array_count->initialized_expressions(m_allocator)
          .push_back(clone_ast_node(m_allocator, source.get()));
      array_count->m_builtin_type = builtin_expression_type::array_length;
      array_count->m_type = m_type_manager->integral(32, &m_used_types);

      allocate_runtime->initialized_parameters(m_allocator)
          .push_back(core::move(num_bytes));
      allocate_runtime->initialized_parameters(m_allocator)
          .push_back(make_cast(core::move(array_count),
              m_type_manager->size_t_t(&m_used_types)));
      allocate_runtime->m_type = m_type_manager->void_ptr_t(&m_used_types);

      auto assign = memory::make_unique<ast_assignment>(m_allocator, _def);
      assign->m_lhs = clone_ast_node(m_allocator, dest.get());
      assign->m_rhs = make_cast(core::move(allocate_runtime), t);
      statements->push_back(core::move(assign));

      statements = temp_st;
      statements->push_back(core::move(if_create_new));
    }

    // If we are a static array, then we should loop over all of our
    // children and copy that way.
    if (t->m_classification == ast_type_classification::static_array ||
        t->m_classification == ast_type_classification::runtime_array) {
      t = t->m_implicitly_contained_type;
      containers::string temp = get_next_temporary_name();
      memory::unique_ptr<ast_loop> m_loop =
          memory::make_unique<ast_loop>(m_allocator, _def);

      memory::unique_ptr<ast_declaration> decl =
          make_temp_declaration(_def, get_next_temporary_name(),
              m_type_manager->integral(32, &m_used_types));
      auto initializer = memory::make_unique<ast_constant>(m_allocator, _def);
      auto arr_size =
          memory::make_unique<ast_builtin_expression>(m_allocator, _def);
      arr_size->m_builtin_type = builtin_expression_type::array_length;
      arr_size->m_type = m_type_manager->integral(32, &m_used_types);
      arr_size->initialized_expressions(m_allocator)
          .push_back(clone_ast_node(m_allocator, source.get()));
      decl->m_initializer = core::move(arr_size);

      auto sizer = memory::make_unique<ast_id>(m_allocator, _def);
      sizer->m_type = m_type_manager->integral(32, &m_used_types);
      sizer->m_declaration = decl.get();
      statements->push_back(core::move(decl));

      auto zero = memory::make_unique<ast_constant>(m_allocator, _def);
      zero->m_type = m_type_manager->integral(32, &m_used_types);
      zero->m_node_value.m_integer = 0;
      zero->m_string_value = containers::string(m_allocator, "0");

      auto comp = memory::make_unique<ast_binary_expression>(m_allocator, _def);
      comp->m_type = m_type_manager->bool_t(&m_used_types);
      comp->m_binary_type = ast_binary_type::neq;
      comp->m_lhs = clone_ast_node(m_allocator, sizer.get());
      comp->m_rhs = core::move(zero);

      m_loop->m_pre_condition = core::move(comp);

      m_loop->m_body = memory::make_unique<ast_scope_block>(m_allocator, _def);
      auto body = &m_loop->m_body->initialized_statements(m_allocator);
      statements->push_back(core::move(m_loop));
      statements = body;

      auto const_one = memory::make_unique<ast_constant>(m_allocator, _def);
      const_one->m_type = m_type_manager->integral(32, &m_used_types);
      const_one->m_node_value.m_integer = 1;
      const_one->m_string_value = containers::string(m_allocator, "1");

      auto sub = memory::make_unique<ast_binary_expression>(m_allocator, _def);
      sub->m_binary_type = ast_binary_type::sub;
      sub->m_lhs = clone_ast_node(m_allocator, sizer.get());
      sub->m_rhs = core::move(const_one);
      sub->m_type = m_type_manager->integral(32, &m_used_types);

      auto assign = memory::make_unique<ast_assignment>(m_allocator, _def);
      assign->m_lhs = clone_ast_node(m_allocator, sizer.get());
      assign->m_rhs = core::move(sub);
      statements->push_back(core::move(assign));

      auto source_access =
          memory::make_unique<ast_array_access_expression>(m_allocator, _def);
      source_access->m_array = core::move(source);
      source_access->m_index = clone_ast_node(m_allocator, sizer.get());
      source_access->m_type = t;
      source = core::move(source_access);

      auto dest_access =
          memory::make_unique<ast_array_access_expression>(m_allocator, _def);
      dest_access->m_array = core::move(dest);
      dest_access->m_index = core::move(sizer);
      dest_access->m_type = t;
      dest = core::move(dest_access);
    }

    if (t->m_classification == ast_type_classification::shared_reference) {
      has_copyable_type = true;

      if (is_runtime_array) {
        containers::dynamic_array<memory::unique_ptr<ast_expression>>
            assign_params(m_allocator);

        auto const_null = memory::make_unique<ast_constant>(m_allocator, _def);
        const_null->m_string_value = containers::string(m_allocator, "");
        const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
        auto null_as_vptr = make_cast(
            core::move(const_null), m_type_manager->void_ptr_t(&m_used_types));
        assign_params.push_back(core::move(null_as_vptr));

        assign_params.push_back(make_cast(
            core::move(source), m_type_manager->void_ptr_t(&m_used_types)));
        auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
        assign->m_lhs = core::move(dest);
        assign->m_rhs = make_cast(
            call_function(_def, m_type_manager->assign_shared(&m_used_builtins),
                core::move(assign_params)),
            assign->m_lhs->m_type);
        statements->push_back(core::move(assign));
      } else {
        auto is_new = memory::make_unique<ast_id>(m_allocator, _def);
        is_new->m_function_parameter = &(params[2]);
        is_new->m_type = m_type_manager->bool_t(&m_used_types);

        auto if_new = memory::make_unique<ast_if_block>(m_allocator, _def);
        if_new->m_condition = clone_ast_node(m_allocator, is_new.get());
        if_new->m_body =
            memory::make_unique<ast_scope_block>(m_allocator, _def);

        auto const_false = memory::make_unique<ast_constant>(m_allocator, _def);
        const_false->m_type = m_type_manager->bool_t(&m_used_types);
        const_false->m_string_value = containers::string(m_allocator, "false");
        const_false->m_node_value.m_bool = false;

        auto neg = memory::make_unique<ast_unary_expression>(m_allocator, _def);
        neg->m_type = is_new->m_type;
        neg->m_unary_type = ast_unary_type::invert;
        neg->m_base_expression = core::move(is_new);

        auto if_not_new = memory::make_unique<ast_if_block>(m_allocator, _def);
        if_not_new->m_condition = core::move(neg);
        if_not_new->m_body =
            memory::make_unique<ast_scope_block>(m_allocator, _def);

        {
          // Is New
          containers::dynamic_array<memory::unique_ptr<ast_expression>>
              assign_params(m_allocator);

          auto const_null =
              memory::make_unique<ast_constant>(m_allocator, _def);
          const_null->m_string_value = containers::string(m_allocator, "");
          const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
          auto null_as_vptr = make_cast(core::move(const_null),
              m_type_manager->void_ptr_t(&m_used_types));
          assign_params.push_back(core::move(null_as_vptr));

          assign_params.push_back(
              make_cast(clone_ast_node(m_allocator, source.get()),
                  m_type_manager->void_ptr_t(&m_used_types)));
          auto assign =
              memory::make_unique<ast_assignment>(m_allocator, nullptr);
          assign->m_lhs = clone_ast_node(m_allocator, dest.get());
          assign->m_rhs =
              make_cast(call_function(_def,
                            m_type_manager->assign_shared(&m_used_builtins),
                            core::move(assign_params)),
                  assign->m_lhs->m_type);
          if_new->m_body->initialized_statements(m_allocator)
              .push_back(core::move(assign));
        }
        {
          // Not New
          containers::dynamic_array<memory::unique_ptr<ast_expression>>
              assign_params(m_allocator);
          assign_params.push_back(
              make_cast(clone_ast_node(m_allocator, dest.get()),
                  m_type_manager->void_ptr_t(&m_used_types)));
          assign_params.push_back(make_cast(
              core::move(source), m_type_manager->void_ptr_t(&m_used_types)));
          auto assign =
              memory::make_unique<ast_assignment>(m_allocator, nullptr);
          assign->m_lhs = core::move(dest);
          assign->m_rhs =
              make_cast(call_function(_def,
                            m_type_manager->assign_shared(&m_used_builtins),
                            core::move(assign_params)),
                  assign->m_lhs->m_type);
          if_not_new->m_body->initialized_statements(m_allocator)
              .push_back(core::move(assign));
        }
        statements->push_back(core::move(if_new));
        statements->push_back(core::move(if_not_new));
      }

    } else {
      if (t->m_assignment) {
        use_function(t->m_assignment);
        has_copyable_type = true;
        auto ref_type = m_type_manager->get_reference_of(
            t, ast_type_classification::reference, &m_used_types);
        containers::dynamic_array<memory::unique_ptr<ast_expression>>
            assign_params(m_allocator);
        assign_params.push_back(make_cast(core::move(dest), ref_type));
        assign_params.push_back(make_cast(core::move(source), ref_type));

        if (is_runtime_array) {
          auto new_runtime_array =
              memory::make_unique<ast_constant>(m_allocator, _def);
          new_runtime_array->m_type = m_type_manager->bool_t(&m_used_types);
          new_runtime_array->m_string_value =
              containers::string(m_allocator, "true");
          new_runtime_array->m_node_value.m_bool = true;
          assign_params.push_back(core::move(new_runtime_array));
        } else {
          auto everything_is_new =
              memory::make_unique<ast_id>(m_allocator, _def);
          everything_is_new->m_function_parameter = &(params[2]);
          everything_is_new->m_type = m_type_manager->bool_t(&m_used_types);
          assign_params.push_back(core::move(everything_is_new));
        }

        statements->push_back(evaluate_expression(
            call_function(_def, t->m_assignment, core::move(assign_params))));
      } else {
        auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
        assign->m_lhs = core::move(dest);
        assign->m_rhs = core::move(source);
        statements->push_back(core::move(assign));
      }
    }
    ++pos;
    statements = old_statements;
  }

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, _def);
  statements->push_back(core::move(ret));

  // If none of our types have explicit assignments, we can avoid calling them,
  // This can save a lot of time, since this cascades.
  if (has_copyable_type) {
    fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());

    _initialized_type->m_assignment = fn.get();
    m_constructor_destructors.push_back(core::move(fn));
  }
  return true;
}

}  // namespace scripting
}  // namespace wn
