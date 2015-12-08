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
memory::allocated_ptr<wn::scripting::script_file> parse_script(
    memory::allocator* _allocator, const wn_char* file_name,
    containers::string_view view, WNLogging::WNLog* _log,
    wn_size_t* _num_warnings, wn_size_t* _num_errors);
}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
