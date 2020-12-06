// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SUPPORT_SUBPROCESS_H__
#define __WN_SUPPORT_SUBPROCESS_H__

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace support {

class subprocess {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  subprocess(const char* _program, memory::allocator* _allocator,
      const scripting::slice<const char*>& _args);
  ~subprocess();

private:
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_SUBPROCESS_H__
