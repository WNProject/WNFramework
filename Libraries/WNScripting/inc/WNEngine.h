// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNScriptTLS.h"
#include "WNScripting/inc/resource_manager.h"
#include "WNScripting/inc/type_manager.h"
#include "core/inc/utilities.h"

namespace wn {
namespace scripting {

struct ast_type;

template <typename R, typename... Args>
class script_function final {
public:
  script_function() : m_function(nullptr) {}

  operator bool() const {
    return (m_function != nullptr);
  }

private:
  friend class engine;

  template <typename U, typename UU, typename... UArgs>
  friend class scripting_virtual_object_function;

  template <typename U, typename UU, typename... UArgs>
  friend class scripting_virtual_actor_function;

  void_f m_function;
};

template <typename T, typename R, typename... Args>
class scripting_object_function final {
public:
  using ret_type = R;

private:
  ret_type do_(engine* _engine, script_pointer<T>& _ptr, Args... args);

  friend class engine;

  template <typename U>
  friend class script_pointer;

  template <typename U>
  friend class shared_script_pointer;

  script_function<R, script_pointer<T>, Args...> m_function;
};

template <typename T, typename R, typename... Args>
class scripting_actor_function final {
public:
  using ret_type = R;

private:
  ret_type do_(engine* _engine, script_pointer<T>& _ptr, Args... args);

  friend class engine;

  template <typename U>
  friend class script_pointer;

  template <typename U>
  friend class shared_script_pointer;

  template <typename U>
  friend class script_actor_pointer;

  script_function<R, script_pointer<T>, Args...> m_function;
};

template <typename T, typename R, typename... Args>
class scripting_virtual_object_function final {
public:
  using ret_type = R;

private:
  ret_type do_(engine* _engine, script_pointer<T>& _ptr, Args... args);

  friend class engine;

  template <typename U>
  friend class script_pointer;

  template <typename U>
  friend class shared_script_pointer;

  script_function<R, script_pointer<T>, Args...> m_function;
  size_t m_vtable_offset = 0;
  size_t m_vtable_idx = 0;
};

template <typename T, typename R, typename... Args>
class scripting_virtual_actor_function final {
public:
  using ret_type = R;

private:
  ret_type do_(engine* _engine, script_pointer<T>& _ptr, Args... args);

  friend class engine;

  template <typename U>
  friend class script_pointer;

  template <typename U>
  friend class shared_script_pointer;

  script_function<R, script_pointer<T>, Args...> m_function;
  size_t m_vtable_offset = 0;
  size_t m_vtable_idx = 0;
};

// Implementors of this class are expected to take
// in files, and give access to function pointer callable from
// C++.
class engine {
public:
  engine(memory::allocator* _allocator, logging::log* _log,
      memory::allocator* _support_allocator,
      memory::allocator* _actor_allocator, scripting_runtime* _runtime)
    : m_num_warnings(0),
      m_num_errors(0),
      m_allocator(_allocator),
      m_log(_log),
      m_type_manager(_allocator, _log),
      m_object_types(_allocator),
      m_actor_types(_allocator),
      m_resources(_allocator),
      m_additional_includes(_allocator) {
    // These exists to stop the compiler from assuming these are unused.
    (void)assign_type_names;
    (void)short_circuit_type_names;
    (void)type_classification_names;
    (void)arithmetic_type_names;
    (void)type_array_depth;
    (void)unary_type_names;
    (void)post_un_names;
    m_tls_data._engine = this;
    m_tls_data._object_types = &m_object_types;
    m_tls_data._actor_types = &m_actor_types;
    m_tls_data._log = m_log;
    m_tls_data._support_allocator = _support_allocator;
    m_tls_data._actor_allocator = _actor_allocator;
    m_tls_data._runtime = _runtime;
  }

  virtual ~engine() {}

  // Implemented in subclasses. Given a file, determine and prepare
  // and function pointers that may exist.
  virtual parse_error parse_file(const containers::string_view file) = 0;

  // Returns the number of errors that were encountered in
  // parsing the file.
  size_t errors() const {
    return m_num_errors;
  }

  // Returns the number of warnings encountered in parsing the file.
  size_t warnings() const {
    return m_num_warnings;
  }

  // Given a C-style function pointer, and a function name,
  // returns a function pointer from a parsed file that matches the
  // signature. The return types and all parameter types must
  // match exactly.
  template <typename R, typename... Args>
  bool get_function(containers::string_view _name,
      script_function<R, Args...>* _function) const;

  template <typename T, typename... Args>
  bool inline get_named_member_function(containers::string_view _name,
      containers::string_view _object_type,
      script_function<T, Args...>* _function) const;

  template <typename R, typename... Args>
  R invoke(const script_function<R, Args...>& _function, Args... _args) const;

  template <typename F, F function>
  bool inline register_function(containers::string_view _name);

  template <typename F, typename... Args>
  bool inline register_resource(memory::unique_ptr<resource_manager> res);

  template <typename T>
  size_t get_vtable_offset();

  template <typename T>
  void register_cpp_type();

  template <typename T>
  void register_child_cpp_type();

  template <typename T>
  void export_script_type();

  template <typename T>
  void export_script_actor_type();

  template <typename T>
  bool resolve_script_type();

  template <typename T>
  bool resolve_script_actor_type();

  template <typename T>
  bool register_named_constant(
      const containers::string_view& name, const T& value);

  inline containers::string get_resource_data(containers::string_view file);

  using void_func = script_function<void>;

  struct script_importer_base {};

  template <typename T, typename... Args>
  shared_cpp_pointer<T> make_shared_cpp(Args&&... args);

  template <typename T>
  struct script_type_importer : public script_importer_base {
    explicit script_type_importer(engine* _engine) : m_engine(_engine) {}

    template <typename R, typename... Args>
    void register_function(const containers::string_view& _name,
        scripting_object_function<T, R, Args...>* _ptr) {
      bool success = m_engine->get_member_function(_name, &_ptr->m_function);
      if (!success) {
        m_engine->m_log->log_error(
            "Could not find member ", _name, " on type ", T::exported_name());
      }
      m_success &= success;
    }

    template <typename R, typename... Args>
    void register_function(const containers::string_view& _name,
        scripting_actor_function<T, R, Args...>* _ptr) {
      bool success = m_engine->get_member_function(_name, &_ptr->m_function);
      if (!success) {
        m_engine->m_log->log_error(
            "Could not find member ", _name, " on type ", T::exported_name());
      }
      m_success &= success;
    }

    template <typename R, typename... Args>
    void register_function(const containers::string_view& _name,
        scripting_virtual_object_function<T, R, Args...>* _ptr) {
      size_t virtual_index =
          m_engine->get_virtual_function<R, script_pointer<T>, Args...>(_name);
      _ptr->m_vtable_idx = virtual_index;
      _ptr->m_vtable_offset = m_engine->get_vtable_offset<T>();
      bool success = true;
      if (_ptr->m_vtable_offset == static_cast<size_t>(-1)) {
        success = false;
        m_engine->m_log->log_error(
            T::exported_name(), " does not have any virtual functions");
      }

      if (virtual_index == static_cast<size_t>(-1)) {
        success = false;
        m_engine->m_log->log_error("Could not find virtual function ", _name,
            " on type ", T::exported_name());
      }

      m_success &= success;
    }
    template <typename R, typename... Args>
    void register_function(const containers::string_view& _name,
        scripting_virtual_actor_function<T, R, Args...>* _ptr) {
      size_t virtual_index =
          m_engine->get_virtual_function<R, script_pointer<T>, Args...>(_name);
      _ptr->m_vtable_idx = virtual_index;
      _ptr->m_vtable_offset = m_engine->get_vtable_offset<T>();
      bool success = true;
      if (_ptr->m_vtable_offset == static_cast<size_t>(-1)) {
        success = false;
        m_engine->m_log->log_error(
            T::exported_name(), " does not have any virtual functions");
      }

      if (virtual_index == static_cast<size_t>(-1)) {
        success = false;
        m_engine->m_log->log_error("Could not find virtual function ", _name,
            " on type ", T::exported_name());
      }

      m_success &= success;
    }

    engine* m_engine;
    bool m_success = true;
  };
  virtual void free_shared(void* v) const = 0;

protected:
  template <typename Enable, typename R, typename... Args>
  struct invoke_wrapper;

  template <typename T>
  bool inline resolve_script_type_internal(T* t);

  template <typename T>
  bool inline fill_parent_type(script_object_type* t);

  template <typename T>
  bool inline fill_parent_type(script_actor_type* t);

  template <typename R, typename... Args>
  bool get_member_function(containers::string_view _name,
      script_function<R, Args...>* _function) const;

  template <typename R, typename... Args>
  bool get_function_internal(containers::string_view _name,
      script_function<R, Args...>* _function, bool _is_member) const;

  template <typename R, typename... Args>
  bool get_named_function_internal(containers::string_view _name,
      containers::string_view _object_name,
      script_function<R, Args...>* _function, bool is_actor) const;

  template <typename R, typename... Args>
  size_t get_virtual_function(containers::string_view _name) const;

  size_t m_num_warnings;
  size_t m_num_errors;
  memory::allocator* m_allocator;
  logging::log* m_log;
  virtual void_f get_function_pointer(containers::string_view _name) const = 0;
  virtual bool register_c_function(containers::string_view name,
      containers::contiguous_range<const ast_type*> _types,
      void_f function) = 0;

  virtual bool register_resource(
      resource_manager* _resource, const ast_type* _type) = 0;

  virtual bool register_mangled_c_function(
      containers::string_view _name, void_f _function, bool _is_virtual) = 0;
  virtual size_t get_vtable_offset(const ast_type* _type) = 0;
  virtual void* allocate_shared(size_t size) = 0;

  type_manager m_type_manager;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_object_type>>
      m_object_types;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_actor_type>>
      m_actor_types;
  containers::deque<memory::unique_ptr<resource_manager>> m_resources;
  scripting_tls_data m_tls_data;
  containers::deque<containers::string> m_additional_includes;
};

template <typename T, typename R, typename... Args>
typename scripting_object_function<T, R, Args...>::ret_type
scripting_object_function<T, R, Args...>::do_(
    engine* _engine, script_pointer<T>& _ptr, Args... args) {
  return _engine->invoke(m_function, _ptr, args...);
}

template <typename T, typename R, typename... Args>
typename scripting_actor_function<T, R, Args...>::ret_type
scripting_actor_function<T, R, Args...>::do_(
    engine* _engine, script_pointer<T>& _ptr, Args... args) {
  return _engine->invoke(m_function, _ptr, args...);
}

template <typename T, typename R, typename... Args>
typename scripting_virtual_object_function<T, R, Args...>::ret_type
scripting_virtual_object_function<T, R, Args...>::do_(
    engine* _engine, script_pointer<T>& _ptr, Args... args) {
  uint8_t* ptr = static_cast<uint8_t*>(_ptr.unsafe_ptr());
  auto temp_function = m_function;

  ptr += m_vtable_offset;
  temp_function.m_function =
      reinterpret_cast<void_f>((*reinterpret_cast<void***>(ptr))[m_vtable_idx]);

  return _engine->invoke(temp_function, _ptr, args...);
}

template <typename T, typename R, typename... Args>
typename scripting_virtual_actor_function<T, R, Args...>::ret_type
scripting_virtual_actor_function<T, R, Args...>::do_(
    engine* _engine, script_pointer<T>& _ptr, Args... args) {
  uint8_t* ptr = static_cast<uint8_t*>(_ptr.unsafe_ptr());
  auto temp_function = m_function;

  ptr += m_vtable_offset;
  temp_function.m_function =
      reinterpret_cast<void_f>((*reinterpret_cast<void***>(ptr))[m_vtable_idx]);

  return _engine->invoke(temp_function, _ptr, args...);
}

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNEngine.inl"

#endif  // __WN_SCRIPTING_ENGINE_H__
