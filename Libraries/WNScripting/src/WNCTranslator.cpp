// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNCTranslator.h"
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

  memory::unique_ptr<script_file> parsed_file =
      parse_script(m_allocator, m_validator, file_, file->typed_range<char>(),
          true, m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return wn::scripting::parse_error::parse_failed;
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
