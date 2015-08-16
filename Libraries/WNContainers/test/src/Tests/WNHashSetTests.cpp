// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNHashSet.h"

template <typename _Type>
struct hash_set : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64>
    hash_set_testing_types;

template <typename T>
using test_set = wn::containers::hash_set<T, std::hash<T>, std::equal_to<T>,
                                          wn::memory::default_test_allocator>;
TYPED_TEST_CASE(hash_set, hash_set_testing_types);

TYPED_TEST(hash_set, construction) {
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
  { test_set<TypeParam> set; }
  // We should not have allocated anything if we have not done anything.s
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, construction_with_allocator) {
  wn::memory::default_test_allocator alloc;
  { test_set<TypeParam> set(&alloc); }
  EXPECT_EQ(0, alloc.allocated());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, insert) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
    bool inserted = false;
    typename test_set<TypeParam>::iterator it;

    std::tie(it, inserted) = set.insert(23);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(23, *it);

    std::tie(it, inserted) = set.insert(23);
    EXPECT_FALSE(inserted);
    EXPECT_EQ(23, *it);
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, iterate) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
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

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, find) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }
    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};
    for (size_t i = 0; i < 10; ++i) {
      typename test_set<TypeParam>::iterator it =
          set.find(static_cast<TypeParam>(i));
      EXPECT_FALSE(set.end() == it);
      EXPECT_EQ(i, *it);
    }
    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(32)));
    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(10)));
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, find_in_empty_set) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
    EXPECT_EQ(set.end(), set.find(static_cast<TypeParam>(10)));
  }

  EXPECT_TRUE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, erase) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }
    {
      typename test_set<TypeParam>::iterator it = set.find(5);
      ASSERT_FALSE(set.end() == it);

      set.erase(it);
    }
    for (size_t i = 0; i < 10; ++i) {
      typename test_set<TypeParam>::iterator it =
          set.find(static_cast<TypeParam>(i));
      if (i == 5) {
        EXPECT_EQ(set.end(), it);
      } else {
        EXPECT_FALSE(set.end() == it);
        EXPECT_EQ(i, *it);
      }
    }

    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};
    for (typename test_set<TypeParam>::iterator it = set.begin();
         it != set.end(); ++it) {
      found[*it] = -1;
    }

    int expected[] = {-1, -1, -1, -1, -1, 55, -1, -1, -1, -1};
    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(expected[i], found[i]);
    }
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}

TYPED_TEST(hash_set, erase_all) {
  wn::memory::default_test_allocator alloc;
  {
    test_set<TypeParam> set(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      set.insert(static_cast<TypeParam>(i));
    }

    typename test_set<TypeParam>::iterator it = set.begin();
    while (it != set.end()) {
      it = set.erase(it);
    }
    EXPECT_EQ(0, set.size());
    EXPECT_EQ(set.end(), set.begin());
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_set<TypeParam>::s_default_allocator.allocated());
}
