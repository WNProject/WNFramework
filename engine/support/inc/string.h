// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SUPPORT_STRING_H__
#define __WN_SUPPORT_STRING_H__

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace support {

class string {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  explicit string(const char* val, memory::allocator* _alloc);
  ~string();
  const char* c_str() {
    return m_string.c_str();
  }

private:
  containers::string m_string;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_STRING_H__
