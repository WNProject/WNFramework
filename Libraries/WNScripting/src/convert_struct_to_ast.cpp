// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

ast_type* parse_ast_convertor::convertor_context::walk_struct_definition(
    const struct_definition* _def) {
  if (m_handled_definitions.find(_def) != m_handled_definitions.end()) {
    return m_structure_types[_def->get_name().to_string(m_allocator)].get();
  }

  containers::deque<const struct_definition*> type_chain(m_allocator);
  ast_type* parent_type = nullptr;

  const struct_definition* d = _def;
  while (d->get_parent_name() != "") {
    auto it =
        m_struct_definitions.find(d->get_parent_name().to_string(m_allocator));
    if (it == m_struct_definitions.end()) {
      _def->log_line(m_log, logging::log_level::error);
      m_log->log_error("Could not find parent type:", _def->get_parent_name());
      return nullptr;
    }
    auto ancestor = walk_struct_definition(it->second);
    if (!ancestor) {
      return nullptr;
    }
    if (!parent_type) {
      parent_type = ancestor;
    }
    type_chain.push_front(it->second);
    d = it->second;
  }

  memory::unique_ptr<ast_type> struct_type =
      memory::make_unique<ast_type>(m_allocator);
  struct_type->m_classification = ast_type_classification::struct_type;
  struct_type->m_parent_type = parent_type ? parent_type : nullptr;
  struct_type->m_struct_is_class = _def->is_class();

  type_chain.push_back(_def);

  containers::string name = _def->get_name().to_string(m_allocator);
  if (m_structure_types.find(name) != m_structure_types.end()) {
    _def->log_line(m_log, logging::log_level::error);
    m_log->log_error("Double or recursive struct definition");
    return nullptr;
  }

  containers::hash_set<containers::string> existing_member(m_allocator);

  struct_type->m_name = name;
  struct_type->calculate_mangled_name(m_allocator);
  ast_type* st = struct_type.get();
  m_structure_types[name] = core::move(struct_type);
  bool has_overloaded_construction = false;

  ast_vtable* vt = nullptr;
  uint32_t vtable_idx = 0;

  auto& decls = st->initialized_structure_members(m_allocator);
  for (auto& t : type_chain) {
    auto& functions = t->get_functions();
    for (size_t i = 0; i < t->get_functions().size(); ++i) {
      if (!vt && functions[i]->is_virtual()) {
        memory::unique_ptr<ast_declaration> decl =
            memory::make_unique<ast_declaration>(m_allocator, nullptr);
        decl->m_name = containers::string(m_allocator, "__vtable");
        decl->m_type = m_vtable_t.get();
        st->m_vtable_index = static_cast<uint32_t>(decls.size());
        decls.push_back(core::move(decl));
        auto vtable = memory::make_unique<ast_vtable>(m_allocator);
        vtable->m_name = containers::string(m_allocator, "__vtable__");
        vtable->m_name += name;
        vt = vtable.get();
        st->m_vtable = vt;
        m_script_file->m_all_vtables.push_back(core::move(vtable));
      }
    }

    for (auto& it : t->get_struct_members()) {
      memory::unique_ptr<ast_declaration> u_decl =
          memory::make_unique<ast_declaration>(m_allocator, it.get());
      ast_declaration* decl = u_decl.get();
      u_decl->m_name = containers::string(m_allocator, it->get_name());
      u_decl->m_type = resolve_type(it->get_type());
      decls.push_back(core::move(u_decl));

      // If a unique value is actually a struct, (as opposed to a class)
      // we can inline the whole thing, no need to deal with indirection.
      if (it->get_type()->get_reference_type() == reference_type::unique) {
        if (!decl->m_type->m_implicitly_contained_type->m_struct_is_class) {
          decl->m_type = decl->m_type->m_implicitly_contained_type;
        } else {
          has_overloaded_construction = true;
        }
      }

      if (it->get_type()->get_node_type() == node_type::array_type) {
        if (decl->m_type->m_implicitly_contained_type->m_classification ==
            ast_type_classification::reference) {
          has_overloaded_construction = true;
        }
      }

      if (has_overloaded_construction && !st->m_struct_is_class) {
        it->log_line(m_log, logging::log_level::error);
        m_log->log_error(
            "A struct cannot contain a class member, or an array of them");
        return nullptr;
      }

      if (!existing_member
               .insert(containers::string(m_allocator, it->get_name()))
               .second) {
        it->log_line(m_log, logging::log_level::error);
        m_log->log_error(
            "Member ", it->get_name(), " hides previous definition");
        return nullptr;
      }
    }
  }

  bool empty = false;
  if (decls.empty()) {
    memory::unique_ptr<ast_declaration> decl =
        memory::make_unique<ast_declaration>(m_allocator, _def);
    decl->m_name = containers::string(m_allocator, "__dummy");
    decl->m_type = m_integral_types[8].get();
    decls.push_back(core::move(decl));
    empty = true;
  }

  ast_type* child = nullptr;

  if (has_overloaded_construction) {
    memory::unique_ptr<ast_type> child_type = st->clone(m_allocator);
    containers::string child_name =
        containers::string(m_allocator, "_") + child_type->m_name;
    auto& child_decls = child_type->initialized_structure_members(m_allocator);

    child_type->m_name = child_name;
    child_type->calculate_mangled_name(m_allocator);
    child_type->m_overloaded_construction_parent = st;
    st->m_overloaded_construction_child = child_type.get();
    for (auto& ty : type_chain) {
      for (auto& it : ty->get_struct_members()) {
        ast_type* t = resolve_type(it->get_type());
        if (t->m_classification == ast_type_classification::reference) {
          memory::unique_ptr<ast_declaration> decl =
              memory::make_unique<ast_declaration>(m_allocator, it.get());
          decl->m_name = containers::string(m_allocator, "_") +
                         containers::string(m_allocator, it->get_name());
          if (!t->m_implicitly_contained_type
                   ->m_overloaded_construction_child) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error("Recursively declared member: ", it->get_name());
            return nullptr;
          }
          decl->m_type =
              t->m_implicitly_contained_type->m_overloaded_construction_child;
          child_decls.push_back(core::move(decl));
        }
      }
    }
    child = child_type.get();
    m_structure_types[child_name] = core::move(child_type);
  } else {
    st->m_overloaded_construction_child = st;
    st->m_overloaded_construction_parent = st;
  }

  m_handled_definitions.insert(_def);
  m_ordered_type_definitions.push_back(st);
  if (child) {
    m_ordered_type_definitions.push_back(child);
  }

  if (!empty) {
    if (!create_constructor(
            type_chain, st->m_overloaded_construction_child, vt, vtable_idx)) {
      return nullptr;
    }
    if (!create_destructor(type_chain, st->m_overloaded_construction_child)) {
      return nullptr;
    }
    if (!_def->is_class() && !create_struct_assign(_def, st)) {
      return nullptr;
    }
  }
  return st;
}

bool parse_ast_convertor::convertor_context::create_constructor(
    const containers::deque<const struct_definition*>& _defs,
    ast_type* _initialized_type, ast_vtable* _vtable, uint32_t _vtable_index) {
  auto st_def = _defs.back();
  ast_type* return_type = _initialized_type->m_overloaded_construction_parent;
  bool returns_different_type = (return_type != _initialized_type);

  containers::string constructor_name(m_allocator);
  constructor_name += "_construct_";
  constructor_name += return_type->m_name;

  auto fn = memory::make_unique<ast_function>(m_allocator, st_def);
  fn->m_return_type =
      get_reference_of(st_def, return_type, ast_type_classification::reference);
  fn->m_name = containers::string(m_allocator, constructor_name);

  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      get_reference_of(
          st_def, _initialized_type, ast_type_classification::reference),
  });

  containers::string mn(m_allocator, "P");
  char count[11] = {
      0,
  };
  memory::writeuint32(
      count, static_cast<uint32_t>(return_type->m_name.size()), 10);
  mn += count;
  mn += return_type->m_name;

  memory::writeuint32(
      count, static_cast<uint32_t>(constructor_name.size()), 10);

  containers::string mangled_name(m_allocator);
  mangled_name = containers::string(m_allocator, "_ZN3wns");
  mangled_name += count;
  mangled_name += constructor_name;
  mangled_name += "E";
  mangled_name += mn;

  mangled_name += "P";
  memory::writeuint32(
      count, static_cast<uint32_t>(_initialized_type->m_name.size()), 10);
  mangled_name += count;
  mangled_name += _initialized_type->m_name;

  fn->m_mangled_name = core::move(mangled_name);
  fn->m_scope = memory::make_unique<ast_scope_block>(m_allocator, st_def);
  auto& statements = fn->m_scope->initialized_statements(m_allocator);

  auto this_id = memory::make_unique<ast_id>(m_allocator, st_def);
  this_id->m_type = get_reference_of(
      st_def, _initialized_type, ast_type_classification::reference);
  this_id->m_function_parameter = &(params[0]);

  uint32_t pos = 0;
  uint32_t vtable_offset = 0xFFFFFFFF;
  if (_vtable) {
    auto member =
        memory::make_unique<ast_member_access_expression>(m_allocator, st_def);
    member->m_base_expression = clone_node(m_allocator, this_id.get());
    member->m_base_expression->m_type = get_reference_of(
        st_def, _initialized_type, ast_type_classification::reference);
    member->m_member_name = containers::string(m_allocator, "__vtable");
    member->m_member_offset = _vtable_index;
    vtable_offset = _vtable_index;
    member->m_type = m_vtable_t.get();

    auto vt_const = memory::make_unique<ast_constant>(m_allocator, st_def);
    vt_const->m_type = m_vtable_t.get();
    vt_const->m_node_value.m_vtable = _vtable;
    vt_const->m_string_value = containers::string(m_allocator, _vtable->m_name);

    auto assign = memory::make_unique<ast_assignment>(m_allocator, st_def);
    assign->m_lhs = core::move(member);
    assign->m_rhs = core::move(vt_const);
    statements.push_back(core::move(assign));
  }

  for (auto& st : _defs) {
    for (auto& it : st->get_struct_members()) {
      if (vtable_offset == pos) {
        ++pos;
      }
      auto member = memory::make_unique<ast_member_access_expression>(
          m_allocator, st_def);
      member->m_base_expression = clone_node(m_allocator, this_id.get());
      member->m_base_expression->m_type = get_reference_of(
          st_def, _initialized_type, ast_type_classification::reference);
      member->m_member_name = containers::string(m_allocator, it->get_name());
      member->m_member_offset = pos;

      memory::unique_ptr<ast_expression> rhs;

      ast_type* t = resolve_type(it->get_type());
      member->m_type = t;
      if (t->m_classification == ast_type_classification::reference &&
          t->m_implicitly_contained_type->m_struct_is_class) {
        if (!t->m_implicitly_contained_type ||
            t->m_implicitly_contained_type->m_classification !=
                ast_type_classification::struct_type) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Unknown reference base");
          return false;
        }

        // Easiest to resolve this expression, and then delete it;
        rhs = resolve_expression(it->get_expression());
        auto member_child = rhs->m_type->m_implicitly_contained_type;
        // auto member_type = rhs->m_type;

        auto constructor = member_child->m_constructor;
        if (!constructor) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot find constructor");
          return false;
        }
        auto construction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        construction_member->m_base_expression =
            clone_node(m_allocator, this_id.get());
        construction_member->m_base_expression->m_type = get_reference_of(
            st_def, _initialized_type, ast_type_classification::reference);
        containers::string nm = containers::string(m_allocator, it->get_name());
        nm.insert(nm.begin(), '_');

        construction_member->m_member_name = nm;
        uint32_t child_pos = 0;
        // child_pos cannot be 0, that would not work.
        for (auto& cp : _initialized_type->m_structure_members) {
          if (cp->m_name == nm) {
            cp->m_type = member_child;
            break;
          }
          child_pos++;
        }
        if (child_pos == _initialized_type->m_structure_members.size()) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot find reference source");
          return false;
        }
        construction_member->m_member_offset = child_pos;
        construction_member->m_type = member_child;

        auto cast =
            memory::make_unique<ast_cast_expression>(m_allocator, st_def);
        cast->m_type = get_reference_of(
            st_def, member_child, ast_type_classification::reference);
        cast->m_base_expression = core::move(construction_member);

        auto constructor_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        constructor_call->initialized_parameters(m_allocator)
            .push_back(core::move(cast));
        constructor_call->m_type = constructor->m_return_type;
        constructor_call->m_function = constructor;

        if (member->m_type != constructor_call->m_type) {
          rhs = make_cast(core::move(constructor_call), member->m_type);
        } else {
          rhs = core::move(constructor_call);
        }
      } else if (t->m_classification == ast_type_classification::reference) {
        member->m_type = member->m_type->m_implicitly_contained_type;
        rhs = resolve_expression(it->get_expression());
        auto member_child = rhs->m_type->m_implicitly_contained_type;
        auto constructor = member_child->m_constructor;
        if (!constructor) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot find constructor");
          return false;
        }

        auto constructor_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        constructor_call->initialized_parameters(m_allocator)
            .push_back(make_cast(
                core::move(member), get_reference_of(st_def, member_child,
                                        ast_type_classification::reference)));
        constructor_call->m_type = get_reference_of(st_def,
            t->m_implicitly_contained_type, ast_type_classification::reference);
        constructor_call->m_function = constructor;

        statements.push_back(evaluate_expression(core::move(constructor_call)));
        pos++;
        continue;
      } else {
        rhs = resolve_expression(it->get_expression());
        if (!rhs) {
          return false;
        }
      }
      auto assign = memory::make_unique<ast_assignment>(m_allocator, st_def);
      assign->m_lhs = core::move(member);
      assign->m_rhs = core::move(rhs);
      statements.push_back(core::move(assign));
      pos++;
    }
  }

  memory::unique_ptr<ast_expression> expr = core::move(this_id);
  if (returns_different_type) {
    auto cast = memory::make_unique<ast_cast_expression>(m_allocator, st_def);
    cast->m_type = get_reference_of(
        st_def, return_type, ast_type_classification::reference);
    cast->m_base_expression = core::move(expr);
    expr = core::move(cast);
  }

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, st_def);
  ret->m_return_expr = core::move(expr);
  statements.push_back(core::move(ret));

  _initialized_type->m_constructor = fn.get();
  fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());

  m_constructor_destructors.push_back(core::move(fn));
  return true;
}

bool parse_ast_convertor::convertor_context::create_destructor(
    const containers::deque<const struct_definition*>& _defs,
    ast_type* _initialized_type) {
  auto st_def = _defs.back();
  ast_type* return_type = _initialized_type->m_overloaded_construction_parent;

  containers::string destructor_name(m_allocator);
  destructor_name += "_destruct_";
  destructor_name += return_type->m_name;

  auto fn = memory::make_unique<ast_function>(m_allocator, st_def);
  fn->m_return_type = m_void_t.get();
  fn->m_name = containers::string(m_allocator, destructor_name);

  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      get_reference_of(
          st_def, _initialized_type, ast_type_classification::reference),
  });

  char count[11] = {
      0,
  };

  memory::writeuint32(count, static_cast<uint32_t>(destructor_name.size()), 10);

  containers::string mangled_name(m_allocator);
  mangled_name = containers::string(m_allocator, "_ZN3wns");
  mangled_name += count;
  mangled_name += destructor_name;
  mangled_name += "EvP";
  memory::writeuint32(
      count, static_cast<uint32_t>(_initialized_type->m_name.size()), 10);
  mangled_name += count;
  mangled_name += _initialized_type->m_name;

  fn->m_mangled_name = core::move(mangled_name);
  fn->m_scope = memory::make_unique<ast_scope_block>(m_allocator, st_def);
  auto& statements = fn->m_scope->initialized_statements(m_allocator);

  auto this_id = memory::make_unique<ast_id>(m_allocator, st_def);
  this_id->m_type = get_reference_of(
      st_def, _initialized_type, ast_type_classification::reference);
  this_id->m_function_parameter = &(params[0]);
  bool has_destructible_type = false;

  uint32_t pos = 0;
  for (auto& st : _defs) {
    for (auto& it : st->get_struct_members()) {
      ast_type* t = resolve_type(it->get_type());
      if (t->m_classification == ast_type_classification::reference) {
        if (!t->m_implicitly_contained_type ||
            t->m_implicitly_contained_type->m_classification !=
                ast_type_classification::struct_type) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Unknown reference base");
          return false;
        }
        containers::string nm = containers::string(m_allocator, it->get_name());
        nm.insert(nm.begin(), '_');

        uint32_t child_pos = 0;
        if (t->m_implicitly_contained_type->m_struct_is_class) {
          for (auto& cp : _initialized_type->m_structure_members) {
            if (cp->m_name == nm) {
              break;
            }
            child_pos++;
          }
          if (child_pos == _initialized_type->m_structure_members.size()) {
            st_def->log_line(m_log, logging::log_level::error);
            m_log->log_error("Cannot find reference source");
            return false;
          }
        } else {
          child_pos = pos;
        }

        auto& ct = _initialized_type->m_structure_members[child_pos];
        auto destructor = ct->m_type->m_destructor;
        if (!destructor) {
          continue;
        }
        has_destructible_type = true;

        auto destruction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        destruction_member->m_base_expression =
            clone_node(m_allocator, this_id.get());
        destruction_member->m_base_expression->m_type = get_reference_of(
            st_def, _initialized_type, ast_type_classification::reference);
        destruction_member->m_member_name = core::move(nm);
        destruction_member->m_member_offset = child_pos;
        destruction_member->m_type = ct->m_type;

        auto cast =
            memory::make_unique<ast_cast_expression>(m_allocator, st_def);
        cast->m_type = get_reference_of(
            st_def, ct->m_type, ast_type_classification::reference);
        cast->m_base_expression = core::move(destruction_member);

        auto destructor_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        destructor_call->initialized_parameters(m_allocator)
            .push_back(core::move(cast));
        destructor_call->m_type = m_void_t.get();
        destructor_call->m_function = destructor;

        auto expression_statement =
            memory::make_unique<ast_evaluate_expression>(m_allocator, st_def);
        expression_statement->m_expr = core::move(destructor_call);
        statements.push_back(core::move(expression_statement));
      }
      if (t->m_classification == ast_type_classification::shared_reference) {
        has_destructible_type = true;

        auto destruction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        destruction_member->m_base_expression =
            clone_node(m_allocator, this_id.get());
        destruction_member->m_base_expression->m_type = get_reference_of(
            st_def, _initialized_type, ast_type_classification::reference);
        destruction_member->m_member_name =
            it->get_name().to_string(m_allocator);
        destruction_member->m_member_offset = pos;
        destruction_member->m_type = t;

        memory::unique_ptr<ast_function_call_expression> function_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        function_call->m_function = m_release_shared;
        memory::unique_ptr<ast_cast_expression> cast =
            memory::make_unique<ast_cast_expression>(m_allocator, st_def);
        cast->m_base_expression = core::move(destruction_member);
        cast->m_type = m_void_ptr_t.get();

        function_call->initialized_parameters(m_allocator)
            .push_back(core::move(cast));
        function_call->m_type = m_void_t.get();

        auto expression_statement =
            memory::make_unique<ast_evaluate_expression>(m_allocator, st_def);
        expression_statement->m_expr = core::move(function_call);
        statements.push_back(core::move(expression_statement));
      }

      ++pos;
    }
  }

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, st_def);
  statements.push_back(core::move(ret));

  // If none of our types have destructors, we can avoid calling the
  // destructor. This can save a lot of time, since this
  // cascades.
  if (has_destructible_type) {
    fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());

    _initialized_type->m_destructor = fn.get();
    m_constructor_destructors.push_back(core::move(fn));
  }
  return true;
}

bool parse_ast_convertor::convertor_context::create_struct_assign(
    const struct_definition* _def, ast_type* _initialized_type) {
  WN_DEBUG_ASSERT(
      _initialized_type == _initialized_type->m_overloaded_construction_parent,
      "Invalid assignmnet type");
  WN_DEBUG_ASSERT(
      !_initialized_type->m_struct_is_class, "Invalid assignment of class");

  containers::string assign_name(m_allocator);
  assign_name += "_assign_";
  assign_name += _initialized_type->m_name;

  auto fn = memory::make_unique<ast_function>(m_allocator, _def);
  fn->m_return_type = m_void_t.get();
  fn->m_name = containers::string(m_allocator, assign_name);

  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      get_reference_of(
          _def, _initialized_type, ast_type_classification::reference),
  });
  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_other"),
      get_reference_of(
          _def, _initialized_type, ast_type_classification::reference),
  });

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
  auto& statements = fn->m_scope->initialized_statements(m_allocator);

  auto this_id = memory::make_unique<ast_id>(m_allocator, _def);
  this_id->m_type = get_reference_of(
      _def, _initialized_type, ast_type_classification::reference);
  this_id->m_function_parameter = &(params[0]);

  auto other_id = memory::make_unique<ast_id>(m_allocator, _def);
  other_id->m_type = get_reference_of(
      _def, _initialized_type, ast_type_classification::reference);
  other_id->m_function_parameter = &(params[1]);

  bool has_copyable_type = false;
  uint32_t pos = 0;

  for (auto& it : _initialized_type->m_structure_members) {
    const ast_type* t = it->m_type;

    auto copy_source =
        memory::make_unique<ast_member_access_expression>(m_allocator, _def);
    copy_source->m_base_expression = clone_node(m_allocator, other_id.get());
    copy_source->m_base_expression->m_type = get_reference_of(
        _def, _initialized_type, ast_type_classification::reference);
    copy_source->m_member_name = containers::string(m_allocator, it->m_name);
    copy_source->m_member_offset = pos;
    copy_source->m_type = t;

    auto copy_dest =
        memory::make_unique<ast_member_access_expression>(m_allocator, _def);
    copy_dest->m_base_expression = clone_node(m_allocator, this_id.get());
    copy_dest->m_base_expression->m_type = get_reference_of(
        _def, _initialized_type, ast_type_classification::reference);
    copy_dest->m_member_name = containers::string(m_allocator, it->m_name);
    copy_dest->m_member_offset = pos;
    copy_dest->m_type = t;

    if (t->m_classification == ast_type_classification::shared_reference) {
      has_copyable_type = true;

      containers::dynamic_array<memory::unique_ptr<ast_expression>>
          assign_params(m_allocator);
      assign_params.push_back(make_cast(
          clone_node(m_allocator, copy_dest.get()), m_void_ptr_t.get()));
      assign_params.push_back(
          make_cast(core::move(copy_source), m_void_ptr_t.get()));

      auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
      assign->m_lhs = core::move(copy_dest);
      assign->m_rhs = make_cast(
          call_function(_def, m_assign_shared, core::move(assign_params)),
          assign->m_lhs->m_type);
      statements.push_back(core::move(assign));
    } else {
      if (t->m_assignment) {
        has_copyable_type = true;
        auto ref_type =
            get_reference_of(_def, t, ast_type_classification::reference);
        containers::dynamic_array<memory::unique_ptr<ast_expression>>
            assign_params(m_allocator);
        assign_params.push_back(make_cast(core::move(copy_dest), ref_type));
        assign_params.push_back(make_cast(core::move(copy_source), ref_type));

        statements.push_back(evaluate_expression(
            call_function(_def, t->m_assignment, core::move(assign_params))));
      } else {
        auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
        assign->m_lhs = core::move(copy_dest);
        assign->m_rhs = core::move(copy_source);
        statements.push_back(core::move(assign));
      }
    }
    ++pos;
  }

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, _def);
  statements.push_back(core::move(ret));

  // If none of our types have destructors, we can avoid calling the
  // destructor. This can save a lot of time, since this
  // cascades.
  if (has_copyable_type) {
    fn->m_function_pointer_type = resolve_function_ptr_type(fn.get());

    _initialized_type->m_assignment = fn.get();
    m_constructor_destructors.push_back(core::move(fn));
  }
  return true;
}

}  // namespace scripting
}  // namespace wn