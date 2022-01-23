// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNHashSet.h"
#include "executable_test/inc/WNTestHarness.h"

template <typename _Type>
struct hash_set : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    hash_set_testing_types;

TYPED_TEST_SUITE(hash_set, hash_set_testing_types);

TYPED_TEST(hash_set, construction) {
  wn::testing::allocator allocator;

  { wn::containers::hash_set<TypeParam> set1(&allocator); }

  EXPECT_EQ(allocator.allocated(), 0u);
}

TYPED_TEST(hash_set, insert) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    auto a = set.insert(23u);

    EXPECT_TRUE(a.second);
    EXPECT_EQ(TypeParam(23), *a.first);

    a = set.insert(23u);

    EXPECT_FALSE(a.second);
    EXPECT_EQ(TypeParam(23), *a.first);
  }
}

TYPED_TEST(hash_set, iterate) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }

    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

    for (auto it = set.begin(); it != set.end(); ++it) {
      found[*it] = -1;
    }

    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(found[i], -1);

      found[i] = static_cast<int>(i * 11);
    }

    for (auto it = set.cbegin(); it != set.cend(); ++it) {
      found[*it] = -1;
    }

    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(found[i], -1);

      found[i] = static_cast<int>(i * 11);
    }
  }
}

TYPED_TEST(hash_set, find) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }

    for (size_t i = 0; i < 10; ++i) {
      auto it = set.find(static_cast<TypeParam>(i));

      EXPECT_FALSE(set.end() == it);
      EXPECT_EQ(i, *it);
    }

    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(32)));
    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(10)));
  }
}

TYPED_TEST(hash_set, find_in_empty_set) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(10)));
  }
}

TYPED_TEST(hash_set, erase) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }

    {
      auto it = set.find(5);

      ASSERT_FALSE(set.end() == it);

      set.erase(it);
    }

    for (size_t i = 0; i < 10; ++i) {
      auto it = set.find(static_cast<TypeParam>(i));

      if (i == 5) {
        EXPECT_EQ(set.end(), it);
      } else {
        EXPECT_FALSE(set.end() == it);
        EXPECT_EQ(i, *it);
      }
    }

    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

    for (auto it = set.begin(); it != set.end(); ++it) {
      found[*it] = -1;
    }

    int expected[] = {-1, -1, -1, -1, -1, 55, -1, -1, -1, -1};

    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(expected[i], found[i]);
    }
  }
}

TYPED_TEST(hash_set, erase_all) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<TypeParam> set(&allocator);

    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }

    auto it = set.begin();

    while (it != set.end()) {
      it = set.erase(it);
    }

    EXPECT_EQ(0u, set.size());
    EXPECT_EQ(set.end(), set.begin());
  }
}

TEST(hash_set, initializers) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_set<std::string> my_set(&allocator, {"a"});

    EXPECT_EQ(my_set.end(), my_set.find("b"));
    EXPECT_NE(my_set.end(), my_set.find("a"));
    EXPECT_EQ(1u, my_set.size());
  }

  {
    wn::containers::hash_set<std::string> my_set(&allocator, {"a", "b", "c"});

    EXPECT_EQ(my_set.end(), my_set.find("d"));
    EXPECT_NE(my_set.end(), my_set.find("a"));
    EXPECT_NE(my_set.end(), my_set.find("b"));
    EXPECT_NE(my_set.end(), my_set.find("b"));
    EXPECT_EQ(3u, my_set.size());
  }
}
