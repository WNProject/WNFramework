// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "executable_test/inc/WNTestHarness.h"

template <typename _Type>
struct dynamic_array : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    dynamic_array_testing_types;

TYPED_TEST_SUITE(dynamic_array, dynamic_array_testing_types);

TYPED_TEST(dynamic_array, range_construct) {
  TypeParam buffer[5] = {
      TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4)};
  wn::testing::allocator allocator;

  {
    wn::containers::dynamic_array<TypeParam> arr(
        &allocator, buffer, buffer + 5);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }
  }
}

TYPED_TEST(dynamic_array, resize) {
  TypeParam buffer[5] = {
      TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4)};
  wn::testing::allocator allocator;

  {
    wn::containers::dynamic_array<TypeParam> arr(
        &allocator, buffer, buffer + 5);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }

    arr.resize(10);

    EXPECT_EQ(arr.size(), 10u);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }

    arr.resize(5);

    EXPECT_EQ(arr.size(), 5u);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }

    arr.resize(10, 6);

    EXPECT_EQ(arr.size(), 10u);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }

    for (size_t i = 5; i < 10; ++i) {
      EXPECT_EQ(TypeParam(6), arr[i]);
    }
  }
}
