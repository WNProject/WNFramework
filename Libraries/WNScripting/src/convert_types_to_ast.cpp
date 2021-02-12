// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

const ast_type* parse_ast_convertor::convertor_context::resolve_type(
    const type* _type) {
  if (_type->get_node_type() == node_type::array_type) {
    return resolve_static_array(cast_to<array_type>(_type));
  } else if (_type->get_node_type() == node_type::runtime_array_type) {
    return resolve_runtime_array(cast_to<runtime_array_type>(_type));
  } else if (_type->get_node_type() == node_type::slice_type) {
    return resolve_slice(cast_to<slice_type>(_type));
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
    auto it = m_type_manager->get_struct_definition(_type->custom_type_name());
    if (!it) {
      auto extern_t = m_type_manager->get_external_type(
          _type->custom_type_name(), &m_used_types);
      if (!extern_t) {
        _type->log_line(m_log, logging::log_level::error);
        m_log->log_error("Could not find type definition");
        return nullptr;
      }
      return extern_t;
    }
    return walk_struct_definition(it);
  }
  WN_DEBUG_ASSERT(_type->get_reference_type() == reference_type::raw,
      "No known struct type can get us here");

  return resolve_builtin_type(_type->get_classification());
}

const ast_type* parse_ast_convertor::convertor_context::resolve_builtin_type(
    const uint32_t _type) {
  switch (_type) {
    case static_cast<uint32_t>(type_classification::void_type):
      return m_type_manager->void_t(&m_used_types);
    case static_cast<uint32_t>(type_classification::int_type):
      return m_type_manager->integral(32, &m_used_types);
    case static_cast<uint32_t>(type_classification::float_type):
      return m_type_manager->floating(32, &m_used_types);
    case static_cast<uint32_t>(type_classification::char_type):
      return m_type_manager->integral(8, &m_used_types);
    case static_cast<uint32_t>(type_classification::bool_type):
      return m_type_manager->bool_t(&m_used_types);
    case static_cast<uint32_t>(type_classification::size_type):
      return m_type_manager->size_t_t(&m_used_types);
    case static_cast<uint32_t>(type_classification::void_ptr_type):
      return m_type_manager->void_ptr_t(&m_used_types);
    case static_cast<uint32_t>(type_classification::function_ptr_type):
      return m_type_manager->function_t(&m_used_types);
    case static_cast<uint32_t>(type_classification::string_type):
      return m_type_manager->cstr_t(&m_used_types);
    default:
      WN_DEBUG_ASSERT(false, "Unknown type index");
  }
  return nullptr;
}

const ast_type* parse_ast_convertor::convertor_context::resolve_reference_type(
    const type* _type) {
  const ast_type* sub_type;
  if (!_type->custom_type_name().empty()) {
    auto it = m_type_manager->get_structure_type(
        _type->custom_type_name(), &m_used_types);
    if (!it) {
      auto extern_it = m_type_manager->get_external_type(
          _type->custom_type_name(), &m_used_types);
      if (!extern_it) {
        _type->log_line(m_log, logging::log_level::error);
        m_log->log_error("Undeclared reference type");
        return nullptr;
      }
      sub_type = extern_it;
    } else {
      sub_type = it;
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
          sub_type, ast_type_classification::reference, &m_used_types);
      break;
    case reference_type::shared:
      return m_type_manager->get_reference_of(
          sub_type, ast_type_classification::shared_reference, &m_used_types);
      break;
    case reference_type::weak:
      return m_type_manager->get_reference_of(
          sub_type, ast_type_classification::weak_reference, &m_used_types);
      break;
    default:
      _type->log_line(m_log, logging::log_level::error);
      m_log->log_error("Unknown reference type");
      return nullptr;
  }
}

const ast_type* parse_ast_convertor::convertor_context::resolve_static_array(
    const array_type* _type) {
  // Static array types sized, only the initializer is actually sized.
  const ast_type* subtype = resolve_type(_type->get_subtype());
  if (!subtype) {
    _type->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not resolve array subtype");
    return nullptr;
  }
  if (_type->get_size() == nullptr) {
    return get_array_of(subtype, 0);
  } else {
    if (const expression* _expr = _type->get_size()) {
      if (_expr->get_node_type() == node_type::constant_expression) {
        const constant_expression* c =
            cast_to<const constant_expression>(_expr);
        if (c->get_index() !=
            static_cast<uint32_t>(type_classification::int_type)) {
          _type->log_line(m_log, logging::log_level::error);
          m_log->log_error("Array must be initialized with a constant integer");
          return nullptr;
        }
        if (c->get_type_text() == "0" || c->get_type_text()[0] == '-') {
          _type->log_line(m_log, logging::log_level::error);
          m_log->log_error("Array size must be positive integer");
        }

        uint32_t num = 0;
        wn::memory::readuint32(
            c->get_type_text().c_str(), num, c->get_type_text().size());

        return get_array_of(subtype, num);
      } else {
        _type->log_line(m_log, logging::log_level::error);
        m_log->log_error("Array must be initialized with a constant size");
        return nullptr;
      }
    }
  }
  return nullptr;
}

const ast_type* parse_ast_convertor::convertor_context::resolve_runtime_array(
    const runtime_array_type* _type) {
  const ast_type* subtype = resolve_type(_type->get_subtype());
  if (!subtype) {
    _type->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not resolve dynamic array subtype");
    return nullptr;
  }
  return get_runtime_array_of(subtype);
}

const ast_type* parse_ast_convertor::convertor_context::resolve_slice(
    const slice_type* _type) {
  const ast_type* subtype = resolve_type(_type->get_subtype());
  if (!subtype) {
    _type->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not resolve slice subtype");
  }
  return get_slice_of(subtype, _type->dimensions());
}

const ast_type*
parse_ast_convertor::convertor_context::resolve_function_ptr_type(
    const ast_function* _function) {
  containers::dynamic_array<const ast_type*> types(
      m_allocator, _function->m_parameters.size() + 1);
  types[0] = _function->m_return_type;
  for (size_t i = 0; i < _function->m_parameters.size(); ++i) {
    types[i + 1] = _function->m_parameters[i].m_type;
  }

  return m_type_manager->resolve_function_ptr_type(
      core::move(types), &m_used_types);
}

}  // namespace scripting
}  // namespace wn
