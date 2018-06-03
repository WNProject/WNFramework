// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef _WN_SCRIPTING_INC_EXTERNALS_H__
#define _WN_SCRIPTING_INC_EXTERNALS_H__

#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace scripting {

template <typename T>
struct c_type_tag {
  static void* get_unique_identifier() {
    static void* x = nullptr;
    return &x;
  }
};

// Any type that is to be exported must specialize
// this template, and provide exported_name
// and exported_members functions.
template <typename Type>
struct exported_script_type {
public:
  // This is the name that is to be exported to the
  // scripting language.
  static containers::string_view exported_name();
  // This should export all scripting members for this
  // type. The passed parameter will follow the exporter
  // interface above.
  template <typename T>
  static void export_members(T* _t);
};

}  // namespace scripting
}  // namespace wn

#endif  // _WN_SCRIPTING_INC_EXTERNALS_H__