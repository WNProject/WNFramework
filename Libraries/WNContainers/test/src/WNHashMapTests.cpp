// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNHashMap.h"
#include "WNTesting/inc/WNTestHarness.h"

template <typename _Type>
struct hash_map : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    hash_map_testing_types;

TYPED_TEST_CASE(hash_map, hash_map_testing_types);

TYPED_TEST(hash_map, construction) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_map<TypeParam, TypeParam> map1;
    const wn::containers::hash_map<TypeParam, TypeParam> map2(&allocator);
  }

  EXPECT_EQ(allocator.allocated(), 0);
}

TYPED_TEST(hash_map, insert) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
    auto it = map.insert(wn::core::make_pair(TypeParam(23), TypeParam(32)));

    EXPECT_TRUE(it.second);
    EXPECT_EQ(32, it.first->second);

    it = map.insert(wn::core::make_pair(TypeParam(23), TypeParam(44)));

    EXPECT_FALSE(it.second);
    EXPECT_EQ(32, it.first->second);
  }
}

TYPED_TEST(hash_map, iterate) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
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
}

TYPED_TEST(hash_map, find) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }
    for (size_t i = 0; i < 10; ++i) {
      typename wn::containers::hash_map<TypeParam, TypeParam>::iterator it =
          map.find(static_cast<TypeParam>(i));
      EXPECT_FALSE(map.end() == it);
      EXPECT_EQ(i, it->first);
      EXPECT_EQ(i * 11, it->second);
    }

    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(32)));
    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(10)));
  }
}

TYPED_TEST(hash_map, find_in_empty_map) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
    EXPECT_EQ(map.end(), map.find(static_cast<TypeParam>(10)));
  }
}

TYPED_TEST(hash_map, erase) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }
    {
      typename wn::containers::hash_map<TypeParam, TypeParam>::iterator it =
          map.find(5);
      ASSERT_FALSE(map.end() == it);

      map.erase(it);
    }
    for (size_t i = 0; i < 10; ++i) {
      typename wn::containers::hash_map<TypeParam, TypeParam>::iterator it =
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
    for (typename wn::containers::hash_map<TypeParam, TypeParam>::iterator it =
             map.begin();
         it != map.end(); ++it) {
      found[it->first] = -1;
    }

    int expected[] = {-1, -1, -1, -1, -1, 55, -1, -1, -1, -1};
    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(expected[i], found[i]);
    }
  }
}

TYPED_TEST(hash_map, erase_all) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<TypeParam, TypeParam> map(&allocator);
    for (size_t i = 0; i < 10; ++i) {
      map.insert(static_cast<TypeParam>(i), static_cast<TypeParam>(11 * i));
    }

    typename wn::containers::hash_map<TypeParam, TypeParam>::iterator it =
        map.begin();
    while (it != map.end()) {
      it = map.erase(it);
    }
    EXPECT_EQ(0, map.size());
    EXPECT_EQ(map.end(), map.begin());
  }
}

TEST(hash_map, initializers) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<std::string, std::string> my_map(
        {{"a", "1"}}, &allocator);
    EXPECT_EQ(my_map.end(), my_map.find("b"));
    EXPECT_NE(my_map.end(), my_map.find("a"));
    EXPECT_EQ(1, my_map.size());
  }
  {
    wn::containers::hash_map<std::string, std::string> my_map(
        {{"a", "1"}, {"b", "2"}, {"c", "3"}}, &allocator);
    EXPECT_EQ(my_map.end(), my_map.find("d"));
    EXPECT_NE(my_map.end(), my_map.find("a"));
    EXPECT_NE(my_map.end(), my_map.find("b"));
    EXPECT_NE(my_map.end(), my_map.find("b"));
    EXPECT_EQ(3, my_map.size());
  }
}

TEST(hash_map, square_brackets) {
  wn::testing::allocator allocator;
  {
    wn::containers::hash_map<std::string, std::string> my_map({{"a", "1"}}, 0,
        std::hash<std::string>(), std::equal_to<std::string>(), &allocator);
    EXPECT_EQ("1", my_map["a"]);
    my_map["b"] = "2";
    EXPECT_EQ(2, my_map.size());
    EXPECT_EQ("2", my_map.find("b")->second);
  }
}

TYPED_TEST(hash_map, move_test) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_map<TypeParam, TypeParam> my_map(
        {{TypeParam(221), TypeParam(1)}, {TypeParam(201), TypeParam(2)},
            {TypeParam(213), TypeParam(3)}},
        &allocator);
    size_t allocated = allocator.allocated();

    EXPECT_EQ(3, my_map.size());

    wn::containers::hash_map<TypeParam, TypeParam> new_map(
        wn::core::move(my_map));

    EXPECT_EQ(0, my_map.size());
    EXPECT_EQ(3, new_map.size());
    EXPECT_EQ(allocated, allocator.allocated());
    EXPECT_EQ(1, new_map[221]);
    EXPECT_EQ(2, new_map[201]);
    EXPECT_EQ(3, new_map[213]);
  }
}

TYPED_TEST(hash_map, copy_test) {
  wn::testing::allocator allocator;

  {
    wn::containers::hash_map<TypeParam, TypeParam> my_map(
        {{TypeParam(221), TypeParam(1)}, {TypeParam(201), TypeParam(2)},
            {TypeParam(213), TypeParam(3)}},
        &allocator);
    size_t allocated = allocator.allocated();
    EXPECT_EQ(3, my_map.size());
    wn::containers::hash_map<TypeParam, TypeParam> new_map(my_map);
    EXPECT_EQ(3, my_map.size());
    EXPECT_EQ(3, new_map.size());
    EXPECT_GT(allocator.allocated(), allocated);
    EXPECT_EQ(my_map[221], new_map[221]);
    EXPECT_EQ(my_map[201], new_map[201]);
    EXPECT_EQ(my_map[213], new_map[213]);
  }
}

TYPED_TEST(hash_map, cbegin) {
  wn::testing::allocator allocator;

  {
    const wn::containers::hash_map<TypeParam, TypeParam> my_map(
        {{TypeParam(1), TypeParam(1)}, {TypeParam(2), TypeParam(2)},
            {TypeParam(3), TypeParam(3)}},
        &allocator);
    EXPECT_EQ(3, my_map.size());
    typename wn::containers::hash_map<TypeParam, TypeParam>::const_iterator it =
        my_map.cbegin();
    for (; it != my_map.cend(); ++it) {
      EXPECT_EQ(it->first, it->second);
    }
  }
}
