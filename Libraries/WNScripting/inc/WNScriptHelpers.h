// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"

namespace wn {
namespace scripting {
class script_file;
struct ast_script_file;
class type_manager;
struct ast_type;

struct external_function {
  containers::string_view name;
  containers::dynamic_array<ast_type*> params;
};

// Use this type instead of normal size_t in order for
// size_t to automatically work between scripting and C++.
// This is because size_t is not a "proper" type in most
// compilers.
struct wn_size_t { size_t val; };

// Simple helper that parses a script and runs any
// passes that are required to make the AST valid.

memory::unique_ptr<ast_script_file> parse_script(memory::allocator* _allocator,
    const char* file_name, containers::string_view view,
    type_manager* _type_manager, bool _print_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors);
}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
