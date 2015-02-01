// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/test/inc/Common.h"

TEST(algorithm, equal) {
    const wn_uint32 array1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_uint32 array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_size_t array_length = 10;

    ASSERT_TRUE(wn::equal(array1, array1 + array_length, array2, array2 + array_length));

    const wn_uint32 array3[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    ASSERT_FALSE(wn::equal(array1, array1 + array_length, array3, array3 + array_length));

    const wn_uint32 array4[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 5 };

    ASSERT_FALSE(wn::equal(array1, array1 + array_length, array4, array4 + array_length));

    const wn_uint32 array5[] = { 1, 2, 3, 4, 4, 6, 7, 8, 9, 0 };

    ASSERT_FALSE(wn::equal(array1, array1 + array_length, array5, array5 + array_length));
}

TEST(algorithm, equal_if) {
    const wn_uint32 array1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_uint32 array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    const wn_size_t array_length = 10;
    const auto predicate = [](const wn_uint32 _element1, const wn_uint32 _element2) -> wn_bool {
        const wn_int32 difference = _element1 - _element2;

        return(difference == 0);
    };

    ASSERT_TRUE(wn::equal_if(array1, array1 + array_length, array2, array2 + array_length, predicate));

    const wn_uint32 array3[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    ASSERT_FALSE(wn::equal_if(array1, array1 + array_length, array3, array3 + array_length, predicate));

    const wn_uint32 array4[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 5 };

    ASSERT_FALSE(wn::equal_if(array1, array1 + array_length, array4, array4 + array_length, predicate));

    const wn_uint32 array5[] = { 1, 2, 3, 4, 4, 6, 7, 8, 9, 0 };

    ASSERT_FALSE(wn::equal_if(array1, array1 + array_length, array5, array5 + array_length, predicate));
}