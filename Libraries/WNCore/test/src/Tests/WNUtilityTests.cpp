// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNTesting/inc/WNTestHarness.h"

template <typename _Type>
struct utility : ::testing::Test {};

typedef ::testing::Types<wn_int8, wn_uint16, wn_float32, wn_nullptr_t> utility_testing_types;

TYPED_TEST_CASE(utility, utility_testing_types);

TYPED_TEST(utility, dependent_true) {
    EXPECT_TRUE(wn::core::dependent_true<TypeParam>::value);
}

TYPED_TEST(utility, dependent_false) {
    EXPECT_FALSE(wn::core::dependent_false<TypeParam>::value);
}