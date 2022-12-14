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

  scripting::slice<uint8_t> split(const char* split_chars);
  scripting::slice<uint8_t> next_split(
      scripting::slice<uint8_t>, const char* split_chars);

private:
  containers::string m_string;
};

class splits {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  explicit splits(memory::allocator* _alloc);
  ~splits();
  scripting::shared_cpp_pointer<splits> split_split(
      int32_t idx, const char* split_chars);
  int32_t num_splits() {
    return static_cast<int32_t>(m_splits.size());
  }
  scripting::slice<uint8_t> get_split_i(int32_t i) {
    const uint32_t x = static_cast<uint32_t>(i);
    if (x < 0 || x > m_splits.size()) {
      return scripting::slice<uint8_t>(nullptr, {0});
    }
    return m_splits[x];
  }
  scripting::slice<uint8_t> get_split_s(scripting::wn_size_t i) {
    if (i.val > m_splits.size()) {
      return scripting::slice<uint8_t>(nullptr, {0});
    }
    return m_splits[i.val];
  }
  containers::dynamic_array<scripting::slice<uint8_t>> m_splits;
  scripting::shared_cpp_pointer<string> m_base_string;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_STRING_H__
