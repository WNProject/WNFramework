// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/inc/utilities.h"
#include "executable_test/inc/WNTestHarness.h"

#include <functional>

WN_ENABLE_TYPED_TEST(utilities);

struct dummy_type final {};

using utilities_testing_types =
    ::testing::Types<int8_t, uint16_t, int32_t, uint64_t, void, size_t, void*,
        nullptr_t, std::function<int()>, const dummy_type>;

TYPED_TEST_SUITE(utilities, utilities_testing_types);

TYPED_TEST(utilities, type_id) {
  const size_t value1 = wn::core::type_id<TypeParam>::value();
  const size_t value2 = wn::core::type_id<TypeParam>::value();

  EXPECT_NE(value1, 0u);
  EXPECT_NE(value2, 0u);
  EXPECT_EQ(value1, value2);

  const size_t value3 = wn::core::type_id<dummy_type>::value();

  EXPECT_NE(value3, 0u);
  EXPECT_NE(value1, value3);

  const size_t value4 = wn::core::type_id<volatile TypeParam>::value();
  const size_t value5 = wn::core::type_id<TypeParam>::value();

  EXPECT_NE(value4, value5);
}
