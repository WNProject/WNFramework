// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/types.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/manipulation.h"

namespace wn {
namespace scripting {

template <typename T>
struct mangled_name {};

template <>
struct mangled_name<int32_t> {
  static containers::string_view get() {
    return "i";
  }
};

template <>
struct mangled_name<float> {
  static containers::string_view get() {
    return "f";
  }
};

template <>
struct mangled_name<uint8_t> {
  static containers::string_view get() {
    return "h";
  }
};

template <>
struct mangled_name<bool> {
  static containers::string_view get() {
    return "b";
  }
};

template <>
struct mangled_name<void> {
  static containers::string_view get() {
    return "v";
  }
};

inline containers::string get_mangled_name(
    memory::allocator* _allocator, const containers::string_view& view) {
  containers::string str(_allocator);

  char count[11] = {0};

  memory::writeuint32(count, static_cast<uint32_t>(view.size()), 10);

  str += "_ZN3wns";
  str += count;
  str += view;
  str += "E";

  return str;
}

}  // namespace scripting
}  // namespace wn
