// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SUPPORT_REGEX_H__
#define __WN_SUPPORT_REGEX_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/resource_manager.h"

#include <re2/re2.h>

namespace wn {
namespace support {

class regex {
public:
  explicit regex(const char* val);
  ~regex();
  bool is_valid() const;

private:
  re2::RE2 m_regex;
};

struct regex_manager : scripting::resource_manager {
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  regex_manager(memory::allocator* _allocator);

  bool convert_to_function(containers::string_view _resource_name,
      containers::string_view _resource_data, containers::string* _dat,
      core::optional<uintptr_t>* _user_data) override;

private:
  memory::allocator* m_allocator;
  containers::hash_map<containers::string, memory::unique_ptr<regex>> m_regexes;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_REGEX_H__
