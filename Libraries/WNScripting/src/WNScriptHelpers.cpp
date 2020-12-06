// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptHelpers.h"

#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/parse_ast_convertor.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"

namespace wn {
namespace scripting {

memory::unique_ptr<ast_script_file> parse_script(memory::allocator* _allocator,
    const containers::string_view _file_name, containers::string_view view,
    functional::function<bool(containers::string_view)> _handle_include,
    functional::function<bool(
        containers::string_view, containers::string_view, bool instantiated)>
        _handle_resource,
    type_manager* _type_manager, bool _dump_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors) {
  auto file_name = _file_name.to_string(_allocator);
  WNScriptASTLexer::InputStreamType input(
      const_cast<ANTLR_UINT8*>(
          reinterpret_cast<const ANTLR_UINT8*>(view.data())),
      ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(view.size()),
      const_cast<ANTLR_UINT8*>(
          reinterpret_cast<const ANTLR_UINT8*>(file_name.c_str())));

  WNScriptASTLexer lexer(&input);
  WNScriptASTParser::TokenStreamType tStream(
      ANTLR_SIZE_HINT, lexer.get_tokSource());
  WNScriptASTParser parser(&tStream);
  parser.set_allocator(_allocator);
  auto ptr = memory::unique_ptr<script_file>(_allocator, parser.program());
  if (parser.getNumberOfSyntaxErrors() != 0 ||
      lexer.getNumberOfSyntaxErrors() != 0) {
    if (_num_errors) {
      _num_errors += parser.getNumberOfSyntaxErrors();
    }
    if (_dump_ast_on_failure) {
      ptr->print_node(_log, wn::logging::log_level::error);
    }
    return nullptr;
  }

  parse_ast_convertor p;
  return p.convert_parse_tree_to_ast(_allocator, _type_manager, _log,
      _handle_include, _handle_resource, ptr.get());
}

}  // namespace scripting
}  // namespace wn
