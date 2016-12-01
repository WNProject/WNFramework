// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNAlgorithm.h"
#include <array>
#include "WNTesting/inc/WNTestHarness.h"

TEST(algorithm, equal) {
  const uint32_t array1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  const std::array<uint32_t, 10> array2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  const size_t array_length = 10;

  EXPECT_TRUE(wn::core::equal(
      array1, array1 + array_length, array2.cbegin(), array2.cend()));

  const uint32_t array3[] = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  EXPECT_FALSE(wn::core::equal(
      array1, array1 + array_length, array3, array3 + array_length));

  const std::array<uint32_t, 10> array4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 5};

  EXPECT_FALSE(wn::core::equal(
      array1, array1 + array_length, array4.cbegin(), array4.cend()));

  const uint32_t array5[] = {1, 2, 3, 4, 4, 6, 7, 8, 9, 0};

  EXPECT_FALSE(wn::core::equal(
      array1, array1 + array_length, array5, array5 + array_length));
}

TEST(algorithm, equal_if) {
  const uint32_t array1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  const std::array<uint32_t, 10> array2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  const size_t array_length = 10;
  const auto predicate = [](
      const uint32_t _element1, const uint32_t _element2) -> bool {
    const int32_t difference = _element1 - _element2;

    return (difference == 0);
  };

  EXPECT_TRUE(wn::core::equal_if(array1, array1 + array_length, array2.cbegin(),
      array2.cend(), predicate));

  const uint32_t array3[] = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  EXPECT_FALSE(wn::core::equal_if(
      array1, array1 + array_length, array3, array3 + array_length, predicate));

  const std::array<uint32_t, 10> array4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 5};

  EXPECT_FALSE(wn::core::equal_if(array1, array1 + array_length,
      array4.cbegin(), array4.cend(), predicate));

  const uint32_t array5[] = {1, 2, 3, 4, 4, 6, 7, 8, 9, 0};

  EXPECT_FALSE(wn::core::equal_if(
      array1, array1 + array_length, array5, array5 + array_length, predicate));
}