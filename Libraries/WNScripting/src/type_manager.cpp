// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/type_manager.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/ast_node_types.h"

namespace wn {
namespace scripting {
type_manager::type_manager(memory::allocator* _allocator, logging::log* _log)
  : m_allocator(_allocator),
    m_log(_log),
    m_external_types(_allocator),
    m_externally_visible_types(_allocator),
    m_externals(_allocator),
    m_integral_types(_allocator),
    m_float_types(_allocator),
    m_reference_types(_allocator),
    m_shared_types(_allocator),
    m_weak_types(_allocator),
    m_static_array_types(_allocator),
    m_runtime_array_types(_allocator),
    m_structure_types(_allocator),
    m_function_pointer_types(_allocator),
    m_all_types(_allocator),
    m_struct_definitions(_allocator) {
  auto uint32_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  uint32_type->m_name = containers::string(m_allocator, "Int");
  uint32_type->m_builtin = builtin_type::integral_type;
  uint32_type->m_bit_width = 32;
  uint32_type->m_is_arithmetic_type = true;
  uint32_type->m_is_comparable_type = true;
  uint32_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<int32_t>::get_unique_identifier()] =
      uint32_type.get();
  m_externally_visible_types
      [c_type_tag<wn_array_ptr<int32_t>>::get_unique_identifier()] =
          get_array_of(uint32_type.get(), 0, nullptr);

  m_integral_types[32] = core::move(uint32_type);

  auto uint8_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  uint8_type->m_name = containers::string(m_allocator, "Char");
  uint8_type->m_builtin = builtin_type::integral_type;
  uint8_type->m_bit_width = 8;
  uint8_type->m_is_arithmetic_type = true;
  uint8_type->m_is_comparable_type = true;
  uint8_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<uint8_t>::get_unique_identifier()] =
      uint8_type.get();
  m_externally_visible_types
      [c_type_tag<wn_array_ptr<uint8_t>>::get_unique_identifier()] =
          get_array_of(uint8_type.get(), 0, nullptr);

  m_integral_types[8] = core::move(uint8_type);

  auto float_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  float_type->m_name = containers::string(m_allocator, "Float");
  float_type->m_builtin = builtin_type::floating_point_type;
  float_type->m_bit_width = 32;
  float_type->m_is_arithmetic_type = true;
  float_type->m_is_comparable_type = true;
  float_type->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<float_t>::get_unique_identifier()] =
      float_type.get();
  m_externally_visible_types
      [c_type_tag<wn_array_ptr<float>>::get_unique_identifier()] =
          get_array_of(float_type.get(), 0, nullptr);
  m_float_types[32] = core::move(float_type);

  m_void_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_void_t->m_name = containers::string(m_allocator, "Void");
  m_void_t->m_builtin = builtin_type::void_type;
  m_void_t->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<void>::get_unique_identifier()] =
      m_void_t.get();

  m_void_ptr_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_void_ptr_t->m_name = containers::string(m_allocator, "VoidPtr");
  m_void_ptr_t->m_builtin = builtin_type::void_ptr_type;
  m_void_ptr_t->m_implicitly_contained_type = m_void_t.get();
  m_void_ptr_t->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<void*>::get_unique_identifier()] =
      m_void_ptr_t.get();

  m_bool_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_bool_t->m_name = containers::string(m_allocator, "Bool");
  m_bool_t->m_builtin = builtin_type::bool_type;
  m_bool_t->calculate_mangled_name(m_allocator);
  m_bool_t->m_is_comparable_type = true;
  m_externally_visible_types[c_type_tag<bool>::get_unique_identifier()] =
      m_bool_t.get();
  m_externally_visible_types
      [c_type_tag<wn_array_ptr<bool>>::get_unique_identifier()] =
          get_array_of(m_bool_t.get(), 0, nullptr);

  m_size_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_size_t->m_name = containers::string(m_allocator, "Size");
  m_size_t->m_builtin = builtin_type::size_type;
  m_size_t->calculate_mangled_name(m_allocator);
  m_size_t->m_is_arithmetic_type = true;
  m_size_t->m_is_comparable_type = true;
  m_externally_visible_types[c_type_tag<wn_size_t>::get_unique_identifier()] =
      m_size_t.get();

  m_nullptr_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_nullptr_t->m_name = containers::string(m_allocator, "Nullptr");
  m_nullptr_t->m_builtin = builtin_type::nullptr_type;
  m_nullptr_t->calculate_mangled_name(m_allocator);
  m_nullptr_t->m_is_comparable_type = true;

  m_function_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_function_t->m_name = containers::string(m_allocator, "Function");
  m_function_t->m_builtin = builtin_type::unresolved_function_type;
  m_function_t->calculate_mangled_name(m_allocator);

  m_vtable_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_vtable_t->m_name = containers::string(m_allocator, "__Vtable");
  m_vtable_t->m_builtin = builtin_type::vtable_type;
  m_vtable_t->calculate_mangled_name(m_allocator);

  m_cstr_t = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  m_cstr_t->m_name = containers::string(m_allocator, "CString");
  m_cstr_t->m_builtin = builtin_type::c_string_type;
  m_cstr_t->calculate_mangled_name(m_allocator);
  m_externally_visible_types[c_type_tag<const char*>::get_unique_identifier()] =
      m_cstr_t.get();

  m_destructor_fn_ptr_t = resolve_function_ptr_type(
      containers::dynamic_array<const ast_type*>(
          m_allocator, {m_void_t.get(), m_void_ptr_t.get()}),
      nullptr);

  m_shared_object_header = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _mem) { return new (_mem) ast_type(&m_all_types); });

  auto decl = memory::make_unique<ast_declaration>(_allocator, nullptr);
  decl->m_type = m_size_t.get();
  decl->m_name = containers::string(_allocator, "ref_count");

  m_shared_object_header->m_name =
      containers::string(m_allocator, "_wns_shared_object");
  m_shared_object_header->m_builtin = builtin_type::not_builtin;
  m_shared_object_header->m_classification =
      ast_type_classification::struct_type;
  m_shared_object_header->initialized_structure_members(m_allocator)
      .push_back(core::move(decl));

  auto destruct = memory::make_unique<ast_declaration>(_allocator, nullptr);
  destruct->m_type = m_destructor_fn_ptr_t;
  destruct->m_name = containers::string(_allocator, "destructor");
  m_shared_object_header->initialized_structure_members(m_allocator)
      .push_back(core::move(destruct));
}

type_manager::~type_manager() {}

ast_type* type_manager::register_external_type(
    containers::string_view _name, uint32_t _size, ast_type* _parent_type) {
  auto type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  type->m_classification = ast_type_classification::extern_type;
  type->m_name = containers::string(m_allocator, _name);
  type->m_external_type_size = _size;
  type->calculate_mangled_name(m_allocator);
  type->m_parent_type = _parent_type;

  auto* ret_type = type.get();
  m_external_types[ret_type->m_name] = core::move(type);
  return ret_type;
}

ast_type* type_manager::register_external_type(containers::string_view _name) {
  auto type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  type->m_classification = ast_type_classification::extern_type;
  type->m_name = containers::string(m_allocator, _name);
  type->calculate_mangled_name(m_allocator);

  auto* ret_type = type.get();
  m_external_types[ret_type->m_name] = core::move(type);
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
    containers::string_view _name, const ast_type* _type, uint32_t _offset) {
  containers::string str(m_allocator, _name);
  containers::string_view v(str);
  m_type->initialized_contained_externals(m_allocator)[v] = {
      core::move(str), _offset, _type};
}

containers::string_view exporter_base::add_contained_function(
    containers::string_view _name,
    containers::dynamic_array<const ast_type*> _types,
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

ast_type* type_manager::get_reference_of(const ast_type* _type,
    ast_type_classification _reference_type, used_type_set* _used) {
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
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  auto ref_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  ref_type->m_classification = _reference_type;
  ref_type->m_name = containers::string(m_allocator, prefix) + _type->m_name;
  ref_type->m_implicitly_contained_type = _type;
  ast_type* return_type = ref_type.get();
  return_type->calculate_mangled_name(m_allocator);
  (*insertion_map)[_type] = core::move(ref_type);
  if (_used) {
    _used->insert(return_type);
  }
  return return_type;
}

ast_type* type_manager::get_array_of(
    const ast_type* _type, uint32_t _size, used_type_set* _used) {
  auto it = m_static_array_types.find(core::make_pair(_size, _type));
  if (it != m_static_array_types.end()) {
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  auto array_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  array_type->m_classification = ast_type_classification::static_array;

  array_type->m_name = containers::string(m_allocator, "_array_");
  if (_size > 0) {
    array_type->m_name += "_of_";

    char buff[11] = {
        0,
    };
    memory::writeuint32(buff, static_cast<uint32_t>(_size), 11);
    array_type->m_name += buff;
    array_type->m_name += "_";
  }

  array_type->m_implicitly_contained_type = _type;
  array_type->m_static_array_size = _size;
  ast_type* return_type = array_type.get();
  return_type->calculate_mangled_name(m_allocator);
  m_static_array_types[core::make_pair(_size, _type)] = core::move(array_type);
  if (_used) {
    _used->insert(return_type);
  }
  return return_type;
}

ast_type* type_manager::get_runtime_array_of(
    const ast_type* _type, used_type_set* _used) {
  auto it = m_runtime_array_types.find(_type);
  if (it != m_runtime_array_types.end()) {
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  auto array_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  array_type->m_classification = ast_type_classification::runtime_array;

  array_type->m_name = containers::string(m_allocator, "_runtime_array_");
  array_type->m_implicitly_contained_type = _type;
  ast_type* return_type = array_type.get();
  return_type->calculate_mangled_name(m_allocator);
  m_runtime_array_types[_type] = core::move(array_type);
  if (_used) {
    _used->insert(return_type);
  }
  return return_type;
}

ast_type* type_manager::get_or_register_struct(
    containers::string_view _type, used_type_set* _used) {
  auto it = m_structure_types.find(_type);
  if (it != m_structure_types.end()) {
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  auto struct_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _memory) { return new (_memory) ast_type(&m_all_types); });
  struct_type->m_name = _type.to_string(m_allocator);
  ast_type* return_type = struct_type.get();
  return_type->m_classification = ast_type_classification::struct_type;
  return_type->calculate_mangled_name(m_allocator);
  m_structure_types[return_type->m_name] = core::move(struct_type);
  if (_used) {
    _used->insert(return_type);
  }
  return return_type;
}

ast_type* type_manager::register_child_struct(containers::string_view _name,
    ast_type* _struct_type, used_type_set* _used) {
  WN_DEBUG_ASSERT(m_structure_types.find(_name) == m_structure_types.end(),
      "You cannot register a child type with this name, it already exists");

  auto struct_type = _struct_type->clone(m_allocator, &m_all_types);
  struct_type->m_name = _name.to_string(m_allocator);
  ast_type* return_type = struct_type.get();
  return_type->calculate_mangled_name(m_allocator);
  m_structure_types[return_type->m_name] = core::move(struct_type);
  if (_used) {
    _used->insert(return_type);
  }
  return return_type;
}

const ast_type* type_manager::resolve_function_ptr_type(
    containers::dynamic_array<const ast_type*> _types, used_type_set* _used) {
  auto type = m_function_pointer_types.find(_types);
  if (type != m_function_pointer_types.end()) {
    if (_used) {
      _used->insert(type->second.get());
    }
    return type->second.get();
  }

  auto fnptr_type = memory::make_unique_delegated<ast_type>(m_allocator,
      [this](void* _mem) { return new (_mem) ast_type(&m_all_types); });
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

  auto t = fnptr_type.get();
  m_function_pointer_types[core::move(_types)] = core::move(fnptr_type);
  if (_used) {
    _used->insert(t);
  }
  return t;
}

containers::dynamic_array<const ast_type*>
type_manager::get_initialialization_order(memory::allocator* _allocator,
    const containers::hash_set<const ast_type*>& _types) {
  containers::dynamic_array<const ast_type*> init(_allocator);
  init.reserve(_types.size());
  for (auto& it : m_all_types) {
    if (_types.find(it) != _types.end()) {
      init.push_back(it);
    }
  }
  return core::move(init);
}

bool type_manager::register_struct_definition(const struct_definition* _def) {
  if (m_struct_definitions.find(_def->get_name()) !=
      m_struct_definitions.end()) {
    m_log->log_error("Structure ", _def->get_name(), " already defined.");
    return false;
  }

  auto st = clone_node(m_allocator, _def);
  auto nm = st->get_name();
  m_struct_definitions.insert(
      core::move(nm), struct_definition_data{core::move(st), nullptr, false});
  return true;
}

ast_type* type_manager::struct_definition_resolved(
    containers::string_view _name) const {
  WN_DEBUG_ASSERT(
      m_struct_definitions.find(_name) != m_struct_definitions.end(),
      "struct_definition was never initialized");
  return m_struct_definitions[_name].m_resolved;
}

void type_manager::set_struct_definition_resolved(
    containers::string_view _name, ast_type* _type) {
  WN_DEBUG_ASSERT(
      m_struct_definitions.find(_name) != m_struct_definitions.end(),
      "struct_definition was never initialized");
  m_struct_definitions[_name].m_resolved = _type;
}

void type_manager::set_member_functions_resolved(
    containers::string_view _name) {
  WN_DEBUG_ASSERT(
      m_struct_definitions.find(_name) != m_struct_definitions.end(),
      "struct_definition was never initialized");
  m_struct_definitions[_name].member_functions_resolved = true;
}

bool type_manager::member_functions_resolved(
    containers::string_view _name) const {
  WN_DEBUG_ASSERT(
      m_struct_definitions.find(_name) != m_struct_definitions.end(),
      "struct_definition was never initialized");
  return m_struct_definitions[_name].member_functions_resolved;
}

const struct_definition* type_manager::get_struct_definition(
    containers::string_view _name) const {
  auto it = m_struct_definitions.find(_name);
  if (it == m_struct_definitions.end()) {
    return nullptr;
  }
  return it->second.st_def.get();
}

}  // namespace scripting
}  // namespace wn