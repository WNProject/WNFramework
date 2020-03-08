// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDeque.h"
#include "WNMemory/inc/unique_ptr.h"
#include "executable_test/inc/WNTestHarness.h"

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
    ASSERT_EQ(deque1.size(), 0u);

    wn::containers::deque<TypeParam> deque2(&allocator, 25);

    ASSERT_FALSE(deque2.empty());
    ASSERT_EQ(deque2.size(), 25u);

    wn::containers::deque<TypeParam> deque3(
        &allocator, 25, static_cast<TypeParam>(5));

    ASSERT_FALSE(deque3.empty());
    ASSERT_EQ(deque3.size(), 25u);

    for (const TypeParam& element : deque3) {
      ASSERT_EQ(element, static_cast<TypeParam>(5));
    }

    const TypeParam array1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    wn::containers::deque<TypeParam> deque4(&allocator, array1, (array1 + 11));

    ASSERT_FALSE(deque4.empty());
    ASSERT_EQ(deque4.size(), 11u);

    for (size_t i = 0; i < 11; ++i) {
      ASSERT_EQ(deque4[i], static_cast<TypeParam>(i));
    }

    wn::containers::deque<TypeParam> deque5(
        &allocator, {0, 1, 2, 3, 4, 5, 6, 7, 8});

    ASSERT_FALSE(deque5.empty());
    ASSERT_EQ(deque5.size(), 9u);

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
    ASSERT_EQ(deque.size(), 2u);
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
    ASSERT_EQ(deque.size(), 2u);
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
    ASSERT_EQ(deque.size(), 2u);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(2));
    ASSERT_EQ(deque.back(), static_cast<TypeParam>(2));

    deque.pop_back();

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 1u);
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
    ASSERT_EQ(deque.size(), 2u);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(2));
    ASSERT_EQ(deque[1], static_cast<TypeParam>(1));
    ASSERT_EQ(deque.front(), static_cast<TypeParam>(2));

    deque.pop_front();

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 1u);
    ASSERT_EQ(deque[0], static_cast<TypeParam>(1));
    ASSERT_EQ(deque.front(), static_cast<TypeParam>(1));
  }
}

TYPED_TEST(deque, clear) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator, 23, 3);

    ASSERT_FALSE(deque.empty());
    ASSERT_EQ(deque.size(), 23u);

    for (const TypeParam& element : deque) {
      ASSERT_EQ(element, static_cast<TypeParam>(3));
    }

    deque.clear();

    ASSERT_TRUE(deque.empty());
    ASSERT_EQ(deque.size(), 0u);
  }
}

TYPED_TEST(deque, move) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator, 23, 3);

    EXPECT_FALSE(deque.empty());
    EXPECT_EQ(23u, deque.size());

    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    wn::containers::deque<TypeParam> deque2(std::move(deque));
    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    EXPECT_TRUE(deque.empty());
    EXPECT_EQ(0u, deque.size());

    EXPECT_FALSE(deque2.empty());
    EXPECT_EQ(23u, deque2.size());

    deque2.clear();

    EXPECT_TRUE(deque2.empty());
    EXPECT_EQ(0u, deque2.size());
  }
}

TYPED_TEST(deque, clear_and_add) {
  wn::testing::allocator allocator;

  {
    wn::containers::deque<TypeParam> deque(&allocator, 23, 3);

    EXPECT_FALSE(deque.empty());
    EXPECT_EQ(23u, deque.size());

    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    deque.clear();
    for (size_t i = 0; i < 23; ++i) {
      deque.push_back(static_cast<TypeParam>(3));
    }
    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }

    deque.clear();
    for (size_t i = 0; i < 23; ++i) {
      deque.push_back(static_cast<TypeParam>(3));
    }
    for (const TypeParam& element : deque) {
      EXPECT_EQ(element, static_cast<TypeParam>(3));
    }
  }
}

struct MyStruct {
  MyStruct(wn::memory::allocator* _allocator)
    : m_data(wn::memory::make_unique<uint32_t>(_allocator, 3)) {}

  wn::memory::unique_ptr<uint32_t> m_data;
};

TEST(deque, random_ops) {
  wn::testing::allocator allocator;
  uint32_t seed = 0;
  srand(seed);
  wn::containers::deque<MyStruct> deque(&allocator);
  for (size_t i = 0; i < 1000; ++i) {
    deque.emplace_back(&allocator);
  }
  size_t expected_size = 1000;
  for (size_t i = 0; i < 1000; ++i) {
    switch (rand() % 4) {
      case 0:
        deque.emplace_front(&allocator);
        expected_size++;
        break;
      case 1:
        deque.emplace_back(&allocator);
        expected_size++;
        break;
      case 2:
        deque.pop_front();
        expected_size--;
        break;
      case 3:
        deque.pop_back();
        expected_size--;
        break;
    }
  }

  EXPECT_EQ(expected_size, deque.size()) << "Seed: " << seed;
}
