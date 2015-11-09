// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNCGenerator.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace scripting {

c_translator::c_translator(memory::allocator* _allocator,
                           file_manager* _manager, WNLogging::WNLog* _log)
    : m_allocator(_allocator),
      m_file_manager(_manager),
      m_compilation_log(_log) {}

parse_error c_translator::translate_file(const char* file) {
  memory::allocated_ptr<file_buffer> buff = m_file_manager->get_file(file);

  if (!buff) {
    return parse_error::does_not_exist;
  }

  memory::allocated_ptr<script_file> parsed_file = parse_script(
      m_allocator, file, containers::string_view(buff->data(), buff->size()),
      m_compilation_log);

  if (parsed_file == wn_nullptr) {
    return wn::scripting::parse_error::parse_failed;
  }
  type_validator validator(m_allocator);
  ast_c_translator translator(m_allocator);
  wn::scripting::ast_walker<ast_c_translator, ast_c_translator_traits> walker(
      &translator, WNLogging::get_null_logger(), &validator, m_allocator);

  containers::string output_string = walker.walk_script_file(parsed_file.get());

  containers::string output_filename(file);
  output_filename.append(".c");

  m_file_manager->write_file(output_filename.c_str(), output_string);

  return parse_error::ok;
}

}  // namespace scripting
}  // namespace wn
