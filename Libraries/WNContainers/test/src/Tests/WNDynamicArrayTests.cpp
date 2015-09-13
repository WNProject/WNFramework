// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNDynamicArray.h"

template <typename _Type>
struct dynamic_array : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64>
    dynamic_array_testing_types;

TYPED_TEST_CASE(dynamic_array, dynamic_array_testing_types);

TYPED_TEST(dynamic_array, range_construct) {
  TypeParam buffer[5] = {TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3),
                         TypeParam(4)};
  wn::memory::default_test_allocator alloc;
  {
    wn::containers::dynamic_array<TypeParam> arr(buffer, buffer + 5, &alloc);

    for (size_t i = 0; i < 5; ++i) {
      EXPECT_EQ(TypeParam(i), arr[i]);
      EXPECT_EQ(buffer[i], arr[i]);
    }
  }

  EXPECT_EQ(alloc.freed(), alloc.allocated());
}
