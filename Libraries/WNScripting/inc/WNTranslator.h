// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_TRANSLATOR_H__
#define __WN_SCRIPTING_TRANSLATOR_H__

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/resource_manager.h"
#include "WNScripting/inc/type_manager.h"

namespace wn {
namespace scripting {

// Base interface for file translation.
// Any implementor is responsible for overloading
// translate_file to perform the actual work.
class translator {
public:
  translator(memory::allocator* _allocator, logging::log* _log)
    : m_num_warnings(0),
      m_num_errors(0),
      m_resources(_allocator),
      m_extension_handlers(_allocator),
      m_allocator(_allocator),
      m_type_manager(_allocator, _log) {}

  virtual ~translator() = default;

  parse_error translate_file(const containers::string_view _file) {
    return translate_file_with_error(_file, false);
  }

  virtual parse_error translate_file_with_error(
      const containers::string_view file, bool _dump_ast_on_failure) = 0;

  size_t errors() const {
    return m_num_errors;
  }
  size_t warnings() const {
    return m_num_warnings;
  }

  template <typename R, typename... Args>
  bool inline register_function(containers::string_view _name);

  template <typename R, typename... Args>
  bool inline register_cpp_function(
      containers::string_view _name, R (*)(Args...));

  template <typename R>
  bool inline register_resource(memory::unique_ptr<resource_manager> resource);

  template <typename T>
  bool register_cpp_type();

  template <typename T>
  bool register_child_cpp_type();

  template <typename T>
  void inline export_script_type();

  template <typename T>
  bool register_named_constant(
      const containers::string_view& name, const T& value);

protected:
  virtual ast_type* register_external_type(containers::string_view _name) = 0;
  size_t m_num_warnings;
  size_t m_num_errors;
  containers::hash_map<containers::string, memory::unique_ptr<resource_manager>>
      m_resources;
  containers::hash_map<containers::string, resource_manager*>
      m_extension_handlers;
  memory::allocator* m_allocator;
  type_manager m_type_manager;
};

template <typename R, typename... Args>
bool inline translator::register_function(containers::string_view _name) {
  containers::dynamic_array<const ast_type*> params =
      m_type_manager.get_types<R, Args...>();
  m_type_manager.add_external(
      external_function{_name, core::move(params)}, true, false);
  return true;
}

template <typename R, typename... Args>
bool inline translator::register_cpp_function(
    containers::string_view _name, R (*)(Args...)) {
  containers::dynamic_array<const ast_type*> params =
      m_type_manager.get_types<R, Args...>();
  m_type_manager.add_external(
      external_function{_name, core::move(params)}, true, false);
  return true;
}

template <typename T>
bool inline translator::register_cpp_type() {
  m_type_manager.register_cpp_type<T>(nullptr);
  return true;
}

template <typename T>
bool inline translator::register_child_cpp_type() {
  m_type_manager.register_child_cpp_type<T>(nullptr);
  return true;
}

template <typename T>
void inline translator::export_script_type() {
  m_type_manager.export_script_type<T>();
}

template <typename T>
bool inline translator::register_named_constant(
    const containers::string_view& _name, const T& value) {
  const ast_type* type = m_type_manager.get_type<T>();
  containers::string sc = get_script_constant<T>::str_const(m_allocator, value);
  return m_type_manager.add_named_constant(type, _name, sc);
}

template <typename R>
bool inline translator::register_resource(
    memory::unique_ptr<resource_manager> _resource) {
  if (!m_type_manager.register_resource_type(
          m_type_manager.get_type<R>(), _resource.get())) {
    return false;
  }
  if (!_resource->get_file_extension().empty()) {
    m_extension_handlers[_resource->get_file_extension().to_string(
        m_allocator)] = _resource.get();
  }
  m_resources[_resource->get_name().to_string(m_allocator)] =
      core::move(_resource);
  return true;
}

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_TRANSLATOR_H__
