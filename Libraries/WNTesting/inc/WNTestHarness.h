// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_TESTING_TEST_HARNESS_H__
#define __WN_TESTING_TEST_HARNESS_H__

#include "WNCore/inc/WNTypes.h"

#define GTEST_HAS_RTTI 0

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4702)  // disable unreachable code warning.
                                 // Work-around for xtree issue when exceptions
                                 // disabled
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

#ifdef _WN_ANDROID
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#else
#ifdef _WN_DEBUG
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)                 \
  EXPECT_DEATH(expression, regexp)
#else
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#endif
#endif

namespace wn {
namespace testing {

#if defined _WN_ANDROID || defined _WN_LINUX
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
#endif  //_WN_ANDROID || _WN_LINUX

void init_test_framework();

}  // namespace testing
}  // namespace wn

#define WN_ENABLE_TYPED_TEST(_name)                                            \
  template <typename T>                                                        \
  struct _name : ::testing::Test {}

#endif  // __WN_TESTING_TEST_HARNESS_H__