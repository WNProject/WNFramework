// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

  return core::move(m_value_overrides);
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
      m_log->log_error("Could not find parent type:", _def->get_parent_name());
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
  bool has_overloaded_construction = false;

  ast_vtable* vt = nullptr;
  uint32_t vtable_idx = 0;

  auto& decls = struct_type->initialized_structure_members(m_allocator);
  for (auto& t : type_chain) {
    // Leave this in. It is needed to register the parent types
    // usage.
    m_type_manager->get_or_register_struct(
        t->get_name(), &m_used_types, &m_used_externals);
    auto& functions = t->get_functions();
    for (size_t i = 0; i < t->get_functions().size(); ++i) {
      if (!vt && functions[i]->is_virtual()) {
        memory::unique_ptr<ast_declaration> decl =
            memory::make_unique<ast_declaration>(m_allocator, nullptr);
        decl->m_name = containers::string(m_allocator, "__vtable");
        decl->m_type = m_type_manager->vtable_t(&m_used_types);
        struct_type->m_vtable_index = static_cast<uint32_t>(decls.size());
        decls.push_back(core::move(decl));
        ast_vtable* vtable = m_type_manager->make_vtable(name);
        vt = vtable;
        struct_type->m_vtable = vt;
        m_script_file->m_all_vtables.push_back(vt);
      }
    }

    for (auto& it : t->get_struct_members()) {
      ast_declaration* decl = nullptr;
      if (!it->is_inherited()) {
        memory::unique_ptr<ast_declaration> u_decl =
            memory::make_unique<ast_declaration>(m_allocator, it.get());
        decl = u_decl.get();
        u_decl->m_name = containers::string(m_allocator, it->get_name());
        u_decl->m_type = resolve_type(it->get_type());
        if (u_decl->m_type == nullptr) {
          return nullptr;
        }
        decls.push_back(core::move(u_decl));
      } else {
        for (auto& de : decls) {
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
      }

      // If a unique value is actually a struct, (as opposed to a class)
      // we can inline the whole thing, no need to deal with indirection.
      if (decl->m_type->m_classification ==
          ast_type_classification::reference) {
        if (!decl->m_type->m_implicitly_contained_type->m_struct_is_class) {
          decl->m_type = decl->m_type->m_implicitly_contained_type;
        } else {
          has_overloaded_construction = true;
        }
      }

      if (decl->m_type->m_classification ==
              ast_type_classification::static_array &&
          decl->m_type->m_static_array_size == 0) {
        has_overloaded_construction = true;
        if (!struct_type->m_struct_is_class) {
          it->log_line(m_log, logging::log_level::error);
          m_log->log_error("Arrays in structs must be of fixed size");
          return nullptr;
        }
      }

      if (decl->m_type->m_classification ==
              ast_type_classification::static_array &&
          decl->m_type->m_implicitly_contained_type->m_classification ==
              ast_type_classification::reference &&
          decl->m_type->m_implicitly_contained_type->m_implicitly_contained_type
                  ->m_classification == ast_type_classification::struct_type &&
          !decl->m_type->m_implicitly_contained_type
               ->m_implicitly_contained_type->m_struct_is_class) {
        decl->m_type = get_array_of(decl->m_type->m_implicitly_contained_type
                                        ->m_implicitly_contained_type,
            decl->m_type->m_static_array_size);
      }

      if (has_overloaded_construction && !struct_type->m_struct_is_class) {
        it->log_line(m_log, logging::log_level::error);
        m_log->log_error(
            "A struct cannot contain a class member, or an array of them");
        return nullptr;
      }

      // Only inherited initializations can hide members.
      if (!it->is_inherited() &&
          !existing_member
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
    decl->m_type = m_type_manager->integral(8, &m_used_types);
    decls.push_back(core::move(decl));
    empty = true;
  }

  ast_type* child = nullptr;

  if (has_overloaded_construction) {
    containers::hash_map<containers::string_view, overriden_type>
        m_value_overrides = resolve_value_override(m_allocator, type_chain);

    // memory::unique_ptr<ast_type> child = struct_type->clone(m_allocator);
    containers::string child_name =
        containers::string(m_allocator, "_") + struct_type->m_name;
    child = m_type_manager->register_child_struct(
        child_name, struct_type, &m_used_types);

    auto& child_decls = child->initialized_structure_members(m_allocator);

    child->m_overloaded_construction_parent = struct_type;
    struct_type->m_overloaded_construction_child = child;
    for (auto& ty : type_chain) {
      for (auto& it : ty->get_struct_members()) {
        auto vo = m_value_overrides.find(it->get_name());
        if (vo != m_value_overrides.end() && vo->second.m_decl != it.get()) {
          continue;
        }

        const ast_type* t = nullptr;
        if (!it->is_inherited()) {
          t = resolve_type(it->get_type());
        } else {
          for (auto& de : decls) {
            if (de->m_name == it->get_name()) {
              t = de->m_type;
              break;
            }
          }
          if (!t) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error("Tried to override ", it->get_name(),
                " which does not", " exist in the parent class");
          }
        }

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
          // This is incorrect here. This will get overriden in the constructor.
          // and converted to the "correct" temporary type.
          decl->m_type =
              t->m_implicitly_contained_type->m_overloaded_construction_child;
          child_decls.push_back(core::move(decl));
        }

        if (t->m_classification == ast_type_classification::static_array &&
            t->m_static_array_size == 0) {
          memory::unique_ptr<ast_declaration> decl =
              memory::make_unique<ast_declaration>(m_allocator, it.get());
          decl->m_name = containers::string(m_allocator, "_") +
                         containers::string(m_allocator, it->get_name());
          decl->m_type = t;
          // This is incorrect here. This will get overriden in the constructor.
          // and converted to the "correct" temporary type.
          child_decls.push_back(core::move(decl));
        }
      }
    }
  } else {
    struct_type->m_overloaded_construction_child = struct_type;
    struct_type->m_overloaded_construction_parent = struct_type;
  }

  m_type_manager->set_struct_definition_resolved(_def->get_name(), struct_type);

  if (!empty) {
    if (!create_constructor(type_chain,
            struct_type->m_overloaded_construction_child, vt, vtable_idx)) {
      return nullptr;
    }
    if (!create_destructor(
            type_chain, struct_type->m_overloaded_construction_child)) {
      return nullptr;
    }
    if (!_def->is_class() && !create_struct_assign(_def, struct_type)) {
      return nullptr;
    }
  }
  return struct_type;
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
  fn->m_return_type = m_type_manager->get_reference_of(
      return_type, ast_type_classification::reference, &m_used_types);
  fn->m_name = containers::string(m_allocator, constructor_name);
  fn->m_is_member_function = true;
  m_current_function = fn.get();
  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      m_type_manager->get_reference_of(
          _initialized_type, ast_type_classification::reference, &m_used_types),
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
  containers::dynamic_array<memory::unique_ptr<ast_statement>>
      vtable_statements(m_allocator);

  auto this_id = memory::make_unique<ast_id>(m_allocator, st_def);
  this_id->m_type = m_type_manager->get_reference_of(
      _initialized_type, ast_type_classification::reference, &m_used_types);
  this_id->m_function_parameter = &(params[0]);

  uint32_t pos = 0;
  uint32_t vtable_offset = 0xFFFFFFFF;
  if (_vtable) {
    auto member =
        memory::make_unique<ast_member_access_expression>(m_allocator, st_def);
    member->m_base_expression = clone_ast_node(m_allocator, this_id.get());
    member->m_base_expression->m_type = m_type_manager->get_reference_of(
        _initialized_type, ast_type_classification::reference, &m_used_types);
    member->m_member_name = containers::string(m_allocator, "__vtable");
    member->m_member_offset = _vtable_index;
    vtable_offset = _vtable_index;
    member->m_type = m_type_manager->vtable_t(&m_used_types);

    auto vt_const = memory::make_unique<ast_constant>(m_allocator, st_def);
    vt_const->m_type = m_type_manager->vtable_t(&m_used_types);
    vt_const->m_node_value.m_vtable = _vtable;
    vt_const->m_string_value = containers::string(m_allocator, _vtable->m_name);

    auto assign = memory::make_unique<ast_assignment>(m_allocator, st_def);
    assign->m_lhs = core::move(member);
    assign->m_rhs = core::move(vt_const);
    vtable_statements.push_back(core::move(assign));
  }

  containers::hash_map<containers::string_view, overriden_type>
      m_value_overrides = resolve_value_override(m_allocator, _defs);
  containers::dynamic_array<memory::unique_ptr<ast_statement>>
      class_level_pre_init_statements(m_allocator);

  for (auto& st : _defs) {
    for (auto& it : st->get_struct_members()) {
      if (vtable_offset == pos) {
        ++pos;
      }
      auto overrider = m_value_overrides.find(it->get_name());

      if (overrider != m_value_overrides.end() &&
          overrider->second.m_decl != it.get()) {
        // We are overriden by something down the chain.
        // Are we the first thing in that chain?
        if (!it->is_inherited()) {
          overrider->second.m_member_offset = pos;
          overrider->second.m_type = it->get_type();
          ++pos;
          continue;
        }
      }

      bool overrides = (overrider != m_value_overrides.end() &&
                        overrider->second.m_decl == it.get());
      auto increment_pos = [&overrides, &pos]() {
        if (!overrides) {
          ++pos;
        }
      };

      auto add_statement = [&overrides, &class_level_pre_init_statements,
                               &statements](
                               memory::unique_ptr<ast_statement> _statement) {
        if (overrides) {
          class_level_pre_init_statements.push_back(core::move(_statement));
        } else {
          statements.push_back(core::move(_statement));
        }
      };

      uint32_t use_pos = overrides ? overrider->second.m_member_offset : pos;
      const ast_type* t =
          resolve_type(overrides ? overrider->second.m_type : it->get_type());

      if (t->m_classification == ast_type_classification::static_array &&
          t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::reference &&
          t->m_implicitly_contained_type->m_implicitly_contained_type
                  ->m_classification == ast_type_classification::struct_type &&
          !t->m_implicitly_contained_type->m_implicitly_contained_type
               ->m_struct_is_class) {
        t = get_array_of(
            t->m_implicitly_contained_type->m_implicitly_contained_type,
            t->m_static_array_size);
      }
      if (t->m_classification == ast_type_classification::runtime_array &&
          t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::reference &&
          t->m_implicitly_contained_type->m_implicitly_contained_type
                  ->m_classification == ast_type_classification::struct_type &&
          !t->m_implicitly_contained_type->m_implicitly_contained_type
               ->m_struct_is_class) {
        t = get_runtime_array_of(
            t->m_implicitly_contained_type->m_implicitly_contained_type);
      }

      auto member = memory::make_unique<ast_member_access_expression>(
          m_allocator, st_def);
      member->m_base_expression = clone_ast_node(m_allocator, this_id.get());
      member->m_base_expression->m_type = m_type_manager->get_reference_of(
          _initialized_type, ast_type_classification::reference, &m_used_types);
      member->m_member_name = containers::string(m_allocator, it->get_name());
      member->m_member_offset = use_pos;

      memory::unique_ptr<ast_expression> rhs;

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
            clone_ast_node(m_allocator, this_id.get());
        construction_member->m_base_expression->m_type =
            m_type_manager->get_reference_of(_initialized_type,
                ast_type_classification::reference, &m_used_types);
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
        cast->m_type = m_type_manager->get_reference_of(
            member_child, ast_type_classification::reference, &m_used_types);
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
        use_function(constructor);
        if (!constructor) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot find constructor");
          return false;
        }

        auto constructor_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        constructor_call->initialized_parameters(m_allocator)
            .push_back(make_cast(core::move(member),
                m_type_manager->get_reference_of(member_child,
                    ast_type_classification::reference, &m_used_types)));
        constructor_call->m_type =
            m_type_manager->get_reference_of(t->m_implicitly_contained_type,
                ast_type_classification::reference, &m_used_types);
        constructor_call->m_function = constructor;

        add_statement(evaluate_expression(core::move(constructor_call)));
        increment_pos();
        continue;
      } else if (t->m_classification == ast_type_classification::static_array ||
                 t->m_classification ==
                     ast_type_classification::runtime_array) {
        bool is_runtime_array =
            t->m_classification == ast_type_classification::runtime_array;
        // Easiest to resolve this expression, and then delete it;
        rhs = resolve_expression(it->get_expression());

        if (rhs->m_temporaries.size() != 2 ||
            (rhs->m_temporaries[0]->m_node_type !=
                    ast_node_type::ast_declaration &&
                rhs->m_temporaries[1]->m_node_type !=
                    ast_node_type::ast_array_allocation)) {
          st_def->log_line(m_log, logging::log_level::error);
          m_log->log_error("Invalid array initializer");
          return false;
        }
        ast_array_allocation* array_init =
            cast_to<ast_array_allocation>(rhs->m_temporaries[1].get());
        if (t->m_implicitly_contained_type->m_classification ==
            ast_type_classification::struct_type) {
          // We need to just construct the final element, not a temporary.
          if (array_init->m_initializer->m_node_type !=
              ast_node_type::ast_function_call_expression) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error("Expected constructor as array intializer");
            return false;
          }

          ast_function_call_expression* constructor =
              cast_to<ast_function_call_expression>(
                  array_init->m_initializer.get());
          constructor->m_parameters[0] =
              clone_ast_node(m_allocator, member.get());
          array_init->m_constructor_initializer =
              memory::unique_ptr<ast_function_call_expression>(
                  array_init->m_initializer.get_allocator(), constructor);
          array_init->m_initializer.release();
        } else if (t->m_implicitly_contained_type->m_classification ==
                   ast_type_classification::shared_reference) {
          if (!array_init->m_initializer->m_type->can_implicitly_cast_to(
                  t->m_implicitly_contained_type)) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error("Invalid shared array initializer");
            return false;
          }
          bool is_shared_null_assign = array_init->m_initializer->m_type ==
                                       m_type_manager->nullptr_t(&m_used_types);
          if (!is_shared_null_assign) {
            auto const_null =
                memory::make_unique<ast_constant>(m_allocator, it.get());
            const_null->m_string_value = containers::string(m_allocator, "");
            const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
            auto null_as_vptr = make_cast(core::move(const_null),
                m_type_manager->void_ptr_t(&m_used_types));

            containers::dynamic_array<memory::unique_ptr<ast_expression>>
                construct_params(m_allocator);
            construct_params.push_back(core::move(null_as_vptr));
            construct_params.push_back(
                core::move(make_cast(core::move(array_init->m_initializer),
                    m_type_manager->void_ptr_t(&m_used_types))));
            array_init->m_initializer =
                make_cast(call_function(it.get(),
                              m_type_manager->assign_shared(&m_used_builtins),
                              core::move(construct_params)),
                    t->m_implicitly_contained_type);
          }
        }

        if (t->m_static_array_size == 0 && !is_runtime_array) {
          auto member_child = rhs->m_type;
          auto construction_member =
              memory::make_unique<ast_member_access_expression>(
                  m_allocator, st_def);
          construction_member->m_base_expression =
              clone_ast_node(m_allocator, this_id.get());
          construction_member->m_base_expression->m_type =
              m_type_manager->get_reference_of(_initialized_type,
                  ast_type_classification::reference, &m_used_types);

          containers::string nm =
              containers::string(m_allocator, it->get_name());
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

          auto constructed_id =
              clone_ast_node(m_allocator, construction_member.get());

          cast_to<ast_array_allocation>(rhs->m_temporaries[1].get())
              ->m_initializee = core::move(construction_member);

          add_statement(core::move(rhs->m_temporaries[1]));

          auto cast =
              memory::make_unique<ast_cast_expression>(m_allocator, st_def);
          cast->m_type = t;
          cast->m_base_expression = core::move(constructed_id);
          rhs = core::move(cast);
        } else {  // t->m_static_array_size != 0
          if (rhs->m_temporaries.size() != 2 ||
              (rhs->m_temporaries[0]->m_node_type !=
                      ast_node_type::ast_declaration &&
                  rhs->m_temporaries[1]->m_node_type !=
                      ast_node_type::ast_array_allocation)) {
            st_def->log_line(m_log, logging::log_level::error);
            m_log->log_error("Invalid array initializer");
            return false;
          }
          if (member->m_type != rhs->m_type) {
            it->log_line(m_log, logging::log_level::error);
            m_log->log_error("Invalid array initializer");
            return false;
          }
          if (is_runtime_array) {
            auto allocate_runtime =
                memory::make_unique<ast_function_call_expression>(
                    m_allocator, st_def);
            allocate_runtime->m_function =
                m_external_functions[containers::string(
                    m_allocator, "_allocate_runtime_array")];

            auto num_bytes = memory::make_unique<ast_builtin_expression>(
                m_allocator, st_def);
            num_bytes->m_type = m_type_manager->size_t(&m_used_types);
            num_bytes->initialized_extra_types(m_allocator)
                .push_back(t->m_implicitly_contained_type);
            num_bytes->m_builtin_type = builtin_expression_type::size_of;

            auto array_count = make_cast(
                clone_ast_node(m_allocator, array_init->m_runtime_size.get()),
                m_type_manager->size_t(&m_used_types));
            allocate_runtime->initialized_parameters(m_allocator)
                .push_back(core::move(num_bytes));
            allocate_runtime->initialized_parameters(m_allocator)
                .push_back(core::move(array_count));
            allocate_runtime->m_type =
                m_type_manager->void_ptr_t(&m_used_types);

            auto assign =
                memory::make_unique<ast_assignment>(m_allocator, st_def);
            assign->m_lhs = clone_ast_node(m_allocator, member.get());
            assign->m_rhs = make_cast(core::move(allocate_runtime), t);
            add_statement(core::move(assign));
          }

          array_init->m_initializee = core::move(member);
          add_statement(core::move(rhs->m_temporaries[1]));
          increment_pos();
          continue;
        }
      } else {
        rhs = resolve_expression(it->get_expression());
        if (!rhs) {
          return false;
        }
        if (t != rhs->m_type) {
          if (rhs->m_type->can_implicitly_cast_to(t)) {
            auto cast =
                memory::make_unique<ast_cast_expression>(m_allocator, it.get());
            cast->m_type = t;
            cast->m_base_expression = core::move(rhs);
            rhs = core::move(cast);
          }
        }
      }
      if (member->m_type != rhs->m_type) {
        it->log_line(m_log, logging::log_level::error);
        m_log->log_error("Trying to initialize an invalid type");
        return false;
      }
      auto assign = memory::make_unique<ast_assignment>(m_allocator, st_def);
      assign->m_lhs = core::move(member);
      assign->m_rhs = core::move(rhs);
      add_statement(core::move(assign));
      increment_pos();
    }
    for (auto stit = class_level_pre_init_statements.rbegin();
         stit != class_level_pre_init_statements.rend(); ++stit) {
      statements.push_front(core::move(*stit));
    }
    class_level_pre_init_statements.clear();
  }

  for (auto vtit = vtable_statements.rbegin(); vtit != vtable_statements.rend();
       ++vtit) {
    statements.push_front(core::move(*vtit));
  }

  memory::unique_ptr<ast_expression> expr = core::move(this_id);
  if (returns_different_type) {
    auto cast = memory::make_unique<ast_cast_expression>(m_allocator, st_def);
    cast->m_type = m_type_manager->get_reference_of(
        return_type, ast_type_classification::reference, &m_used_types);
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
  fn->m_return_type = m_type_manager->void_t(&m_used_types);
  fn->m_name = containers::string(m_allocator, destructor_name);

  auto& params = fn->initialized_parameters(m_allocator);

  params.push_back(ast_function::parameter{
      containers::string(m_allocator, "_this"),
      m_type_manager->get_reference_of(
          _initialized_type, ast_type_classification::reference, &m_used_types),
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
  this_id->m_type = m_type_manager->get_reference_of(
      _initialized_type, ast_type_classification::reference, &m_used_types);
  this_id->m_function_parameter = &(params[0]);
  bool has_destructible_type = false;

  uint32_t pos = 0;

  containers::hash_map<containers::string_view, overriden_type>
      m_value_overrides = resolve_value_override(m_allocator, _defs);
  containers::dynamic_array<memory::unique_ptr<ast_statement>>
      class_level_pre_init_statements(m_allocator);

  for (auto& st : _defs) {
    for (auto& it : st->get_struct_members()) {
      auto overrider = m_value_overrides.find(it->get_name());

      if (overrider != m_value_overrides.end() &&
          overrider->second.m_decl != it.get()) {
        // We are overriden by something down the chain.
        // Are we the first thing in that chain?
        if (!it->is_inherited()) {
          overrider->second.m_member_offset = pos;
          overrider->second.m_type = it->get_type();
          ++pos;
          continue;
        }
      }

      bool overrides = (overrider != m_value_overrides.end() &&
                        overrider->second.m_decl == it.get());
      auto increment_pos = [&overrides, &pos]() {
        if (!overrides) {
          ++pos;
        }
      };

      auto add_statement = [&overrides, &class_level_pre_init_statements,
                               &statements](
                               memory::unique_ptr<ast_statement> _statement) {
        if (overrides) {
          class_level_pre_init_statements.push_back(core::move(_statement));
        } else {
          statements.push_back(core::move(_statement));
        }
      };

      uint32_t use_pos = overrides ? overrider->second.m_member_offset : pos;
      const ast_type* t =
          resolve_type(overrides ? overrider->second.m_type : it->get_type());

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
          child_pos = use_pos;
        }

        auto& ct = _initialized_type->m_structure_members[child_pos];
        auto destructor = ct->m_type->m_destructor;
        use_function(destructor);
        if (!destructor) {
          continue;
        }
        has_destructible_type = true;

        auto destruction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        destruction_member->m_base_expression =
            clone_ast_node(m_allocator, this_id.get());
        destruction_member->m_base_expression->m_type =
            m_type_manager->get_reference_of(_initialized_type,
                ast_type_classification::reference, &m_used_types);
        destruction_member->m_member_name = core::move(nm);
        destruction_member->m_member_offset = child_pos;
        destruction_member->m_type = ct->m_type;

        auto cast =
            memory::make_unique<ast_cast_expression>(m_allocator, st_def);
        cast->m_type = m_type_manager->get_reference_of(
            ct->m_type, ast_type_classification::reference, &m_used_types);
        cast->m_base_expression = core::move(destruction_member);

        auto destructor_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        destructor_call->initialized_parameters(m_allocator)
            .push_back(core::move(cast));
        destructor_call->m_type = m_type_manager->void_t(&m_used_types);
        destructor_call->m_function = destructor;

        auto expression_statement =
            memory::make_unique<ast_evaluate_expression>(m_allocator, st_def);
        expression_statement->m_expr = core::move(destructor_call);
        add_statement(core::move(expression_statement));
      }
      if (t->m_classification == ast_type_classification::shared_reference) {
        has_destructible_type = true;

        auto destruction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        destruction_member->m_base_expression =
            clone_ast_node(m_allocator, this_id.get());
        destruction_member->m_base_expression->m_type =
            m_type_manager->get_reference_of(_initialized_type,
                ast_type_classification::reference, &m_used_types);
        destruction_member->m_member_name =
            it->get_name().to_string(m_allocator);
        destruction_member->m_member_offset = use_pos;
        destruction_member->m_type = t;

        memory::unique_ptr<ast_function_call_expression> function_call =
            memory::make_unique<ast_function_call_expression>(
                m_allocator, st_def);
        function_call->m_function =
            m_type_manager->release_shared(&m_used_builtins);
        memory::unique_ptr<ast_cast_expression> cast =
            memory::make_unique<ast_cast_expression>(m_allocator, st_def);
        cast->m_base_expression = core::move(destruction_member);
        cast->m_type = m_type_manager->void_ptr_t(&m_used_types);

        function_call->initialized_parameters(m_allocator)
            .push_back(core::move(cast));
        function_call->m_type = m_type_manager->void_t(&m_used_types);

        auto expression_statement =
            memory::make_unique<ast_evaluate_expression>(m_allocator, st_def);
        expression_statement->m_expr = core::move(function_call);
        add_statement(core::move(expression_statement));
      }
      if (t->m_classification == ast_type_classification::static_array ||
          t->m_classification == ast_type_classification::runtime_array) {
        if (t->m_implicitly_contained_type->m_classification ==
                ast_type_classification::shared_reference ||
            (t->m_implicitly_contained_type->m_classification ==
                    ast_type_classification::struct_type &&
                t->m_implicitly_contained_type->m_destructor)) {
          has_destructible_type = true;
          memory::unique_ptr<ast_array_destruction> dest =
              memory::make_unique<ast_array_destruction>(m_allocator, st_def);
          if (t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::shared_reference) {
            dest->m_destructor =
                m_type_manager->release_shared(&m_used_builtins);
            dest->m_shared = true;
          } else {
            dest->m_destructor = t->m_implicitly_contained_type->m_destructor;
            use_function(dest->m_destructor);
          }
          auto destruction_member =
              memory::make_unique<ast_member_access_expression>(
                  m_allocator, st_def);
          destruction_member->m_base_expression =
              clone_ast_node(m_allocator, this_id.get());
          destruction_member->m_base_expression->m_type =
              m_type_manager->get_reference_of(_initialized_type,
                  ast_type_classification::reference, &m_used_types);
          destruction_member->m_member_name =
              it->get_name().to_string(m_allocator);
          destruction_member->m_member_offset = use_pos;
          destruction_member->m_type = t;

          dest->m_target = core::move(destruction_member);
          add_statement(core::move(dest));
        }
      }
      if (t->m_classification == ast_type_classification::runtime_array) {
        has_destructible_type = true;
        auto array_free = memory::make_unique<ast_function_call_expression>(
            m_allocator, st_def);
        array_free->m_function =
            m_external_functions[containers::string(m_allocator, "_free")];

        auto destruction_member =
            memory::make_unique<ast_member_access_expression>(
                m_allocator, st_def);
        destruction_member->m_base_expression =
            clone_ast_node(m_allocator, this_id.get());
        destruction_member->m_base_expression->m_type =
            m_type_manager->get_reference_of(_initialized_type,
                ast_type_classification::reference, &m_used_types);
        destruction_member->m_member_name =
            it->get_name().to_string(m_allocator);
        destruction_member->m_member_offset = use_pos;
        destruction_member->m_type = t;

        array_free->initialized_parameters(m_allocator)
            .push_back(core::move(make_cast(core::move(destruction_member),
                m_type_manager->void_ptr_t(&m_used_types))));
        array_free->m_type = m_type_manager->void_t(&m_used_types);
        auto dest_c =
            memory::make_unique<ast_evaluate_expression>(m_allocator, st_def);
        dest_c->m_expr = core::move(array_free);
        add_statement(core::move(dest_c));
      }
      increment_pos();
    }

    for (auto stit = class_level_pre_init_statements.rbegin();
         stit != class_level_pre_init_statements.rend(); ++stit) {
      statements.push_front(core::move(*stit));
    }
    class_level_pre_init_statements.clear();
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

      auto if_not_new = memory::make_unique<ast_if_chain>(m_allocator, _def);
      if_not_new->initialized_conditionals(m_allocator)
          .push_back(ast_if_chain::conditional_block{core::move(not_new),
              memory::make_unique<ast_scope_block>(m_allocator, _def)});

      containers::string temp = get_next_temporary_name();
      memory::unique_ptr<ast_declaration> decl =
          memory::make_unique<ast_declaration>(m_allocator, nullptr);
      decl->m_name = temp;
      decl->m_type = m_type_manager->bool_t(&m_used_types);

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

      statements =
          &if_not_new->m_conditionals[0].m_scope->initialized_statements(
              m_allocator);
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
        size_doesnt_match->m_lhs = core::move(source_size);
        size_doesnt_match->m_rhs = core::move(dest_size);

        auto if_size_doesnt_match =
            memory::make_unique<ast_if_chain>(m_allocator, _def);
        if_size_doesnt_match->initialized_conditionals(m_allocator)
            .push_back(
                ast_if_chain::conditional_block{core::move(size_doesnt_match),
                    memory::make_unique<ast_scope_block>(m_allocator, _def)});

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
        if_size_doesnt_match->m_conditionals[0]
            .m_scope->initialized_statements(m_allocator)
            .push_back(core::move(dest_c));

        // else don't create a new one
        if_size_doesnt_match->m_else_block =
            memory::make_unique<ast_scope_block>(m_allocator, _def);

        auto const_false = memory::make_unique<ast_constant>(m_allocator, _def);
        const_false->m_type = m_type_manager->bool_t(&m_used_types);
        const_false->m_string_value = containers::string(m_allocator, "false");
        const_false->m_node_value.m_bool = false;

        auto assign = memory::make_unique<ast_assignment>(m_allocator, _def);
        assign->m_lhs = clone_ast_node(m_allocator, create_new_id.get());
        assign->m_rhs = core::move(const_false);
        if_size_doesnt_match->m_else_block->initialized_statements(m_allocator)
            .push_back(core::move(assign));
        statements->push_back(core::move(if_size_doesnt_match));
      }
      old_statements->push_back(core::move(if_not_new));

      auto if_create_new = memory::make_unique<ast_if_chain>(m_allocator, _def);
      if_create_new->initialized_conditionals(m_allocator)
          .push_back(ast_if_chain::conditional_block{core::move(create_new_id),
              memory::make_unique<ast_scope_block>(m_allocator, _def)});
      statements =
          &if_create_new->m_conditionals[0].m_scope->initialized_statements(
              m_allocator);

      auto allocate_runtime =
          memory::make_unique<ast_function_call_expression>(m_allocator, _def);
      allocate_runtime->m_function = m_external_functions[containers::string(
          m_allocator, "_allocate_runtime_array")];

      auto num_bytes =
          memory::make_unique<ast_builtin_expression>(m_allocator, _def);
      num_bytes->m_type = m_type_manager->size_t(&m_used_types);
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
          .push_back(make_cast(
              core::move(array_count), m_type_manager->size_t(&m_used_types)));
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
          memory::make_unique<ast_declaration>(m_allocator, nullptr);
      decl->m_name = temp;
      decl->m_type = m_type_manager->integral(32, &m_used_types);
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

        auto if_new = memory::make_unique<ast_if_chain>(m_allocator, _def);
        if_new->initialized_conditionals(m_allocator)
            .push_back(ast_if_chain::conditional_block{core::move(is_new),
                memory::make_unique<ast_scope_block>(m_allocator, _def)});
        if_new->m_else_block =
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
          if_new->m_conditionals[0]
              .m_scope->initialized_statements(m_allocator)
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
          if_new->m_else_block->initialized_statements(m_allocator)
              .push_back(core::move(assign));
        }
        statements->push_back(core::move(if_new));
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