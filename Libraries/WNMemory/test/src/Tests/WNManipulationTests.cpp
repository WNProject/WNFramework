// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"

template <typename _Type>
struct manipulation : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64> manipulation_testing_types;

TYPED_TEST_CASE(manipulation, manipulation_testing_types);

TYPED_TEST(manipulation, memzero) {
    TypeParam value = static_cast<TypeParam>(1);
    TypeParam values[4] = { static_cast<TypeParam>(1),
                            static_cast<TypeParam>(2),
                            static_cast<TypeParam>(3),
                            static_cast<TypeParam>(4) };

    wn::memory::memzero(&value, sizeof(TypeParam));
    wn::memory::memzero(values, sizeof(TypeParam) * 4);

    ASSERT_EQ(value, static_cast<TypeParam>(0));
    ASSERT_EQ(values[0], static_cast<TypeParam>(0));
    ASSERT_EQ(values[1], static_cast<TypeParam>(0));
    ASSERT_EQ(values[2], static_cast<TypeParam>(0));
    ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(manipulation, memset) {
    TypeParam value = static_cast<TypeParam>(1);
    TypeParam values[4] = { static_cast<TypeParam>(1),
                            static_cast<TypeParam>(2),
                            static_cast<TypeParam>(3),
                            static_cast<TypeParam>(4) };

    wn::memory::memset(&value, 0, sizeof(TypeParam));
    wn::memory::memset(values, 0, sizeof(TypeParam) * 4);

    ASSERT_EQ(value, static_cast<TypeParam>(0));
    ASSERT_EQ(values[0], static_cast<TypeParam>(0));
    ASSERT_EQ(values[1], static_cast<TypeParam>(0));
    ASSERT_EQ(values[2], static_cast<TypeParam>(0));
    ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(manipulation, memcpy) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn::memory::memcpy(&value1, &value2, sizeof(TypeParam));
    wn::memory::memcpy(values1, values2, sizeof(TypeParam) * 4);

    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(values1[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values1[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values1[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values1[3], static_cast<TypeParam>(8));
    ASSERT_EQ(values2[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values2[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values2[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values2[3], static_cast<TypeParam>(8));
}

TYPED_TEST(manipulation, memmove) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn::memory::memmove(&value1, &value2, sizeof(TypeParam));
    wn::memory::memmove(values1, values2, sizeof(TypeParam) * 4);

    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(values1[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values1[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values1[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values1[3], static_cast<TypeParam>(8));
    ASSERT_EQ(values2[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values2[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values2[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values2[3], static_cast<TypeParam>(8));
}

TYPED_TEST(manipulation, memcmp) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn_int32 compare1 = wn::memory::memcmp(&value1, &value2, sizeof(TypeParam));
    wn_int32 compare2 = wn::memory::memcmp(values1, values2, sizeof(TypeParam) * 4);

    ASSERT_NE(compare1, 0);
    ASSERT_NE(compare2, 0);

    value2 = static_cast<TypeParam>(1);
    values2[0] = static_cast<TypeParam>(1);
    values2[1] = static_cast<TypeParam>(2);
    values2[2] = static_cast<TypeParam>(3);
    values2[3] = static_cast<TypeParam>(4);

    compare1 = wn::memory::memcmp(&value1, &value2, sizeof(TypeParam));
    compare2 = wn::memory::memcmp(values1, values2, sizeof(TypeParam) * 4);

    ASSERT_EQ(compare1, 0);
    ASSERT_EQ(compare2, 0);
}

TYPED_TEST(manipulation, memory_zero) {
    TypeParam value = static_cast<TypeParam>(1);
    TypeParam values[4] = { static_cast<TypeParam>(1),
                            static_cast<TypeParam>(2),
                            static_cast<TypeParam>(3),
                            static_cast<TypeParam>(4) };

    wn::memory::memory_zero(&value);
    wn::memory::memory_zero(values, 4);
    wn::memory::memory_zero(static_cast<wn_void*>(values), sizeof(TypeParam) * 4);

    ASSERT_EQ(value, static_cast<TypeParam>(0));
    ASSERT_EQ(values[0], static_cast<TypeParam>(0));
    ASSERT_EQ(values[1], static_cast<TypeParam>(0));
    ASSERT_EQ(values[2], static_cast<TypeParam>(0));
    ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(manipulation, memory_set) {
    TypeParam value = static_cast<TypeParam>(1);
    TypeParam values[4] = { static_cast<TypeParam>(1),
                            static_cast<TypeParam>(2),
                            static_cast<TypeParam>(3),
                            static_cast<TypeParam>(4) };

    wn::memory::memory_set(&value, 0);
    wn::memory::memory_set(values, 0, 4);
    wn::memory::memory_set(static_cast<wn_void*>(values), 0, sizeof(TypeParam) * 4);

    ASSERT_EQ(value, static_cast<TypeParam>(0));
    ASSERT_EQ(values[0], static_cast<TypeParam>(0));
    ASSERT_EQ(values[1], static_cast<TypeParam>(0));
    ASSERT_EQ(values[2], static_cast<TypeParam>(0));
    ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(manipulation, memory_copy) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn::memory::memory_copy(&value1, &value2);
    wn::memory::memory_copy(values1, values2, 4);
    wn::memory::memory_copy(static_cast<wn_void*>(values1), static_cast<wn_void*>(values2), sizeof(TypeParam) * 4);

    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(values1[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values1[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values1[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values1[3], static_cast<TypeParam>(8));
    ASSERT_EQ(values2[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values2[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values2[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values2[3], static_cast<TypeParam>(8));
}

TYPED_TEST(manipulation, memory_move) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn::memory::memory_move(&value1, &value2);
    wn::memory::memory_move(values1, values2, 4);
    wn::memory::memory_move(static_cast<wn_void*>(values1), static_cast<wn_void*>(values2), sizeof(TypeParam) * 4);

    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(values1[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values1[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values1[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values1[3], static_cast<TypeParam>(8));
    ASSERT_EQ(values2[0], static_cast<TypeParam>(2));
    ASSERT_EQ(values2[1], static_cast<TypeParam>(4));
    ASSERT_EQ(values2[2], static_cast<TypeParam>(6));
    ASSERT_EQ(values2[3], static_cast<TypeParam>(8));
}

TYPED_TEST(manipulation, memory_compare) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(2);
    TypeParam values1[4] = { static_cast<TypeParam>(1),
                             static_cast<TypeParam>(2),
                             static_cast<TypeParam>(3),
                             static_cast<TypeParam>(4) };
    TypeParam values2[4] = { static_cast<TypeParam>(2),
                             static_cast<TypeParam>(4),
                             static_cast<TypeParam>(6),
                             static_cast<TypeParam>(8) };

    wn_int32 compare1 = wn::memory::memory_compare(&value1, &value2);
    wn_int32 compare2 = wn::memory::memory_compare(values1, values2, 4);

    ASSERT_NE(compare1, 0);
    ASSERT_NE(compare2, 0);

    value2 = static_cast<TypeParam>(1);
    values2[0] = static_cast<TypeParam>(1);
    values2[1] = static_cast<TypeParam>(2);
    values2[2] = static_cast<TypeParam>(3);
    values2[3] = static_cast<TypeParam>(4);

    compare1 = wn::memory::memory_compare(&value1, &value2);
    compare2 = wn::memory::memory_compare(values1, values2, 4);

    ASSERT_EQ(compare1, 0);
    ASSERT_EQ(compare2, 0);
}