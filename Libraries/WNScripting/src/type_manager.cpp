// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/type_manager.h"
#include "WNScripting/inc/ast_node_types.h"

namespace wn {
namespace scripting {
type_manager::type_manager(memory::allocator* _allocator)
  : m_allocator(_allocator),
    m_external_types(_allocator),
    m_externally_visible_types(_allocator),
    m_externals(_allocator),
    m_integral_types(_allocator),
    m_float_types(_allocator),
    m_reference_types(_allocator),
    m_shared_types(_allocator),
    m_weak_types(_allocator),
    m_static_array_types(_allocator) {
  auto uint32_type = memory::make_unique<ast_type>(m_allocator);
  uint32_type->m_name = containers::string(m_allocator, "Int");
  uint32_type->m_builtin = builtin_type::integral_type;
  uint32_type->m_bit_width = 32;
  uint32_type->m_is_arithmetic_type = true;
  uint32_type->m_is_comparable_type = true;
  uint32_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<int32_t>::get_unique_identifier()] =
      uint32_type.get();
  m_integral_types[32] = core::move(uint32_type);

  auto uint8_type = memory::make_unique<ast_type>(m_allocator);
  uint8_type->m_name = containers::string(m_allocator, "Char");
  uint8_type->m_builtin = builtin_type::integral_type;
  uint8_type->m_bit_width = 8;
  uint8_type->m_is_arithmetic_type = true;
  uint8_type->m_is_comparable_type = true;
  uint8_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<uint8_t>::get_unique_identifier()] =
      uint8_type.get();

  m_integral_types[8] = core::move(uint8_type);

  auto float_type = memory::make_unique<ast_type>(m_allocator);
  float_type->m_name = containers::string(m_allocator, "Float");
  float_type->m_builtin = builtin_type::floating_point_type;
  float_type->m_bit_width = 32;
  float_type->m_is_arithmetic_type = true;
  float_type->m_is_comparable_type = true;
  float_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<float_t>::get_unique_identifier()] =
      float_type.get();
  m_float_types[32] = core::move(float_type);

  m_void_t = memory::make_unique<ast_type>(m_allocator);
  m_void_t->m_name = containers::string(m_allocator, "Void");
  m_void_t->m_builtin = builtin_type::void_type;
  m_void_t->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<void>::get_unique_identifier()] =
      m_void_t.get();

  m_void_ptr_t = memory::make_unique<ast_type>(m_allocator);
  m_void_ptr_t->m_name = containers::string(m_allocator, "VoidPtr");
  m_void_ptr_t->m_builtin = builtin_type::void_ptr_type;
  m_void_ptr_t->m_implicitly_contained_type = m_void_t.get();
  m_void_ptr_t->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<void*>::get_unique_identifier()] =
      m_void_ptr_t.get();

  m_bool_t = memory::make_unique<ast_type>(m_allocator);
  m_bool_t->m_name = containers::string(m_allocator, "Bool");
  m_bool_t->m_builtin = builtin_type::bool_type;
  m_bool_t->calculate_mangled_name(m_allocator);
  m_bool_t->m_is_comparable_type = true;
  m_externally_visible_types[c_type_tag<bool>::get_unique_identifier()] =
      m_bool_t.get();

  m_size_t = memory::make_unique<ast_type>(m_allocator);
  m_size_t->m_name = containers::string(m_allocator, "Size");
  m_size_t->m_builtin = builtin_type::size_type;
  m_size_t->calculate_mangled_name(m_allocator);
  m_size_t->m_is_arithmetic_type = true;
  m_size_t->m_is_comparable_type = true;
  m_externally_visible_types[c_type_tag<wn_size_t>::get_unique_identifier()] =
      m_size_t.get();

  m_nullptr_t = memory::make_unique<ast_type>(m_allocator);
  m_nullptr_t->m_name = containers::string(m_allocator, "Nullptr");
  m_nullptr_t->m_builtin = builtin_type::nullptr_type;
  m_nullptr_t->calculate_mangled_name(m_allocator);
  m_nullptr_t->m_is_comparable_type = true;

  m_function_t = memory::make_unique<ast_type>(m_allocator);
  m_function_t->m_name = containers::string(m_allocator, "Function");
  m_function_t->m_builtin = builtin_type::unresolved_function_type;
  m_function_t->calculate_mangled_name(m_allocator);

  m_vtable_t = memory::make_unique<ast_type>(m_allocator);
  m_vtable_t->m_name = containers::string(m_allocator, "__Vtable");
  m_vtable_t->m_builtin = builtin_type::vtable_type;
  m_vtable_t->calculate_mangled_name(m_allocator);
}

type_manager::~type_manager() {}

ast_type* type_manager::register_external_type(
    containers::string_view _name, uint32_t _size, ast_type* _parent_type) {
  auto type = memory::make_unique<ast_type>(m_allocator);
  type->m_classification = ast_type_classification::extern_type;
  type->m_name = containers::string(m_allocator, _name);
  type->m_external_type_size = _size;
  type->calculate_mangled_name(m_allocator);
  type->m_parent_type = _parent_type;

  auto* ret_type = type.get();
  m_external_types[_name.to_string(m_allocator)] = core::move(type);
  return ret_type;
}

containers::string_view type_manager::internal_get_mangled_name(
    void* idx) const {
  return m_externally_visible_types.find(idx)->second->m_mangled_name;
}

void type_manager::finalize_external_type(ast_type* _type) const {
  _type->setup_external_type(m_allocator);
}

void exporter_base::add_contained_type(
    containers::string_view _name, ast_type* _type, uint32_t _offset) {
  containers::string str(m_allocator, _name);
  containers::string_view v(str);
  m_type->initialized_contained_externals(m_allocator)[v] = {
      core::move(str), _offset, _type};
}

containers::string_view exporter_base::add_contained_function(
    containers::string_view _name, containers::dynamic_array<ast_type*> _types,
    bool _is_pseudo_function) {
  memory::unique_ptr<ast_function> func =
      memory::make_unique<ast_function>(m_allocator, nullptr);
  auto& params = func->initialized_parameters(m_allocator);
  for (uint32_t i = 1; i < _types.size(); ++i) {
    char count[12] = {
        '_',
        0,
    };
    memory::writeuint32(count + 1, i, 10);
    params.push_back(ast_function::parameter{
        containers::string(m_allocator, count), _types[i]});
  }
  func->m_is_member_function = true;
  func->m_return_type = _types[0];
  func->m_name = _name.to_string(m_allocator);
  func->calculate_mangled_name(m_allocator);
  func->m_is_external = true;
  func->m_is_external_pseudo = _is_pseudo_function;
  containers::string_view mangled_name = func->m_mangled_name;
  m_type->initialized_external_member_functions(m_allocator)
      .push_back(core::move(func));
  return mangled_name;
}

ast_type* type_manager::get_reference_of(
    const ast_type* _type, ast_type_classification _reference_type) {
  containers::hash_map<const ast_type*, memory::unique_ptr<ast_type>>*
      insertion_map;
  const char* prefix = "";

  switch (_reference_type) {
    case ast_type_classification::reference:
      insertion_map = &m_reference_types;
      prefix = "_unique_";
      break;
    case ast_type_classification::shared_reference:
      insertion_map = &m_shared_types;
      prefix = "_shared_";
      break;
    case ast_type_classification::weak_reference:
      insertion_map = &m_weak_types;
      prefix = "_weak_";
      break;
    default:
      WN_RELEASE_ASSERT(false, "Unknown reference type");
      return nullptr;
  }

  auto it = insertion_map->find(_type);
  if (it != insertion_map->end()) {
    return it->second.get();
  }

  auto ref_type = memory::make_unique<ast_type>(m_allocator);
  ref_type->m_classification = _reference_type;
  ref_type->m_name = containers::string(m_allocator, prefix) + _type->m_name;
  ref_type->m_implicitly_contained_type = _type;
  ast_type* return_type = ref_type.get();
  return_type->calculate_mangled_name(m_allocator);
  (*insertion_map)[_type] = core::move(ref_type);
  return return_type;
}

ast_type* type_manager::get_array_of(const ast_type* _type, uint32_t _size) {
  auto it = m_static_array_types.find(core::make_pair(_size, _type));
  if (it != m_static_array_types.end()) {
    return it->second.get();
  }

  auto array_type = memory::make_unique<ast_type>(m_allocator);
  array_type->m_classification = ast_type_classification::static_array;

  array_type->m_name = containers::string(m_allocator, "_array_");
  if (_size > 0) {
    array_type->m_name += "_of_";

    char buff[11] = {
        0,
    };
    memory::writeuint32(
        buff, static_cast<uint32_t>(_size), 11);
    array_type->m_name += buff;
    array_type->m_name += "_";
  }

  array_type->m_implicitly_contained_type = _type;
  array_type->m_static_array_size = _size;
  ast_type* return_type = array_type.get();
  return_type->calculate_mangled_name(m_allocator);
  m_static_array_types[core::make_pair(_size, _type)] = core::move(array_type);
  return return_type;
}

}  // namespace scripting
}  // namespace wn