// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/inc/stl_allocator.h"
#include "executable_test/inc/WNTestHarness.h"

#include <vector>

template <typename T>
struct stl_allocator : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    stl_allocator_testing_types;

TYPED_TEST_SUITE(stl_allocator, stl_allocator_testing_types);

TYPED_TEST(stl_allocator, default_allocator) {
  using vector_allocator =
      wn::memory::stl_allocator<TypeParam, wn::testing::allocator>;

  std::vector<TypeParam, vector_allocator> vector;

  for (TypeParam i = 0; i < 100; ++i) {
    vector.push_back(i);
  }

  vector.clear();
}

TYPED_TEST(stl_allocator, injected_allocator) {
  wn::testing::allocator allocator;

  using vector_allocator =
      wn::memory::stl_allocator<TypeParam, wn::testing::allocator>;

  {
    std::vector<TypeParam, vector_allocator> vector(&allocator);

    for (TypeParam i = 0; i < 100; ++i) {
      vector.push_back(i);
    }

    vector.clear();
  }
}
