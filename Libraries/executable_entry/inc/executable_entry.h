// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_EXECUTABLE_ENTRY_EXECUTABLE_ENTRY_H__
#define __WN_EXECUTABLE_ENTRY_EXECUTABLE_ENTRY_H__

#include "core/inc/base.h"

namespace wn {
namespace executable {

void wn_dummy();

#if defined _WN_ANDROID || defined _WN_LINUX

struct foo {
  foo() {
    wn_dummy();
  }
};

// This will force my_foo to be created, which will in turn
// force test_dummy to be called, which exists in WNTestHarness.cpp.
// This means that WNTestHarness.cpp will be examined, and
// wn_main will be found, and actually linked in.
// It is not needed on other platforms because they are creating
// an executable rather than a shared object library.
static foo __attribute__((used)) my_foo;
#endif  //_WN_ANDROID || _WN_LINUX

}  // namespace executable
}  // namespace wn

#endif  // __WN_EXECUTABLE_ENTRY_EXECUTABLE_ENTRY_H__
