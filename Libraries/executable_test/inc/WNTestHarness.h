// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_EXECUTABLE_TEST_TEST_HARNESS_H__
#define __WN_EXECUTABLE_TEST_TEST_HARNESS_H__

#include "WNTestUtilities/inc/WNTestHarness.h"
#include "executable_data/inc/executable_data.h"
#include "executable_entry/inc/executable_entry.h"

namespace wn {
namespace testing {

#if defined _WN_ANDROID || defined _WN_LINUX
void test_dummy();

struct foo final {
  foo() {
    test_dummy();
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

extern const executable::executable_data* k_executable_data;

void init_test_framework();

}  // namespace testing
}  // namespace wn

#endif  // __WN_EXECUTABLE_TEST_TEST_HARNESS_H__
