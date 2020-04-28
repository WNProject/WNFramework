// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNList.h"
#include "executable_test/inc/WNTestHarness.h"

template <typename _Type>
struct list : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    list_testing_types;

TYPED_TEST_SUITE(list, list_testing_types);

TYPED_TEST(list, creation) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
}

TYPED_TEST(list, insert_one) {
  wn::memory::basic_allocator alloc;
  {
    wn::containers::list<TypeParam> l(&alloc);
    l.insert(l.begin(), 1);
    EXPECT_FALSE(l.begin() == l.end());
    EXPECT_EQ(static_cast<TypeParam>(1), *l.begin());
  }
  {
    wn::containers::list<TypeParam> l(&alloc);
    l.insert(l.cbegin(), 1);
    EXPECT_FALSE(l.cbegin() == l.cend());
    EXPECT_EQ(static_cast<TypeParam>(1), *l.cbegin());
  }
}

TYPED_TEST(list, insert_two) {
  wn::memory::basic_allocator alloc;
  {
    wn::containers::list<TypeParam> l(&alloc);
    typename wn::containers::list<TypeParam>::iterator it =
        l.insert(l.begin(), 2);
    l.insert(++it, 9);
    EXPECT_FALSE(l.begin() == l.end());
    EXPECT_EQ(static_cast<TypeParam>(2), *l.begin());
    EXPECT_EQ(static_cast<TypeParam>(9), *(++l.begin()));
  }
  {
    wn::containers::list<TypeParam> l(&alloc);
    typename wn::containers::list<TypeParam>::const_iterator it =
        l.insert(l.cbegin(), 2);
    l.insert(++it, 9);
    EXPECT_FALSE(l.cbegin() == l.cend());
    EXPECT_EQ(static_cast<TypeParam>(2), *l.cbegin());
    EXPECT_EQ(static_cast<TypeParam>(9), *(++l.cbegin()));
  }
}

TYPED_TEST(list, erase_first) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(static_cast<TypeParam>(8), *l.begin());
  EXPECT_EQ(static_cast<TypeParam>(9), *(++l.begin()));
  EXPECT_EQ(static_cast<TypeParam>(10), *(++(++l.begin())));

  EXPECT_EQ(static_cast<TypeParam>(9), *l.erase(l.begin()));
  EXPECT_EQ(static_cast<TypeParam>(9), *l.begin());
  EXPECT_EQ(static_cast<TypeParam>(10), *(++l.begin()));

  EXPECT_EQ(static_cast<TypeParam>(10), *l.erase(l.cbegin()));
  EXPECT_EQ(static_cast<TypeParam>(10), *l.cbegin());
  EXPECT_EQ(l.cend(), ++l.cbegin());
}

TYPED_TEST(list, erase_middle) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(static_cast<TypeParam>(8), *l.begin());
  EXPECT_EQ(static_cast<TypeParam>(9), *(++l.begin()));
  EXPECT_EQ(static_cast<TypeParam>(10), *(++(++l.begin())));

  l.erase(++l.begin());
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(10));

  l.insert(l.cbegin(), 7);
  EXPECT_EQ(static_cast<TypeParam>(10), *l.erase(++l.cbegin()));
  EXPECT_EQ(static_cast<TypeParam>(7), *l.cbegin());
  EXPECT_EQ(static_cast<TypeParam>(10), *(++l.cbegin()));
}

TYPED_TEST(list, erase_last) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(9));
  EXPECT_EQ(*(++(++l.begin())), static_cast<TypeParam>(10));

  EXPECT_EQ(l.end(), l.erase(++(++l.begin())));
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(9));

  EXPECT_EQ(l.end(), l.erase(++l.cbegin()));
  EXPECT_EQ(static_cast<TypeParam>(8), *(l.cbegin()));
}

TYPED_TEST(list, remove_from_end) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(9));
  EXPECT_EQ(*(++(++l.begin())), static_cast<TypeParam>(10));

  EXPECT_EQ(l.end(), l.erase(--l.end()));
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(9));
}

TYPED_TEST(list, clear) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  l.insert(l.begin(), 1);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(1));
  l.clear();
  EXPECT_EQ(l.begin(), l.end());
  l.insert(l.begin(), 1);
  l.insert(l.begin(), 2);
  l.insert(l.begin(), 3);
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(3));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(2));
  EXPECT_EQ(*(++(++l.begin())), static_cast<TypeParam>(1));
  l.clear();
  EXPECT_EQ(l.begin(), l.end());
}

TYPED_TEST(list, erase_and_add) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(static_cast<TypeParam>(8), *l.begin());
  EXPECT_EQ(static_cast<TypeParam>(9), *(++l.begin()));
  EXPECT_EQ(static_cast<TypeParam>(10), *(++(++l.begin())));

  EXPECT_EQ(static_cast<TypeParam>(10), *l.erase(++l.begin()));
  EXPECT_EQ(static_cast<TypeParam>(8), *l.begin());
  EXPECT_EQ(static_cast<TypeParam>(10), *(++l.begin()));

  l.insert(l.begin(), static_cast<TypeParam>(1));
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(1));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++(++l.begin())), static_cast<TypeParam>(10));

  l.insert(++(++(++l.begin())), static_cast<TypeParam>(42));
  EXPECT_EQ(*(--l.end()), static_cast<TypeParam>(42));
}

TYPED_TEST(list, erase_all_and_add) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  typename wn::containers::list<TypeParam>::iterator it =
      l.insert(l.begin(), 8);
  it = l.insert(++it, 9);
  it = l.insert(++it, 10);
  EXPECT_FALSE(l.begin() == l.end());
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(8));
  EXPECT_EQ(*(++l.begin()), static_cast<TypeParam>(9));
  EXPECT_EQ(*(++(++l.begin())), static_cast<TypeParam>(10));

  l.erase(l.begin());
  l.erase(l.begin());
  l.erase(l.begin());
  EXPECT_EQ(l.begin(), l.end());

  l.insert(l.begin(), static_cast<TypeParam>(32));
  EXPECT_EQ(*l.begin(), static_cast<TypeParam>(32));
}

TYPED_TEST(list, size) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  EXPECT_EQ(0u, l.size());
  l.emplace(l.begin(), static_cast<TypeParam>(0));
  EXPECT_EQ(1u, l.size());
  l.emplace(l.begin(), static_cast<TypeParam>(0));
  EXPECT_EQ(2u, l.size());
  for (size_t i = 0; i < 1000; ++i) {
    l.emplace(l.begin(), static_cast<TypeParam>(0));
  }
  EXPECT_EQ(1002u, l.size());
  l.erase(l.begin());
  EXPECT_EQ(1001u, l.size());
  for (size_t i = 0; i < 1000; ++i) {
    l.erase(l.begin());
  }
  EXPECT_EQ(1u, l.size());
  l.erase(l.begin());
  EXPECT_EQ(0u, l.size());
  l.emplace(l.begin(), static_cast<TypeParam>(32));
  EXPECT_EQ(1u, l.size());
}

struct construction_test_struct {
  enum class construction_type {
    constructor,
    copy_constructor,
    move_constructor,
    typed_constructor
  };

  construction_test_struct() {
    type = construction_type::constructor;
  }

  construction_test_struct(int64_t v1, int8_t v2) : v1(v1), v2(v2) {
    type = construction_type::typed_constructor;
  }

  construction_test_struct(const construction_test_struct&) {
    type = construction_type::copy_constructor;
  }

  construction_test_struct(construction_test_struct&& _other) {
    type = construction_type::move_constructor;
    v1 = _other.v1;
    v2 = _other.v2;
  }

  construction_type type;
  int64_t v1;
  int8_t v2;
};

TEST(list, emplace) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<construction_test_struct> l(&alloc);
  l.emplace(l.begin(), 32, static_cast<int8_t>(4));
  l.emplace(l.begin(), 4, static_cast<int8_t>(12));
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      l.begin()->type);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      (l.begin() + 1)->type);

  EXPECT_EQ(4u, l.begin()->v1);
  EXPECT_EQ(12, l.begin()->v2);
  EXPECT_EQ(32u, (l.begin() + 1)->v1);
  EXPECT_EQ(4, (l.begin() + 1)->v2);
}

TEST(list, transfer_to_same_alloc) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc);
  l.emplace(l.begin());
  void* ptr = &(*l.begin());
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::constructor);
  l.transfer_to(l.begin(), l2.begin(), l2);
  EXPECT_EQ(l.begin(), l.end());
  // Nothing at all interesting should have happeend to this struct.
  EXPECT_EQ(l2.begin()->type,
      construction_test_struct::construction_type::constructor);
  EXPECT_EQ(ptr, &(*l2.begin()));
}

TEST(list, transfer_to_different_alloc) {
  wn::memory::basic_allocator alloc;
  wn::memory::basic_allocator alloc2;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc2);
  l.emplace(l.begin());
  void* ptr = &(*l.begin());
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::constructor);
  l.transfer_to(l.begin(), l2.begin(), l2);
  EXPECT_EQ(l.begin(), l.end());
  // Every element should have been moved.
  EXPECT_EQ(l2.begin()->type,
      construction_test_struct::construction_type::move_constructor);
  EXPECT_FALSE(ptr == &(*l2.begin()));
}

TEST(list, transfer_range_to_same_alloc) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc);
  l.emplace(l.begin());
  l.emplace(l.end());
  l.emplace(l.end());
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::constructor);
  l.transfer_to(l.begin(), l.end(), l2.begin(), l2);

  EXPECT_EQ(l.begin(), l.end());
  for (auto& element : l2) {
    // Nothing interesting should have happened
    EXPECT_EQ(
        element.type, construction_test_struct::construction_type::constructor);
  }
}

TEST(list, transfer_range_to_front) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc);
  l.emplace(l.begin(), 1, '0');
  l.emplace(l.end(), 2, '0');
  l.emplace(l.end(), 3, '0');
  l2.emplace(l2.begin(), 4, '0');
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::typed_constructor);
  l.transfer_to(l.begin(), l.end(), l2.begin(), l2);

  EXPECT_EQ(l.begin(), l.end());
  EXPECT_EQ(1, l2.begin()->v1);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      l2.begin()->type);
  EXPECT_EQ(2, (l2.begin() + 1)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      (l2.begin() + 1)->type);
  EXPECT_EQ(3, (l2.begin() + 2)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      (l2.begin() + 2)->type);
  EXPECT_EQ(4, (l2.begin() + 3)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      (l2.begin() + 3)->type);
}

TEST(list, transfer_range_to_front_different_alloc) {
  wn::memory::basic_allocator alloc;
  wn::memory::basic_allocator alloc2;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc2);
  l.emplace(l.begin(), 1, '0');
  l.emplace(l.end(), 2, '0');
  l.emplace(l.end(), 3, '0');
  l2.emplace(l2.begin(), 4, '0');
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::typed_constructor);
  l.transfer_to(l.begin(), l.end(), l2.begin(), l2);

  EXPECT_EQ(l.begin(), l.end());
  EXPECT_EQ(1, l2.begin()->v1);
  EXPECT_EQ(construction_test_struct::construction_type::move_constructor,
      l2.begin()->type);
  EXPECT_EQ(2, (l2.begin() + 1)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::move_constructor,
      (l2.begin() + 1)->type);
  EXPECT_EQ(3, (l2.begin() + 2)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::move_constructor,
      (l2.begin() + 2)->type);
  EXPECT_EQ(4, (l2.begin() + 3)->v1);
  EXPECT_EQ(construction_test_struct::construction_type::typed_constructor,
      (l2.begin() + 3)->type);
}

TEST(list, transfer_range_to_different_alloc) {
  wn::memory::basic_allocator alloc;
  wn::memory::basic_allocator alloc2;
  wn::containers::list<construction_test_struct> l(&alloc);
  wn::containers::list<construction_test_struct> l2(&alloc2);
  l.emplace(l.begin());
  l.emplace(l.end());
  l.emplace(l.end());
  EXPECT_EQ(l.begin()->type,
      construction_test_struct::construction_type::constructor);
  l.transfer_to(l.begin(), l.end(), l2.begin(), l2);

  EXPECT_EQ(l.begin(), l.end());
  for (auto& element : l2) {
    // Every element should have been moved
    EXPECT_EQ(element.type,
        construction_test_struct::construction_type::move_constructor);
  }
}

TYPED_TEST(list, iterate) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  for (size_t i = 0; i < 10; ++i) {
    l.push_back(static_cast<TypeParam>(i));
  }
  typename wn::containers::list<TypeParam>::iterator it = l.begin();
  typename wn::containers::list<TypeParam>::const_iterator cit = l.cbegin();
  for (size_t i = 0; i < 10; ++i) {
    EXPECT_EQ(i, *it++);
    EXPECT_EQ(i, *cit++);
  }
  EXPECT_EQ(l.end(), it);
  EXPECT_EQ(l.cend(), cit);
}

TYPED_TEST(list, reverse_iterate) {
  wn::memory::basic_allocator alloc;
  wn::containers::list<TypeParam> l(&alloc);
  for (size_t i = 0; i < 10; ++i) {
    l.push_back(static_cast<TypeParam>(i));
  }
  typename wn::containers::list<TypeParam>::reverse_iterator rit = l.rbegin();
  typename wn::containers::list<TypeParam>::const_reverse_iterator crit =
      l.crbegin();
  for (size_t i = 0; i < 10; ++i) {
    EXPECT_EQ(10 - i - 1, *rit++);
    EXPECT_EQ(10 - i - 1, *crit++);
  }
  EXPECT_EQ(l.rend(), rit);
  EXPECT_EQ(l.crend(), crit);
}
