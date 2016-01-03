// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNCGenerator.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace scripting {

c_translator::c_translator(type_validator* _validator,
    memory::allocator* _allocator, file_manager* _manager,
    WNLogging::WNLog* _log)
  : translator(_validator),
    m_allocator(_allocator),
    m_file_manager(_manager),
    m_compilation_log(_log) {}

parse_error c_translator::translate_file(const char* file) {
  memory::allocated_ptr<file_buffer> buff = m_file_manager->get_file(file);

  if (!buff) {
    return parse_error::does_not_exist;
  }

  memory::allocated_ptr<script_file> parsed_file = parse_script(m_allocator,
      m_validator, file, containers::string_view(buff->data(), buff->size()),
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == wn_nullptr) {
    return wn::scripting::parse_error::parse_failed;
  }
  type_validator validator(m_allocator);
  ast_code_generator<ast_c_traits> generator;
  ast_c_translator translator(m_allocator, &generator);
  generator.set_generator(&translator);
  run_ast_pass<ast_code_generator<ast_c_traits>>(&generator, parsed_file.get());

  containers::string output_string = translator.get_output();
  containers::string output_filename(file);
  output_filename.append(".c");

  m_file_manager->write_file(output_filename.c_str(), output_string);

  return parse_error::ok;
}

}  // namespace scripting
}  // namespace wn
