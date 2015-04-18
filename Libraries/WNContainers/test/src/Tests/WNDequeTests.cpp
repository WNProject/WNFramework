// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"

template <typename _Type>
struct deque : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64> deque_testing_types;

TYPED_TEST_CASE(deque, deque_testing_types);

TYPED_TEST(deque, construction) {
    wn::containers::deque<TypeParam> deque1;

    ASSERT_TRUE(deque1.empty());
    ASSERT_EQ(deque1.size(), 0);

    wn::containers::deque<TypeParam> deque2(25);

    ASSERT_FALSE(deque2.empty());
    ASSERT_EQ(deque2.size(), 25);

    wn::containers::deque<TypeParam> deque3(25, static_cast<TypeParam>(5));

    ASSERT_FALSE(deque3.empty());
    ASSERT_EQ(deque3.size(), 25);

    for (const TypeParam& element : deque3) {
        ASSERT_EQ(element, static_cast<TypeParam>(5));
    }

    const TypeParam array1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    wn::containers::deque<TypeParam> deque4(array1, (array1 + 11));

    ASSERT_FALSE(deque4.empty());
    ASSERT_EQ(deque4.size(), 11);

    for (wn_size_t i = 0; i < 11; ++i) {
        ASSERT_EQ(deque4[i], static_cast<TypeParam>(i));
    }

    wn::containers::deque<TypeParam> deque5 { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    ASSERT_FALSE(deque5.empty());
    ASSERT_EQ(deque5.size(), 9);

    for (wn_size_t i = 0; i < 9; ++i) {
        ASSERT_EQ(deque5[i], static_cast<TypeParam>(i));
    }
}

TYPED_TEST(deque, insert) {

}

TYPED_TEST(deque, emplace) {

}

TYPED_TEST(deque, erase) {

}

TYPED_TEST(deque, push_back) {
    wn::containers::deque<TypeParam> deque;

    deque.push_back(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_back(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(2));
}

TYPED_TEST(deque, push_front) {
    wn::containers::deque<TypeParam> deque;

    deque.push_front(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_front(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(2));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(1));
}

TYPED_TEST(deque, pop_back) {
    wn::containers::deque<TypeParam> deque;

    deque.push_back(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_back(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(2));
    ASSERT_EQ(deque.back(), static_cast<TypeParam>(2));

    deque.pop_back();

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 1);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque.back(), static_cast<TypeParam>(1));
}

TYPED_TEST(deque, pop_front) {
    wn::containers::deque<TypeParam> deque;

    deque.push_front(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_front(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(2));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(1));
    ASSERT_EQ(deque.front(), static_cast<TypeParam>(2));

    deque.pop_front();

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 1);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque.front(), static_cast<TypeParam>(1));
}

TYPED_TEST(deque, clear) {
    wn::containers::deque<TypeParam> deque(23, 3);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 23);

    for (const TypeParam& element : deque) {
        ASSERT_EQ(element, static_cast<TypeParam>(3));
    }

    deque.clear();

    ASSERT_TRUE(deque.empty());
    ASSERT_EQ(deque.size(), 0);
}