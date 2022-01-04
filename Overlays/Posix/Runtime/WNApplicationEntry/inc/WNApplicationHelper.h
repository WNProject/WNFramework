// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_APPLICATION_APPLICATION_HELPER_H__
#define __WN_RUNTIME_APPLICATION_APPLICATION_HELPER_H__

namespace wn {
namespace runtime {
namespace application {
namespace internal {

void test_dummy();

struct foo {
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

}  // namespace internal
}  // namespace application
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_APPLICATION_APPLICATION_HELPER_H__
