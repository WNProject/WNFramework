// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNStringView.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace scripting {

memory::allocated_ptr<wn::scripting::script_file> parse_script(
    memory::allocator* _allocator, const wn_char* file_name,
    containers::string_view view, WNLogging::WNLog* _log) {
  wn::memory::allocated_ptr<wn::scripting::script_file> ptr;
  {
    WNScriptASTLexer::InputStreamType input(
        reinterpret_cast<const ANTLR_UINT8*>(view.data()), ANTLR_ENC_8BIT,
        static_cast<ANTLR_UINT32>(view.size()),
        reinterpret_cast<const ANTLR_UINT8*>(file_name));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT,
                                               lexer.get_tokSource());
    WNScriptASTParser parser(&tStream);
    parser.set_allocator(_allocator);
    parser.set_log(_log);
    ptr = std::move(
        wn::memory::default_allocated_ptr(_allocator, parser.program()));
    if (parser.getNumberOfSyntaxErrors() != 0 ||
        lexer.getNumberOfSyntaxErrors() != 0) {
      return wn_nullptr;
    }

    if (!run_id_association_pass(ptr.get(), _log)) {
      return wn_nullptr;
    }
    if (!run_type_association_pass(ptr.get(), _log)) {
      return wn_nullptr;
    }
  }
  return std::move(ptr);
}

}  // namespace scripting
}  // namespace wn
