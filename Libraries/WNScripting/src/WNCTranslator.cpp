// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNCTranslator.h"
#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/c_compiler.h"

namespace wn {
namespace scripting {

c_translator::c_translator(memory::allocator* _allocator,
    file_system::mapping* _source_mapping, file_system::mapping* _dest_mapping,
    logging::log* _log)
  : translator(_allocator),
    m_source_mapping(_source_mapping),
    m_dest_mapping(_dest_mapping),
    m_compilation_log(_log) {
  m_type_manager.m_externals.push_back(external_function{"_allocate",
      containers::dynamic_array<ast_type*>(m_allocator,
          {m_type_manager.m_void_ptr_t.get(), m_type_manager.m_size_t.get()})});

  m_type_manager.m_externals.push_back(external_function{"_free",
      containers::dynamic_array<ast_type*>(m_allocator,
          {m_type_manager.m_void_t.get(), m_type_manager.m_void_ptr_t.get()})});
}

parse_error c_translator::translate_file_with_error(
    const char* _file, bool _dump_ast_on_failure) {
  file_system::result res;
  file_system::file_ptr file = m_source_mapping->open_file(_file, res);

  if (!file) {
    return parse_error::does_not_exist;
  }

  memory::unique_ptr<ast_script_file> parsed_file = parse_script(m_allocator,
      _file, file->typed_range<char>(), &m_type_manager, _dump_ast_on_failure,
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return scripting::parse_error::parse_failed;
  }

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
  auto type = memory::make_unique<ast_type>(m_allocator);

  auto ret_type = type.get();
  type->m_classification = ast_type_classification::extern_type;
  type->m_name = containers::string(m_allocator, _name);
  type->calculate_mangled_name(m_allocator);
  m_type_manager.m_external_types[_name.to_string(m_allocator)] =
      core::move(type);

  return ret_type;
}

}  // namespace scripting
}  // namespace wn
