// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_TYPE_MANAGER_H__
#define __WN_SCRIPTING_TYPE_MANAGER_H__

#include "WNContainers/inc/WNHashMap.h"
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
  dynamic_array,
  function_pointer,
  extern_type,
};

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

private:
  ast_type* register_external_type(containers::string_view _type,
      uint32_t _size, ast_type* _parent_type = nullptr);
  void finalize_external_type(ast_type* _type) const;
  containers::string_view internal_get_mangled_name(void* idx) const;
  memory::allocator* m_allocator;

public:
  containers::hash_map<containers::string, memory::unique_ptr<ast_type>>
      m_external_types;
  containers::hash_map<void*, ast_type*> m_externally_visible_types;
  containers::dynamic_array<external_function> m_externals;
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

  template <typename T>
  containers::string_view get_mangled_name() const {
    return internal_get_mangled_name(c_type_tag<T>::get_unique_identifier());
  }

  template <typename T>
  ast_type* get_type() const {
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
  containers::dynamic_array<ast_type*> get_types() const {
    return containers::dynamic_array<ast_type*>(
        m_allocator, {get_type<Args>()...});
  }

  ast_type* get_reference_of(
      const ast_type* _type, ast_type_classification _reference_type);

  ast_type* get_array_of(const ast_type* _type, uint32_t _size);
};
}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/type_manager.inl"

#endif  // __WN_SCRIPTING_TYPE_MANAGER_H__