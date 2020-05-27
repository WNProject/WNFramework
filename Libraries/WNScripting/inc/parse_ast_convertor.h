// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_PARSE_AST_CONVERTOR_H__
#define __WN_SCRIPTING_PARSE_AST_CONVERTOR_H__

#include "WNMemory/inc/intrusive_ptr.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/type_manager.h"

namespace wn {
namespace scripting {

// We use the parse/ast convertor to handle
// both types and AST nodes.
// TODO: Allow us to export custom types onto this.
class parse_ast_convertor {
  // The convertor context is what can be used to pass
  // temporary data between commands.

public:
  memory::unique_ptr<ast_script_file> convert_parse_tree_to_ast(
      memory::allocator* _allocator, type_manager* _type_manager,
      logging::log* _log,
      functional::function<bool(containers::string_view)> _handle_includes,
      functional::function<bool(containers::string_view, containers::string_view)> _handle_resources,
      const script_file*) const;

private:
  struct convertor_context;
  friend struct convertor_context;
};

}  // namespace scripting
}  // namespace wn
#endif  // __WN_SCRIPTING_PARSE_AST_CONVERTOR_H__