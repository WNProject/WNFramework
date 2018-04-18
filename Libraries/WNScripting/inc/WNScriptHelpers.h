// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace scripting {
class script_file;
struct ast_script_file;

struct external_function {
  containers::string_view name;
  uint32_t ret_type;
  containers::contiguous_range<uint32_t> params;
};

// Simple helper that parses a script and runs any
// passes that are required to make the AST valid.

memory::unique_ptr<ast_script_file> parse_script(memory::allocator* _allocator,
    const char* file_name, containers::string_view view,
    const containers::contiguous_range<external_function>& externals,
    bool _print_ast_on_failure, logging::log* _log, size_t* _num_warnings,
    size_t* _num_errors);
}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
