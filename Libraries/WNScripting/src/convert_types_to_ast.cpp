// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

ast_type* parse_ast_convertor::convertor_context::resolve_type(
    const type* _type) {
  if (_type->get_node_type() == node_type::array_type) {
    return resolve_static_array(cast_to<array_type>(_type));
  } else if (_type->get_node_type() == node_type::dynamic_array_type) {
    return resolve_dynamic_array(cast_to<dynamic_array_type>(_type));
  }
  WN_DEBUG_ASSERT(_type->get_node_type() == node_type::type,
      "Type nodes must be of a known type");

  if (_type->get_reference_type() == reference_type::unique ||
      _type->get_reference_type() == reference_type::shared ||
      _type->get_reference_type() == reference_type::weak) {
    WN_RELEASE_ASSERT(_type->get_reference_type() != reference_type::weak,
        "Not implemented: Weak references");
    return resolve_reference_type(_type);
  }

  if (!_type->custom_type_name().empty()) {
    auto it = m_struct_definitions.find(
        _type->custom_type_name().to_string(m_allocator));
    if (it == m_struct_definitions.end()) {
      auto extern_it = m_type_manager->m_external_types.find(
          _type->custom_type_name().to_string(m_allocator));
      if (extern_it == m_type_manager->m_external_types.end()) {
        _type->log_line(m_log, logging::log_level::error);
        m_log->log_error("Could not find type definition");
        return nullptr;
      }
      return extern_it->second.get();
    }
    return walk_struct_definition(it->second);
  }
  WN_DEBUG_ASSERT(_type->get_reference_type() == reference_type::raw,
      "No known struct type can get us here");

  return resolve_builtin_type(_type->get_classification());
}

ast_type* parse_ast_convertor::convertor_context::resolve_builtin_type(
    const uint32_t _type) {
  switch (_type) {
    case static_cast<uint32_t>(type_classification::void_type):
      return m_type_manager->m_void_t.get();
    case static_cast<uint32_t>(type_classification::int_type):
      return m_type_manager->m_integral_types[32].get();
    case static_cast<uint32_t>(type_classification::float_type):
      return m_type_manager->m_float_types[32].get();
    case static_cast<uint32_t>(type_classification::char_type):
      return m_type_manager->m_integral_types[8].get();
    case static_cast<uint32_t>(type_classification::bool_type):
      return m_type_manager->m_bool_t.get();
    case static_cast<uint32_t>(type_classification::size_type):
      return m_type_manager->m_size_t.get();
    case static_cast<uint32_t>(type_classification::void_ptr_type):
      return m_type_manager->m_void_ptr_t.get();
    case static_cast<uint32_t>(type_classification::function_ptr_type):
      return m_type_manager->m_function_t.get();
    default:
      WN_DEBUG_ASSERT(false, "Unknown type index");
  }
  return nullptr;
}

ast_type* parse_ast_convertor::convertor_context::resolve_reference_type(
    const type* _type) {
  ast_type* sub_type;
  if (!_type->custom_type_name().empty()) {
    auto it = m_structure_types.find(
        _type->custom_type_name().to_string(m_allocator));
    if (it == m_structure_types.end()) {
      auto extern_it = m_type_manager->m_external_types.find(
          _type->custom_type_name().to_string(m_allocator));
      if (extern_it == m_type_manager->m_external_types.end()) {
        _type->log_line(m_log, logging::log_level::error);
        m_log->log_error("Undeclared reference type");
        return nullptr;
      }
      sub_type = extern_it->second.get();
    } else {
      sub_type = it->second.get();
    }
  } else {
    sub_type = resolve_type(_type->get_subtype());
    if (!sub_type) {
      _type->log_line(m_log, logging::log_level::error);
      m_log->log_error("Could not resolve pointer subtype");
      return nullptr;
    }
  }

  switch (_type->get_reference_type()) {
    case reference_type::unique:
      return m_type_manager->get_reference_of(
          sub_type, ast_type_classification::reference);
      break;
    case reference_type::shared:
      return m_type_manager->get_reference_of(
          sub_type, ast_type_classification::shared_reference);
      break;
    case reference_type::weak:
      return m_type_manager->get_reference_of(
          sub_type, ast_type_classification::weak_reference);
      break;
    default:
      _type->log_line(m_log, logging::log_level::error);
      m_log->log_error("Unknown reference type");
      return nullptr;
  }
}

ast_type* parse_ast_convertor::convertor_context::resolve_static_array(
    const array_type* _type) {
  // Static array types sized, only the initializer is actually sized.
  ast_type* subtype = resolve_type(_type->get_subtype());
  if (!subtype) {
    _type->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not resolve array subtype");
    return nullptr;
  }
  auto it = m_static_array_types.find(core::make_pair(uint32_t(0), subtype));
  if (it != m_static_array_types.end()) {
    return it->second.get();
  }

  auto array_type = memory::make_unique<ast_type>(m_allocator);
  array_type->m_classification = ast_type_classification::static_array;
  array_type->m_name =
      containers::string(m_allocator, "_array_of_0_") + subtype->m_name;
  array_type->m_implicitly_contained_type = subtype;
  ast_type* return_type = array_type.get();
  m_static_array_types[core::make_pair(uint32_t(0), subtype)] =
      core::move(array_type);
  m_ordered_type_definitions.push_back(return_type);
  return return_type;
}

ast_type* parse_ast_convertor::convertor_context::resolve_dynamic_array(
    const dynamic_array_type* _type) {
  ast_type* subtype = resolve_type(_type->get_subtype());
  if (!subtype) {
    _type->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not resolve dynamic array subtype");
    return nullptr;
  }
  auto it = m_dynamic_array_types.find(subtype);
  if (it != m_dynamic_array_types.end()) {
    return it->second.get();
  }

  auto dynamic_array_type = memory::make_unique<ast_type>(m_allocator);
  dynamic_array_type->m_name =
      containers::string(m_allocator, "_dyn_array_of_") + subtype->m_name;
  dynamic_array_type->m_classification = ast_type_classification::dynamic_array;
  dynamic_array_type->m_implicitly_contained_type = subtype;
  ast_type* return_type = dynamic_array_type.get();
  m_dynamic_array_types[subtype] = core::move(dynamic_array_type);
  m_ordered_type_definitions.push_back(return_type);
  return return_type;
}

ast_type* parse_ast_convertor::convertor_context::resolve_function_ptr_type(
    const ast_function* _function) {
  containers::dynamic_array<const ast_type*> types(
      m_allocator, _function->m_parameters.size() + 1);
  types[0] = _function->m_return_type;
  for (size_t i = 0; i < _function->m_parameters.size(); ++i) {
    types[i + 1] = _function->m_parameters[i].m_type;
  }

  return resolve_function_ptr_type(core::move(types));
}

ast_type* parse_ast_convertor::convertor_context::resolve_function_ptr_type(
    containers::dynamic_array<const ast_type*> _types) {
  auto type = m_function_pointer_types.find(_types);
  if (type != m_function_pointer_types.end()) {
    return type->second.get();
  }

  auto fnptr_type = memory::make_unique<ast_type>(m_allocator);
  fnptr_type->m_builtin = builtin_type::not_builtin;
  // Figure out names
  fnptr_type->m_name = containers::string(m_allocator, "_wns_fn_destructor_");

  fnptr_type->m_classification = ast_type_classification::function_pointer;
  for (auto t : _types) {
    fnptr_type->initialized_contained_types(m_allocator).push_back(t);
  }

  char buff[11] = {
      0,
  };
  memory::writeuint32(
      buff, static_cast<uint32_t>(m_function_pointer_types.size()), 11);
  containers::string str = containers::string(m_allocator, "_wns_fn_ptr_");
  str += buff;
  fnptr_type->m_name = core::move(str);
  fnptr_type->calculate_mangled_name(m_allocator);
  m_ordered_type_definitions.push_back(fnptr_type.get());

  auto t = fnptr_type.get();
  m_function_pointer_types[core::move(_types)] = core::move(fnptr_type);

  return t;
}

}  // namespace scripting
}  // namespace wn