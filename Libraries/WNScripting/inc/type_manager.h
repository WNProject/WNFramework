// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_TYPE_MANAGER_H__
#define __WN_SCRIPTING_TYPE_MANAGER_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNContainers/inc/WNString.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/externals.h"
#include "WNScripting/inc/resource_manager.h"
#include "core/inc/optional.h"

namespace wn {
namespace scripting {
using void_f = void (*)();

struct ast_type;
class struct_definition;
struct ast_function;
struct ast_function_call_expression;
struct ast_expression;
struct ast_vtable;
class node;

enum class ast_type_classification {
  primitive,
  reference,
  shared_reference,
  weak_reference,
  struct_type,
  static_array,
  runtime_array,
  slice_type,
  function_pointer,
  extern_type,
};

using used_type_set = containers::hash_set<const ast_type*>;
using used_function_set = containers::hash_set<const ast_function*>;

class type_manager {
public:
  type_manager(memory::allocator* _allocator, logging::log* _log);
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
  void export_script_type();

  template <typename T>
  containers::string_view get_mangled_name() const {
    return internal_get_mangled_name(get_type<T>());
  }

  template <typename T>
  typename core::enable_if<!is_script_pointer<T>::value &&
                               !is_shared_script_pointer<T>::value,
      const ast_type*>::type
  get_type() {
    return m_externally_visible_types.find(core::type_id<T>::value())->second;
  }

  template <typename T>
  typename core::enable_if<is_script_pointer<T>::value, const ast_type*>::type
  get_type() {
    return get_reference_of(get_type<typename T::value_type>(),
        ast_type_classification::reference, nullptr);
  }

  template <typename T>
  typename core::enable_if<is_shared_script_pointer<T>::value,
      const ast_type*>::type
  get_type() {
    return get_reference_of(get_type<typename T::value_type>(),
        ast_type_classification::shared_reference, nullptr);
  }

  template <typename T>
  const ast_type* get_type() const {
    return m_externally_visible_types.find(core::type_id<T>::value())->second;
  }

  template <typename... Args>
  containers::dynamic_array<containers::string_view> get_mangled_names() const {
    return containers::dynamic_array<containers::string_view>(
        m_allocator, {get_mangled_name<Args>()...});
  }

  template <typename... Args>
  containers::dynamic_array<const ast_type*> get_types() {
    return containers::dynamic_array<const ast_type*>(
        m_allocator, {get_type<Args>()...});
  }

  size_t get_virtual_function(const containers::string_view& _name,
      const containers::dynamic_array<const ast_type*>& _types) const;

  template <typename... Args>
  containers::dynamic_array<const ast_type*> get_types() const {
    return containers::dynamic_array<const ast_type*>(
        m_allocator, {get_type<Args>()...});
  }

  containers::string_view get_base_name(const ast_type* t) const;

  ast_type* get_reference_of(const ast_type* _type,
      ast_type_classification _reference_type, used_type_set* _used);

  ast_type* get_array_of(const ast_type* _type, uint32_t _size,
      used_type_set* _used, bool allow_class = false);
  ast_type* get_runtime_array_of(const ast_type* _type, used_type_set* _used);
  ast_type* get_slice_of(
      const ast_type* _type, uint32_t _size, used_type_set* _used);

  ast_type* get_or_register_struct(containers::string_view _name,
      used_type_set* _used, used_function_set* _used_functions);
  ast_type* register_child_struct(containers::string_view _name,
      ast_type* _struct_type, used_type_set* _used);
  containers::dynamic_array<const ast_type*> get_initialialization_order(
      memory::allocator* _allocator,
      const containers::hash_set<const ast_type*>& _types);
  ast_vtable* make_vtable(const containers::string_view& _struct_name);
  void finalize_functions(
      const containers::deque<memory::unique_ptr<ast_function>>& functions);

  void finalize_builtin_functions(
      const containers::hash_set<const ast_function*>* _used_builtins,
      containers::deque<const ast_function*>* _ordered_used_builtins);

  void finalize_used_externals(
      const containers::hash_set<const ast_function*>* _used_externals,
      containers::deque<const ast_function*>* _ordered_used_externals);

  const ast_type* resolve_function_ptr_type(
      containers::dynamic_array<const ast_type*> _types, used_type_set* _used);

  ast_function* add_external(
      const external_function& function, bool _system, bool _is_member);
  // finalize_builtins must be called after _allocate, _free, and
  // _allocate_runtime_array have been called
  void finalize_builtins() {
    add_builtins();
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
  const ast_type* size_t_t(used_type_set* _used) {
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

  const containers::deque<memory::unique_ptr<ast_function>>&
  external_functions() const {
    return m_externals;
  }

  ast_type* export_script_type(containers::string_view _type);

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

  bool register_struct_definition(const struct_definition* _def);
  ast_type* struct_definition_resolved(containers::string_view _name) const;
  void set_struct_definition_resolved(
      containers::string_view _name, ast_type* _type);
  const struct_definition* get_struct_definition(
      containers::string_view _name) const;

  void set_member_functions_resolved(containers::string_view _name);
  bool member_functions_resolved(containers::string_view _name) const;

  const ast_function* allocate_shared(
      containers::hash_set<const ast_function*>* _used_functions) {
    if (_used_functions) {
      _used_functions->insert(m_allocate_shared);
    }
    return m_allocate_shared;
  }
  const ast_function* release_shared(
      containers::hash_set<const ast_function*>* _used_functions) {
    if (_used_functions) {
      _used_functions->insert(m_release_shared);
    }
    return m_release_shared;
  }
  const ast_function* assign_shared(
      containers::hash_set<const ast_function*>* _used_functions) {
    if (_used_functions) {
      _used_functions->insert(m_assign_shared);
      _used_functions->insert(m_release_shared);
    }
    return m_assign_shared;
  }
  const ast_function* return_shared(
      containers::hash_set<const ast_function*>* _used_functions) {
    if (_used_functions) {
      _used_functions->insert(m_return_shared);
    }
    return m_return_shared;
  }
  const ast_function* strlen(
      containers::hash_set<const ast_function*>* _used_functions) {
    if (_used_functions) {
      _used_functions->insert(m_strlen);
    }
    return m_strlen;
  }

  bool is_pass_by_reference(const ast_type* _type) const;

  bool add_named_constant(const ast_type* _type, containers::string_view _name,
      const containers::string_view& _value);

  // register_resource_type takes an ast_type, a resource_name and
  // a callback. When a resource node is processed, the callback will
  // be called with the parameter associated with the resource.
  // The callback is expected to return a void* identifier if the
  // resource is valid, and nullptr if the resource is not.

  // Although not strictly required, it is generally expected that
  //   multpile calls with the same parameter data will give the
  //   same identifier.
  bool register_resource_type(
      const ast_type* _type, resource_manager* resource);

  bool register_resource_data(const containers::string_view& _file,
      const containers::string_view& _res_name);
  containers::string get_resource_data(const containers::string_view& _file) {
    auto it = m_resource_data.find(_file.to_string(m_allocator));
    if (it != m_resource_data.end()) {
      return containers::string(m_allocator, it->second);
    }
    return containers::string();
  }

  // get_resource tries to find the resource of the given name with the given
  //  resource data.
  // If the name or data is invalid, this is ignored.
  const ast_type* get_resource(containers::string_view resource_name,
      containers::string_view resource_data, containers::string* data,
      core::optional<uintptr_t>* _user_data);

  struct named_constant {
    containers::string _value;
    const ast_type* _type;
  };

  bool get_named_constant(
      const containers::string_view& _name, named_constant* _out);

private:
  ast_type* register_external_type(containers::string_view _type,
      uint32_t _size, ast_type* _parent_type = nullptr);
  void add_builtins() {
    add_builtin_functions();
  }

  bool add_builtin_functions();
  void add_allocate_shared();
  void add_release_shared();
  void add_assign_shared();
  void add_return_shared();
  void add_strlen();

  void finalize_external_type(ast_type* _type) const;
  containers::string_view internal_get_mangled_name(
      const ast_type* _type) const;
  memory::allocator* m_allocator;
  logging::log* m_log;

  containers::hash_map<containers::string_view, memory::unique_ptr<ast_type>>
      m_external_types;
  containers::hash_map<uintptr_t, ast_type*> m_externally_visible_types;
  containers::deque<memory::unique_ptr<ast_function>> m_externals;
  containers::hash_map<containers::string_view, ast_function*>
      m_externals_by_name;
  containers::deque<memory::unique_ptr<ast_function>> m_builtins;

  memory::unique_ptr<ast_type> m_void_t;
  memory::unique_ptr<ast_type> m_void_ptr_t;
  memory::unique_ptr<ast_type> m_bool_t;
  memory::unique_ptr<ast_type> m_size_t;
  memory::unique_ptr<ast_type> m_function_t;
  memory::unique_ptr<ast_type> m_nullptr_t;
  memory::unique_ptr<ast_type> m_vtable_t;
  memory::unique_ptr<ast_type> m_cstr_t;
  const ast_function* m_allocate_shared;
  const ast_function* m_release_shared;
  const ast_function* m_assign_shared;
  const ast_function* m_return_shared;
  const ast_function* m_strlen;

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
  containers::hash_map<core::pair<uint32_t, const ast_type*>,
      memory::unique_ptr<ast_type>,
      containers::pair_hasher<uint32_t, const ast_type*>,
      containers::pair_equality<uint32_t, const ast_type*>>
      m_slice_types;

  struct res_type {
    const ast_type* type;
    resource_manager* m_resource;
  };

  containers::hash_map<containers::string, res_type> m_resource_types;
  containers::hash_map<containers::string, containers::string> m_resource_data;

  containers::deque<ast_type*> m_all_types;

  struct struct_definition_data {
    memory::unique_ptr<struct_definition> st_def;
    ast_type* m_resolved;
    bool member_functions_resolved;
  };
  containers::hash_map<containers::string_view, struct_definition_data>
      m_struct_definitions;
  containers::deque<memory::unique_ptr<ast_vtable>> m_vtables;
  memory::unique_ptr<ast_function_call_expression> call_function(
      const node* _base_node, const ast_function* _func,
      containers::contiguous_range<memory::unique_ptr<ast_expression>>
          _expressions);

  containers::hash_map<containers::string, named_constant> m_named_constants;
};
}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/type_manager.inl"

#endif  // __WN_SCRIPTING_TYPE_MANAGER_H__
