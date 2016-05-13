// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"

namespace wn {
namespace scripting {

memory::unique_ptr<script_file> parse_script(memory::allocator* _allocator,
    wn::scripting::type_validator* _validator, const char* file_name,
    containers::string_view view, WNLogging::WNLog* _log, size_t* _num_warnings,
    size_t* _num_errors) {
  memory::unique_ptr<script_file> ptr;
  {
    WNScriptASTLexer::InputStreamType input(
        reinterpret_cast<const ANTLR_UINT8*>(view.data()), ANTLR_ENC_8BIT,
        static_cast<ANTLR_UINT32>(view.size()),
        reinterpret_cast<const ANTLR_UINT8*>(file_name));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(
        ANTLR_SIZE_HINT, lexer.get_tokSource());
    WNScriptASTParser parser(&tStream);
    parser.set_allocator(_allocator);
    ptr = std::move(
        memory::unique_ptr<script_file>(_allocator, parser.program()));
    if (parser.getNumberOfSyntaxErrors() != 0 ||
        lexer.getNumberOfSyntaxErrors() != 0) {
      if (_num_errors) {
        _num_errors += parser.getNumberOfSyntaxErrors();
      }
      return nullptr;
    }
    if (!run_dce_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      return nullptr;
    }

    if (!run_if_reassociation_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      return nullptr;
    }

    if (!run_member_reassociation_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      return nullptr;
    }
    if (!run_id_association_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      return nullptr;
    }
    if (!run_type_association_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      return nullptr;
    }
  }
  return std::move(ptr);
}

}  // namespace scripting
}  // namespace wn
