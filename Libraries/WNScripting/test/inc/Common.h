// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_USE_PRECOMPILED_HEADER
#pragma once
#endif

#ifndef __WN_SCRIPTING_TESTS_COMMON_H__
#define __WN_SCRIPTING_TESTS_COMMON_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/WNBase.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNTesting/inc/WNTestHarness.h"

namespace wn {
namespace scripting {

memory::unique_ptr<scripting::script_file> test_parse_file(const char* _file,
    file_system::mapping* _mapping, memory::allocator* _allocator,
    WNLogging::WNLog* _log, size_t* _num_warnings, size_t* _num_errors) {
  scripting::type_validator validator(_allocator);
  file_system::result res;
  file_system::file_ptr buff = _mapping->open_file(_file, res);
  EXPECT_NE(nullptr, buff);

  memory::unique_ptr<scripting::script_file> ptr =
      scripting::parse_script(_allocator, &validator, _file,
          buff->typed_range<char>(), _log, _num_warnings, _num_errors);

  return std::move(ptr);
}

memory::unique_ptr<scripting::script_file> test_parse_file(const char* _file,
    file_system::mapping* _mapping, memory::allocator* _allocator,
    size_t* _num_warnings, size_t* _num_errors) {
  return test_parse_file(_file, _mapping, _allocator,
      WNLogging::get_null_logger(), _num_warnings, _num_errors);
}

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_TESTS_COMMON_H__
