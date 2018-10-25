// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

parse_ast_convertor::convertor_context::convertor_context(
    memory::allocator* _allocator, logging::log* _log,
    type_manager* _type_manager, const parse_ast_convertor* _convertor)
  : m_allocator(_allocator),
    m_convertor(_convertor),
    m_used_types(_allocator),
    m_used_builtins(_allocator),
    m_used_externals(_allocator),
    m_nested_scopes(_allocator),
    m_constructor_destructors(_allocator),
    m_temporary_cleanup(_allocator),
    m_external_functions(_allocator),
    m_named_functions(_allocator),
    m_current_loop(nullptr),
    m_temporary_number(0),
    m_log(_log),
    m_type_manager(_type_manager) {}

memory::unique_ptr<ast_script_file>
parse_ast_convertor::convert_parse_tree_to_ast(memory::allocator* _allocator,
    type_manager* _type_manager, wn::logging::log* _log,
    functional::function<bool(containers::string_view)> _handle_includes,
    const script_file* _file) const {
  convertor_context context(_allocator, _log, _type_manager, this);
  for (auto& inc : _file->get_includes()) {
    if (!_handle_includes(inc.to_string_view().substr(1, inc.size() - 2))) {
      return nullptr;
    }
  }
  if (!context.walk_script_file(_file)) {
    return nullptr;
  }

  return core::move(context.m_script_file);
}

bool parse_ast_convertor::convertor_context::resolve_member_functions(
    ast_type* _type) {
  if (m_type_manager->member_functions_resolved(_type->m_name)) {
    return true;
  }

  if (_type->m_parent_type) {
    if (!resolve_member_functions(_type->m_parent_type)) {
      return false;
    }
    auto member_functions = core::move(_type->m_member_functions);
    wn::containers::deque<ast_function*> additional_functions(m_allocator);
    _type->m_member_functions = containers::deque<ast_function*>(
        m_allocator, _type->m_parent_type->m_member_functions);
    for (auto mf : member_functions) {
      bool handled = false;
      for (auto& f : _type->m_member_functions) {
        if (mf->m_name == f->m_name &&
            mf->m_parameters.size() == f->m_parameters.size()) {
          bool same = true;
          bool same_as_virtual = false;
          bool is_convertible_to = true;
          for (size_t i = 1; i < mf->m_parameters.size(); ++i) {
            if (mf->m_parameters[i].m_type != f->m_parameters[i].m_type) {
              same = false;
            }
            if (!mf->m_parameters[i].m_type->can_implicitly_cast_to(
                    f->m_parameters[i].m_type) &&
                !f->m_parameters[i].m_type->can_implicitly_cast_to(
                    mf->m_parameters[i].m_type)) {
              is_convertible_to = false;
            }
          }
          if (!is_convertible_to) {
            continue;
          }

          if (f->m_is_override || f->m_is_virtual) {
            same_as_virtual = true;
          }
          if ((is_convertible_to && !same) ||
              (same && !mf->m_is_override && !same_as_virtual)) {
            mf->log_line(m_log, logging::log_level::error);
            m_log->log_error("Definition of ", mf->m_name,
                " hides definition in parent class");
            return false;
          }
          if (same_as_virtual && mf->m_is_override) {
            f = mf;
            handled = true;
            break;
          }
        }
      }
      if (!handled) {
        // If we got here then the function doesn't override anything.
        if (mf->m_is_override) {
          mf->log_line(m_log, logging::log_level::error);
          m_log->log_error("Definition of ", mf->m_name,
              " marked override but does not override parent class");
          return false;
        }
        additional_functions.push_back(mf);
      }
    }

    for (auto& af : additional_functions) {
      _type->m_member_functions.push_back(af);
    }
  }

  uint32_t vtable_index = 0;
  // Time to build the vtable!
  for (auto af : _type->m_member_functions) {
    if (af->m_is_virtual || af->m_is_override) {
      WN_DEBUG_ASSERT(_type->m_vtable,
          "Impossible to have virtual functions without a vtable");
      if (af->m_virtual_index == 0xFFFFFFFF) {
        af->m_virtual_index = vtable_index;
      }
      WN_DEBUG_ASSERT(
          af->m_virtual_index == vtable_index, "Out of sync vtables");

      _type->m_vtable->initialized_functions(m_allocator).push_back(af);
      ++vtable_index;
    }
  }

  m_type_manager->set_member_functions_resolved(_type->m_name);
  return true;
}  // namespace scripting

bool parse_ast_convertor::convertor_context::walk_script_file(
    const script_file* _file) {
  m_script_file =
      memory::make_unique<ast_script_file>(m_allocator, _file, m_allocator);

  for (auto& ext : m_type_manager->external_functions()) {
    m_external_functions[ext->m_name] = ext.get();
    auto it = m_named_functions.find(ext->m_name);
    if (it == m_named_functions.end()) {
      it = m_named_functions
               .insert(core::make_pair(ext->m_name,
                   containers::deque<const ast_function*>(m_allocator)))
               .first;
    }
    it->second.push_back(ext.get());
  }

  for (auto& type : _file->get_structs()) {
    if (!m_type_manager->register_struct_definition(type.get())) {
      type->log_line(m_log, logging::log_level::error);
      return false;
    }
    (void)m_type_manager->get_or_register_struct(
        type->get_name(), &m_used_types, &m_used_externals);
  }

  for (auto& type : _file->get_structs()) {
    if (!walk_struct_definition(type.get())) {
      return false;
    }
  }

  containers::deque<core::pair<const function*, ast_function*>> functions(
      m_allocator);

  for (auto& fn : m_constructor_destructors) {
    m_script_file->m_functions.push_back(core::move(fn));
  }

  m_constructor_destructors.clear();

  for (auto& type : _file->get_structs()) {
    for (auto& f : type->get_functions()) {
      auto st_type = m_type_manager->get_or_register_struct(
          type->get_name(), &m_used_types, &m_used_externals);

      auto fun = pre_resolve_function(
          f.get(), m_type_manager->get_reference_of(st_type,
                       ast_type_classification::reference, &m_used_types));
      if (!fun) {
        return false;
      }

      st_type->initialized_member_functions(m_allocator).push_back(fun.get());
      functions.push_back(core::make_pair(f.get(), fun.get()));
      m_script_file->m_functions.push_back(core::move(fun));
    }
  }

  // Now that all member functions are initialized, set up all member
  // functions on all struct types.
  for (auto& type : _file->get_structs()) {
    auto st_type = m_type_manager->get_or_register_struct(
        type->get_name(), &m_used_types, &m_used_externals);
    if (!resolve_member_functions(st_type)) {
      return false;
    }
  }

  for (auto& function : _file->get_functions()) {
    auto fun = pre_resolve_function(function.get());
    if (!fun) {
      return false;
    }
    functions.push_back(core::make_pair(function.get(), fun.get()));
    m_script_file->m_functions.push_back(core::move(fun));
  }

  for (auto& function : functions) {
    if (!resolve_function(function.first, function.second)) {
      return false;
    }
  }

  m_script_file->m_initialization_order =
      m_type_manager->get_initialialization_order(m_allocator, m_used_types);

  m_type_manager->finalize_used_externals(
      &m_used_externals, &m_script_file->m_used_externals);

  m_type_manager->finalize_functions(m_script_file->m_functions);
  // We use finalize_builtin_functions so that the builtins in the
  // script file will be strongly ordered. Otherwise the hash_set
  // would be unordered, and we would end up with non-deterministic
  // outputs.
  m_type_manager->finalize_builtin_functions(
      &m_used_builtins, &m_script_file->m_used_builtins);

  return true;
}

const ast_type* parse_ast_convertor::convertor_context::get_array_of(
    const ast_type* _type, uint32_t _size) {
  ast_type* tp = m_type_manager->get_array_of(_type, _size, &m_used_types);
  return tp;
}

const ast_type* parse_ast_convertor::convertor_context::get_runtime_array_of(
    const ast_type* _type) {
  ast_type* tp = m_type_manager->get_runtime_array_of(_type, &m_used_types);
  return tp;
}

const ast_type* parse_ast_convertor::convertor_context::get_slice_of(
    const ast_type* _type, uint32_t _size) {
  ast_type* tp = m_type_manager->get_slice_of(_type, _size, & m_used_types);
  return tp;
}

}  // namespace scripting
}  // namespace wn