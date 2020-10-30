// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SUPPORT_REGEX_H__
#define __WN_SUPPORT_REGEX_H__

#include "WNRe2/inc/re2.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/resource_manager.h"

namespace wn {
namespace support {

class regex {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  explicit regex(const char* val);
  ~regex();

private:
  re2::RE2 m_regex;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_REGEX_H__
