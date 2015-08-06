// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingJITEngine.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"

namespace wn {
namespace scripting {

parse_error jit_engine::parse_file(const char* file) {
  memory::allocated_ptr<file_buffer> buff = m_file_manager->get_file(file);
  if (!buff) {
    return parse_error::does_not_exist;
  }
  
  WNScriptASTLexer::InputStreamType input(
      reinterpret_cast<const ANTLR_UINT8*>(buff->data()), ANTLR_ENC_8BIT,
      static_cast<ANTLR_UINT32>(buff->size()),
      reinterpret_cast<const ANTLR_UINT8*>(file));

  WNScriptASTLexer lexer(&input);
  WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT, lexer.get_tokSource());
  WNScriptASTParser parser(&tStream);
  parser.set_allocator(m_allocator);
  wn::memory::allocated_ptr<wn::scripting::script_file> ptr =
      wn::memory::default_allocated_ptr(m_allocator, parser.program());
  if (parser.getNumberOfSyntaxErrors() != 0 ||
    lexer.getNumberOfSyntaxErrors() != 0) {
    return parse_error::parse_failed;
  }
  return parse_error::ok;
}
}
}