// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/test/inc/Common.h"

#include <array>

TEST(algorithm, equal) {
    const wn_uint32 array1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const std::array<wn_uint32, 10> array2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_size_t array_length = 10;

    EXPECT_TRUE(wn::core::equal(array1, array1 + array_length, array2.cbegin(), array2.cend()));

    const wn_uint32 array3[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    EXPECT_FALSE(wn::core::equal(array1, array1 + array_length, array3, array3 + array_length));

    const std::array<wn_uint32, 10> array4 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 5 };

    EXPECT_FALSE(wn::core::equal(array1, array1 + array_length, array4.cbegin(), array4.cend()));

    const wn_uint32 array5[] = { 1, 2, 3, 4, 4, 6, 7, 8, 9, 0 };

    EXPECT_FALSE(wn::core::equal(array1, array1 + array_length, array5, array5 + array_length));
}

TEST(algorithm, equal_if) {
    const wn_uint32 array1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const std::array<wn_uint32, 10> array2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_size_t array_length = 10;
    const auto predicate = [](const wn_uint32 _element1, const wn_uint32 _element2) -> wn_bool {
        const wn_int32 difference = _element1 - _element2;

        return(difference == 0);
    };

    EXPECT_TRUE(wn::core::equal_if(array1, array1 + array_length, array2.cbegin(), array2.cend(), predicate));

    const wn_uint32 array3[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    EXPECT_FALSE(wn::core::equal_if(array1, array1 + array_length, array3, array3 + array_length, predicate));

    const std::array<wn_uint32, 10> array4 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 5 };

    EXPECT_FALSE(wn::core::equal_if(array1, array1 + array_length, array4.cbegin(), array4.cend(), predicate));

    const wn_uint32 array5[] = { 1, 2, 3, 4, 4, 6, 7, 8, 9, 0 };

    EXPECT_FALSE(wn::core::equal_if(array1, array1 + array_length, array5, array5 + array_length, predicate));
}