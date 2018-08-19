// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_TYPE_MANAGER_H__
#define __WN_SCRIPTING_TYPE_MANAGER_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNContainers/inc/WNString.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/externals.h"

namespace wn {
namespace scripting {
using void_f = void (*)();

struct ast_type;

enum class ast_type_classification {
  primitive,
  reference,
  shared_reference,
  weak_reference,
  struct_type,
  static_array,
  runtime_array,
  function_pointer,
  extern_type,
};

using used_type_set = containers::hash_set<const ast_type*>;

class type_manager {
public:
  type_manager(memory::allocator* _allocator);
  ~type_manager();

  template <typename T>
  void register_child_cpp_type(functional::function<void(
          containers::string_view _mangled_name, void* _ptr, bool _is_virtual)>
          _external_function_added);

  template <typename T>
  void register_cpp_type(functional::function<void(
          containers::string_view _mangled_name, void* _ptr, bool _is_virtual)>
          _external_function_added);

  template <typename T>
  containers::string_view get_mangled_name() const {
    return internal_get_mangled_name(c_type_tag<T>::get_unique_identifier());
  }

  template <typename T>
  const ast_type* get_type() const {
    return m_externally_visible_types
        .find(c_type_tag<T>::get_unique_identifier())
        ->second;
  }

  template <typename... Args>
  containers::dynamic_array<containers::string_view> get_mangled_names() const {
    return containers::dynamic_array<containers::string_view>(
        m_allocator, {get_mangled_name<Args>()...});
  }

  template <typename... Args>
  containers::dynamic_array<const ast_type*> get_types() const {
    return containers::dynamic_array<const ast_type*>(
        m_allocator, {get_type<Args>()...});
  }

  ast_type* get_reference_of(const ast_type* _type,
      ast_type_classification _reference_type, used_type_set* _used);

  ast_type* get_array_of(
      const ast_type* _type, uint32_t _size, used_type_set* _used);
  ast_type* get_runtime_array_of(const ast_type* _type, used_type_set* _used);

  ast_type* get_or_register_struct(
      containers::string_view _name, used_type_set* _used);
  ast_type* register_child_struct(containers::string_view _name,
      ast_type* _struct_type, used_type_set* _used);
  containers::dynamic_array<const ast_type*> get_initialialization_order(
      memory::allocator* _allocator,
      const containers::hash_set<const ast_type*>& _types);

  const ast_type* resolve_function_ptr_type(
      containers::dynamic_array<const ast_type*> _types, used_type_set* _used);

  external_function* add_external(external_function function) {
    m_externals.push_back(function);
    return &m_externals.back();
  }

  const ast_type* void_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_void_t.get());
    }
    return m_void_t.get();
  }

  const ast_type* void_ptr_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_void_ptr_t.get());
    }
    return m_void_ptr_t.get();
  }
  const ast_type* bool_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_bool_t.get());
    }
    return m_bool_t.get();
  }
  const ast_type* size_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_size_t.get());
    }
    return m_size_t.get();
  }
  const ast_type* function_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_function_t.get());
    }
    return m_function_t.get();
  }
  const ast_type* nullptr_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_nullptr_t.get());
    }
    return m_nullptr_t.get();
  }
  const ast_type* vtable_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_vtable_t.get());
    }
    return m_vtable_t.get();
  }
  const ast_type* cstr_t(used_type_set* _used) {
    if (_used) {
      _used->insert(m_cstr_t.get());
    }
    return m_cstr_t.get();
  }
  const ast_type* destructor_fn_ptr(used_type_set* _used) {
    if (_used) {
      _used->insert(m_destructor_fn_ptr_t);
    }
    return m_destructor_fn_ptr_t;
  }
  const ast_type* shared_object_header(used_type_set* _used) {
    if (_used) {
      _used->insert(m_shared_object_header.get());
    }
    return m_shared_object_header.get();
  }

  const ast_type* integral(uint32_t size, used_type_set* _used) {
    auto it = m_integral_types.find(size);
    if (it == m_integral_types.end()) {
      return nullptr;
    }
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  const ast_type* floating(uint32_t size, used_type_set* _used) {
    auto it = m_float_types.find(size);
    if (it == m_float_types.end()) {
      return nullptr;
    }
    if (_used) {
      _used->insert(it->second.get());
    }
    return it->second.get();
  }

  const containers::deque<external_function>& external_functions() const {
    return m_externals;
  }
  ast_type* register_external_type(containers::string_view _type);
  const ast_type* get_external_type(
      containers::string_view _name, used_type_set* _used) const {
    auto it = m_external_types.find(_name);
    if (it != m_external_types.end()) {
      _used->insert(it->second.get());
      return it->second.get();
    }
    return nullptr;
  }

  const ast_type* get_structure_type(
      containers::string_view _name, used_type_set* _used) const {
    auto it = m_structure_types.find(_name);
    if (it != m_structure_types.end()) {
      _used->insert(it->second.get());
      return it->second.get();
    }
    return nullptr;
  }

private:
  ast_type* register_external_type(containers::string_view _type,
      uint32_t _size, ast_type* _parent_type = nullptr);

  void finalize_external_type(ast_type* _type) const;
  containers::string_view internal_get_mangled_name(void* idx) const;
  memory::allocator* m_allocator;

  containers::hash_map<containers::string_view, memory::unique_ptr<ast_type>>
      m_external_types;
  containers::hash_map<void*, ast_type*> m_externally_visible_types;
  containers::deque<external_function> m_externals;
  memory::unique_ptr<ast_type> m_void_t;
  memory::unique_ptr<ast_type> m_void_ptr_t;
  memory::unique_ptr<ast_type> m_bool_t;
  memory::unique_ptr<ast_type> m_size_t;
  memory::unique_ptr<ast_type> m_function_t;
  memory::unique_ptr<ast_type> m_nullptr_t;
  memory::unique_ptr<ast_type> m_vtable_t;
  memory::unique_ptr<ast_type> m_cstr_t;
  containers::hash_map<uint32_t, memory::unique_ptr<ast_type>> m_integral_types;
  containers::hash_map<uint32_t, memory::unique_ptr<ast_type>> m_float_types;

  const ast_type* m_destructor_fn_ptr_t;
  memory::unique_ptr<ast_type> m_shared_object_header;

  // Reference types
  containers::hash_map<const ast_type*, memory::unique_ptr<ast_type>>
      m_reference_types;
  containers::hash_map<const ast_type*, memory::unique_ptr<ast_type>>
      m_shared_types;
  containers::hash_map<const ast_type*, memory::unique_ptr<ast_type>>
      m_weak_types;
  containers::hash_map<core::pair<uint32_t, const ast_type*>,
      memory::unique_ptr<ast_type>,
      containers::pair_hasher<uint32_t, const ast_type*>,
      containers::pair_equality<uint32_t, const ast_type*>>
      m_static_array_types;
  containers::hash_map<const ast_type*, memory::unique_ptr<ast_type>>
      m_runtime_array_types;
  containers::hash_map<containers::string_view, memory::unique_ptr<ast_type>>
      m_structure_types;
  containers::hash_map<containers::dynamic_array<const ast_type*>,
      memory::unique_ptr<ast_type>>
      m_function_pointer_types;

  containers::deque<ast_type*> m_all_types;
};
}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/type_manager.inl"

#endif  // __WN_SCRIPTING_TYPE_MANAGER_H__