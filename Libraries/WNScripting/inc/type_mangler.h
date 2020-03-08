// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/types.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/manipulation.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNScriptTLS.h"

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

inline containers::string_view get_mangling(
    type_classification _type_classification) {
  switch (_type_classification) {
    case type_classification::int_type:
      return "i";
    case type_classification::float_type:
      return "f";
    case type_classification::char_type:
      return "h";
    case type_classification::bool_type:
      return "b";
    case type_classification::void_type:
      return "v";
    case type_classification::size_type:
      return "N3wns4sizeE";
    case type_classification::void_ptr_type:
      return "Pv";
    default:
      return "";
  }
}

template <typename T>
struct script_type {};

template <>
struct script_type<int32_t> {
  static type_classification get() {
    return type_classification::int_type;
  }
};

template <>
struct script_type<float> {
  static type_classification get() {
    return type_classification::float_type;
  }
};

template <>
struct script_type<uint8_t> {
  static type_classification get() {
    return type_classification::char_type;
  }
};

template <>
struct script_type<bool> {
  static type_classification get() {
    return type_classification::bool_type;
  }
};

template <>
struct script_type<void> {
  static type_classification get() {
    return type_classification::void_type;
  }
};

inline containers::string get_mangled_name(memory::allocator* _allocator,
    const containers::string_view& view,
    const containers::string_view& object_type = "") {
  containers::string str(_allocator);

  char count[11] = {0};

  str += "_ZN3wns";
  if (!object_type.empty()) {
    memory::writeuint32(count, static_cast<uint32_t>(object_type.size()), 10);
    str += count;
    str += object_type;
  }

  memory::writeuint32(count, static_cast<uint32_t>(view.size()), 10);
  str += count;
  str += view;
  str += "E";

  return str;
}

}  // namespace scripting
}  // namespace wn
