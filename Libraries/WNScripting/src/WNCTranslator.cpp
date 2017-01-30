// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNCTranslator.h"
#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNCGenerator.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

c_translator::c_translator(type_validator* _validator,
    memory::allocator* _allocator, file_system::mapping* _mapping,
    WNLogging::WNLog* _log)
  : translator(_validator),
    m_allocator(_allocator),
    m_file_mapping(_mapping),
    m_compilation_log(_log) {}

parse_error c_translator::translate_file(const char* file_) {
  file_system::result res;
  file_system::file_ptr file = m_file_mapping->open_file(file_, res);

  if (!file) {
    return parse_error::does_not_exist;
  }
  containers::array<uint32_t, 2> allocate_shared_params = {
    static_cast<uint32_t>(type_classification::size_type),
    static_cast<uint32_t>(type_classification::function_ptr_type)
  };

  containers::array<uint32_t, 1> deref_shared_params = {
    static_cast<uint32_t>(type_classification::void_ptr_type)
  };
  containers::array<uint32_t, 2> assign_params = {
    static_cast<uint32_t>(type_classification::void_ptr_type),
    static_cast<uint32_t>(type_classification::void_ptr_type)
  };
  // We have a set of functions that we MUST expose to the scripting
  // engine.
  containers::array<external_function, 4> required_functions;
  required_functions[0] = {"_allocate_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      allocate_shared_params};
  required_functions[1] = {"_deref_shared",
      static_cast<uint32_t>(type_classification::void_type),
      deref_shared_params};
  required_functions[2] = {"_assign_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      assign_params};
  // _return_shared is a little bit special.
  // It dereferences a value, but does not delete the object if it
  // ends up being 0. This will get injected at return sites,
  // since we know that the results will always be incremented.
  required_functions[3] = {"_return_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      deref_shared_params};
  memory::unique_ptr<script_file> parsed_file = parse_script(m_allocator,
      m_validator, file_, required_functions, file->typed_range<char>(),
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return scripting::parse_error::parse_failed;
  }
  ast_code_generator<ast_c_traits> generator(m_allocator);
  ast_c_translator translator(m_allocator, &generator, m_validator);
  generator.set_generator(&translator);
  run_ast_pass<ast_code_generator<ast_c_traits>>(
      &generator, static_cast<const script_file*>(parsed_file.get()));

  const containers::string& output_string(translator.get_output());
  containers::string output_filename(file_, m_allocator);
  output_filename.append(".c");

  file_system::file_ptr output_file =
      m_file_mapping->create_file(output_filename, res);
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
