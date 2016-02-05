// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace WNLogging {
class WNLog;
}

namespace wn {
namespace scripting {
class script_file;
class type_validator;
// Simple helper that parses a script and runs any
// passes that are required to make the AST valid.
memory::unique_ptr<wn::scripting::script_file> parse_script(
    memory::allocator* _allocator, type_validator* _validator,
    const char* file_name, containers::string_view view,
    WNLogging::WNLog* _log, size_t* _num_warnings, size_t* _num_errors);
}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
