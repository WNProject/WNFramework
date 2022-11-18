// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNCTranslator.h"

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNFunctional/inc/WNDefer.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/c_compiler.h"

namespace wn {
namespace scripting {

c_translator::c_translator(memory::allocator* _allocator,
    file_system::mapping* _source_mapping, file_system::mapping* _dest_mapping,
    logging::log* _log)
  : translator(_allocator, _log),
    m_source_mapping(_source_mapping),
    m_dest_mapping(_dest_mapping),
    m_compilation_log(_log),
    m_started_files(_allocator),
    m_finished_files(_allocator) {
  m_type_manager.add_external(
      external_function{
          "_allocate", containers::dynamic_array<const ast_type*>(
                           m_allocator, {m_type_manager.void_ptr_t(nullptr),
                                            m_type_manager.size_t_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{
          "_free", containers::dynamic_array<const ast_type*>(
                       m_allocator, {m_type_manager.void_t(nullptr),
                                        m_type_manager.void_ptr_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{"_allocate_actor",
          containers::dynamic_array<const ast_type*>(
              m_allocator, {m_type_manager.void_ptr_t(nullptr),
                               m_type_manager.size_t_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{
          "_free_actor", containers::dynamic_array<const ast_type*>(m_allocator,
                             {m_type_manager.void_t(nullptr),
                                 m_type_manager.void_ptr_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{"_allocate_actor_call",
          containers::dynamic_array<const ast_type*>(
              m_allocator, {m_type_manager.void_ptr_t(nullptr),
                               m_type_manager.size_t_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{"_free_actor_call",
          containers::dynamic_array<const ast_type*>(
              m_allocator, {m_type_manager.void_t(nullptr),
                               m_type_manager.void_ptr_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{"_call_actor_function",
          containers::dynamic_array<const ast_type*>(
              m_allocator, {m_type_manager.void_t(nullptr),
                               m_type_manager.integral(32, nullptr),
                               m_type_manager.void_ptr_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{
          "update_actors", containers::dynamic_array<const ast_type*>(
                               m_allocator, {m_type_manager.void_t(nullptr)})},
      true, false);

  m_type_manager.add_external(
      external_function{"_allocate_runtime_array",
          containers::dynamic_array<const ast_type*>(
              m_allocator, {m_type_manager.void_ptr_t(nullptr),
                               m_type_manager.size_t_t(nullptr),
                               m_type_manager.size_t_t(nullptr)})},
      true, false);
  m_type_manager.finalize_builtins();
}

parse_error c_translator::translate_file_with_error(
    const containers::string_view _file, bool _dump_ast_on_failure) {
  if (m_finished_files.find(_file.to_string(m_allocator)) !=
      m_finished_files.end()) {
    return parse_error::ok;
  }
  for (size_t i = 0; i < m_started_files.size(); ++i) {
    if (m_started_files[i] == _file) {
      m_compilation_log->log_error("Recursive include detected");
      m_compilation_log->log_error("        ", _file);
      for (int32_t j = int32_t(m_started_files.size() - 1); j >= 0; --j) {
        m_compilation_log->log_error("        ->", m_started_files[j]);
      }
      return parse_error::invalid_parameters;
    }
  }
  m_started_files.push_back(_file);
  functional::defer clean(functional::function<void()>(
      m_allocator, [this]() { m_started_files.pop_back(); }));
  file_system::result res;
  file_system::file_ptr file;
  containers::string synthetic_contents(m_allocator);
  bool use_synthetic_contents = false;

  if (!_file.ends_with(".wns")) {
    auto pt = _file.find_last_of(".");
    if (pt == containers::string_view::npos) {
      m_compilation_log->log_error("Missing file extension");
      return parse_error::eUnsupported;
    }
    if (m_extension_handlers.find(_file.substr(pt).to_string(m_allocator)) ==
        m_extension_handlers.end()) {
      m_compilation_log->log_error(
          "Unhandled file extension ", _file.substr(pt));
      return parse_error::eUnsupported;
    }
    containers::string out_data(m_allocator);
    auto ext_handler =
        m_extension_handlers[_file.substr(pt).to_string(m_allocator)];
    convert_type convert_res = ext_handler->convert_file(m_compilation_log,
        m_source_mapping, _file, &out_data, &synthetic_contents);
    if (convert_res == convert_type::failed) {
      m_compilation_log->log_error(
          "Resource file ", _file, " was not handled by resource handler");
      return parse_error::eWNInvalidFile;
    }
    m_type_manager.register_resource_data(_file, out_data);
    use_synthetic_contents = convert_res == convert_type::success;
  }
  if (!use_synthetic_contents) {
    file = m_source_mapping->open_file(_file, res);

    if (!file) {
      m_compilation_log->log_error("Could not find file", _file);
      return parse_error::does_not_exist;
    }
  }

  memory::unique_ptr<ast_script_file> parsed_file = parse_script(m_allocator,
      _file,
      use_synthetic_contents ? synthetic_contents.to_contiguous_range()
                             : file->typed_range<char>(),
      functional::function<bool(containers::string_view)>(m_allocator,
          [this, _dump_ast_on_failure](containers::string_view include) {
            return translate_file_with_error(include, _dump_ast_on_failure) ==
                           scripting::parse_error::ok
                       ? true
                       : false;
          }),
      functional::function<bool(
          containers::string_view, containers::string_view, bool)>(m_allocator,
          [this, _dump_ast_on_failure](containers::string_view resource_type,
              containers::string_view resource_name, bool instantiated) {
            auto it = m_resources.find(resource_type.to_string(m_allocator));
            if (it == m_resources.end()) {
              return true;
            }
            if (it->second->must_be_instantiated() && !instantiated) {
              m_compilation_log->log_error("Resource ", resource_type,
                  " can only be used in an instantiated context");
              return false;
            }
            if (!it->second->can_be_instantiated() && instantiated) {
              m_compilation_log->log_error("Resource ", resource_type,
                  " can only not be used in an instantiated context");
              return false;
            }
            containers::string str(m_allocator);
            if (!it->second->setup_resource(resource_name, &str)) {
              return false;
            }
            if (str.empty()) {
              return true;
            }

            return translate_file_with_error(str, _dump_ast_on_failure) ==
                           scripting::parse_error::ok
                       ? true
                       : false;
          }),

      &m_type_manager, _dump_ast_on_failure, m_compilation_log, &m_num_warnings,
      &m_num_errors);

  if (parsed_file == nullptr) {
    return scripting::parse_error::parse_failed;
  }
  m_finished_files.insert(_file.to_string(m_allocator));

  c_compiler compiler(m_allocator);
  compiler.compile(parsed_file.get());

  const containers::string& output_string = compiler.get_output();
  containers::string output_filename(m_allocator, _file);
  output_filename.append(".c");

  file_system::file_ptr output_file =
      m_dest_mapping->create_file(output_filename, res);
  if (!output_file) {
    return parse_error::cannot_open_file;
  }
  if (!output_file->resize(output_string.size())) {
    return parse_error::cannot_open_file;
  }
  memory::memory_copy(output_file->typed_data<char>(), output_string.data(),
      output_string.size());
  return parse_error::ok;
}

ast_type* c_translator::register_external_type(containers::string_view _name) {
  return m_type_manager.register_external_type(_name);
}

}  // namespace scripting
}  // namespace wn
