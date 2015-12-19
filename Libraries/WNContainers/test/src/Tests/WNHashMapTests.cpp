// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNContainers/inc/WNHashMap.h"

template <typename _Type>
struct hash_map : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64>
    hash_map_testing_types;

template <typename T>
using test_map = wn::containers::hash_map<T, T, std::hash<T>, std::equal_to<T>,
                                          wn::memory::default_test_allocator>;
TYPED_TEST_CASE(hash_map, hash_map_testing_types);

TYPED_TEST(hash_map, construction) {
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
  { test_map<TypeParam> map; }
  // We should not have allocated anything if we have not done anything.s
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, construction_with_allocator) {
  wn::memory::default_test_allocator alloc;
  { test_map<TypeParam> map(&alloc); }
  EXPECT_EQ(0, alloc.allocated());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, insert) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    bool inserted = false;
    typename test_map<TypeParam>::iterator it;

    std::tie(it, inserted) = map.insert(std::make_pair(23, 32));
    EXPECT_TRUE(inserted);
    EXPECT_EQ(32, it->second);

    std::tie(it, inserted) = map.insert(std::make_pair(23, 44));
    EXPECT_FALSE(inserted);
    EXPECT_EQ(32, it->second);
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, iterate) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }
    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};
    for (auto it = map.begin(); it != map.end(); ++it) {
      EXPECT_EQ(it->second, found[it->first]);
      found[it->first] = -1;
    }
    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(found[i], -1);
      found[i] = static_cast<int>(i * 11);
    }

    for (auto it = map.cbegin(); it != map.cend(); ++it) {
      EXPECT_EQ(it->second, found[it->first]);
      found[it->first] = -1;
    }
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, find) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }
    for (size_t i = 0; i < 10; ++i) {
      typename test_map<TypeParam>::iterator it =
          map.find(static_cast<TypeParam>(i));
      EXPECT_FALSE(map.end() == it);
      EXPECT_EQ(i, it->first);
      EXPECT_EQ(i * 11, it->second);
    }

    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(32)));
    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(10)));
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, find_in_empty_map) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(10)));
  }

  EXPECT_TRUE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, erase) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }
    {
      typename test_map<TypeParam>::iterator it = map.find(5);
      ASSERT_FALSE(map.end() == it);

      map.erase(it);
    }
    for (size_t i = 0; i < 10; ++i) {
      typename test_map<TypeParam>::iterator it =
          map.find(static_cast<TypeParam>(i));
      if (i == 5) {
        EXPECT_EQ(map.end(), it);
      } else {
        EXPECT_FALSE(map.end() == it);
        EXPECT_EQ(i, it->first);
        EXPECT_EQ(i * 11, it->second);
      }
    }

    int found[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};
    for (typename test_map<TypeParam>::iterator it = map.begin();
         it != map.end(); ++it) {
      found[it->first] = -1;
    }

    int expected[] = {-1, -1, -1, -1, -1, 55, -1, -1, -1, -1};
    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(expected[i], found[i]);
    }
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TYPED_TEST(hash_map, erase_all) {
  wn::memory::default_test_allocator alloc;
  {
    test_map<TypeParam> map(&alloc);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }

    typename test_map<TypeParam>::iterator it = map.begin();
    while (it != map.end()) {
      it = map.erase(it);
    }
    EXPECT_EQ(0, map.size());
    EXPECT_EQ(map.end(), map.begin());
  }

  EXPECT_FALSE(0 == alloc.allocated());
  EXPECT_TRUE(alloc.empty());
  EXPECT_EQ(0, test_map<TypeParam>::get_default_allocator()->allocated());
}

TEST(hash_map, initializers) {
  wn::memory::default_test_allocator alloc;
  {
    wn::containers::hash_map<std::string, std::string> my_map({{"a", "1"}},
                                                              &alloc);
    EXPECT_EQ(my_map.end(), my_map.find("b"));
    EXPECT_NE(my_map.end(), my_map.find("a"));
    EXPECT_EQ(1, my_map.size());
  }
  {
    wn::containers::hash_map<std::string, std::string> my_map(
        {{"a", "1"}, {"b", "2"}, {"c", "3"}}, &alloc);
    EXPECT_EQ(my_map.end(), my_map.find("d"));
    EXPECT_NE(my_map.end(), my_map.find("a"));
    EXPECT_NE(my_map.end(), my_map.find("b"));
    EXPECT_NE(my_map.end(), my_map.find("b"));
    EXPECT_EQ(3, my_map.size());
  }
}

TEST(hash_map, square_brackets) {
  wn::memory::default_test_allocator alloc;
  {
    wn::containers::hash_map<std::string, std::string> my_map(
        {{"a", "1"}}, 0, std::hash<std::string>(), std::equal_to<std::string>(),
        &alloc);
    EXPECT_EQ("1", my_map["a"]);
    my_map["b"] = "2";
    EXPECT_EQ(2, my_map.size());
    EXPECT_EQ("2", my_map.find("b")->second);
  }
}

TYPED_TEST(hash_map, move_test) {
  wn::memory::default_test_allocator alloc;
  {
    wn::containers::hash_map<TypeParam, TypeParam> my_map(
    { {221, 1}, {201, 2}, {213, 3} }, &alloc);
    wn_size_t allocated = alloc.allocated();
    EXPECT_EQ(3, my_map.size());
    wn::containers::hash_map < TypeParam, TypeParam> new_map(std::move(my_map));
    EXPECT_EQ(0, my_map.size());
    EXPECT_EQ(3, new_map.size());
    EXPECT_EQ(allocated, alloc.allocated());
    EXPECT_EQ(1, new_map[221]);
    EXPECT_EQ(2, new_map[201]);
    EXPECT_EQ(3, new_map[213]);
  }
}

TYPED_TEST(hash_map, copy_test) {
  wn::memory::default_test_allocator alloc;
  {
    wn::containers::hash_map<TypeParam, TypeParam> my_map(
    { { 221, 1 },{ 201, 2 },{ 213, 3 } }, &alloc);
    wn_size_t allocated = alloc.allocated();
    EXPECT_EQ(3, my_map.size());
    wn::containers::hash_map < TypeParam, TypeParam> new_map(my_map);
    EXPECT_EQ(3, my_map.size());
    EXPECT_EQ(3, new_map.size());
    EXPECT_GT(alloc.allocated(), allocated);
    EXPECT_EQ(my_map[221], new_map[221]);
    EXPECT_EQ(my_map[201], new_map[201]);
    EXPECT_EQ(my_map[213], new_map[213]);
  }
}

TYPED_TEST(hash_map, cbegin) {
  wn::memory::default_test_allocator alloc;
  {
    const wn::containers::hash_map<TypeParam, TypeParam> my_map(
    { { 1, 1 },{ 2, 2 },{ 3, 3 } }, &alloc);
    EXPECT_EQ(3, my_map.size());
    typename wn::containers::hash_map<TypeParam, TypeParam>::const_iterator it =
        my_map.cbegin();
    for(;it != my_map.cend(); ++it) {
      EXPECT_EQ(it->first, it->second);
    }
  }
  EXPECT_EQ(alloc.allocated(), alloc.freed());
}
