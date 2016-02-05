// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDeque.h"
#include "WNTesting/inc/WNTestHarness.h"

template <typename _Type>
struct deque : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    deque_testing_types;

TYPED_TEST_CASE(deque, deque_testing_types);

TYPED_TEST(deque, construction) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque1;

    ASSERT_TRUE(deque1.empty());
    ASSERT_EQ(deque1.size(), 0);

    wn::containers::deque<TypeParam> deque2(25, &allocator);

    ASSERT_FALSE(deque2.empty());
    ASSERT_EQ(deque2.size(), 25);

    wn::containers::deque<TypeParam> deque3(
        25, static_cast<TypeParam>(5), &allocator);

    ASSERT_FALSE(deque3.empty());
    ASSERT_EQ(deque3.size(), 25);

    for (const TypeParam& element : deque3) {
      ASSERT_EQ(element, static_cast<TypeParam>(5));
    }

    const TypeParam array1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    wn::containers::deque<TypeParam> deque4(array1, (array1 + 11), &allocator);

    ASSERT_FALSE(deque4.empty());
    ASSERT_EQ(deque4.size(), 11);

    for (size_t i = 0; i < 11; ++i) {
      ASSERT_EQ(deque4[i], static_cast<TypeParam>(i));
    }

    wn::containers::deque<TypeParam> deque5(
        {0, 1, 2, 3, 4, 5, 6, 7, 8}, &allocator);

    ASSERT_FALSE(deque5.empty());
    ASSERT_EQ(deque5.size(), 9);

    for (size_t i = 0; i < 9; ++i) {
      ASSERT_EQ(deque5[i], static_cast<TypeParam>(i));
    }
  }
}

TYPED_TEST(deque, push_back) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator);

    deque.push_back(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_back(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(2));
  }
}

TYPED_TEST(deque, push_front) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator);

    deque.push_front(static_cast<TypeParam>(1));

    const TypeParam value = static_cast<TypeParam>(2);

    deque.push_front(value);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 2);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(2));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(1));
  }
}

TYPED_TEST(deque, pop_back) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator);

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
}

TYPED_TEST(deque, pop_front) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator);

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
}

TYPED_TEST(deque, clear) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(23, 3, &allocator);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 23);

    for (const TypeParam& element : deque) {
      ASSERT_EQ(element, static_cast<TypeParam>(3));
    }

    deque.clear();

    ASSERT_TRUE(deque.empty());
    ASSERT_EQ(deque.size(), 0);
  }
}

TYPED_TEST(deque, move) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(23, 3, &allocator);

    EXPECT_FALSE(deque.empty());
    EXPECT_EQ(23, deque.size());

    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    wn::containers::deque<TypeParam> deque2(std::move(deque));
    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    EXPECT_TRUE(deque.empty());
    EXPECT_EQ(0, deque.size());

    EXPECT_FALSE(deque2.empty());
    EXPECT_EQ(23, deque2.size());

    deque2.clear();

    EXPECT_TRUE(deque2.empty());
    EXPECT_EQ(0, deque2.size());
  }
}
