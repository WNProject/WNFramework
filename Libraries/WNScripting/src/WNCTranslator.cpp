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
  : translator(),
    m_allocator(_allocator),
    m_source_mapping(_source_mapping),
    m_dest_mapping(_dest_mapping),
    m_compilation_log(_log) {}

parse_error c_translator::translate_file_with_error(
    const char* _file, bool _dump_ast_on_failure) {
  file_system::result res;
  file_system::file_ptr file = m_source_mapping->open_file(_file, res);

  if (!file) {
    return parse_error::does_not_exist;
  }
  containers::array<uint32_t, 1> allocate_params = {
      static_cast<uint32_t>(type_classification::size_type)};
  containers::array<uint32_t, 1> free_params = {
      static_cast<uint32_t>(type_classification::void_ptr_type)};

  // We have a set of functions that we MUST expose to the scripting
  // engine.
  containers::array<external_function, 2> required_functions;
  required_functions[0] = {"_allocate",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      allocate_params};
  required_functions[1] = {"_free",
      static_cast<uint32_t>(type_classification::void_type), free_params};

  memory::unique_ptr<ast_script_file> parsed_file = parse_script(m_allocator,
      _file, file->typed_range<char>(), required_functions,
      _dump_ast_on_failure, m_compilation_log, &m_num_warnings, &m_num_errors);

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

}  // namespace scripting
}  // namespace wn
