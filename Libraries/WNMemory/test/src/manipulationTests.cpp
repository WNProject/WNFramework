// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/inc/manipulation.h"
#include "executable_test/inc/WNTestHarness.h"

template <typename T>
struct memory : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    memory_testing_types;

TYPED_TEST_SUITE(memory, memory_testing_types);

template <typename T>
struct dummy_construct {
  dummy_construct(T& _value) {
    _value = static_cast<T>(2);
  }

  dummy_construct(T& _value1, T& _value2) : dummy_construct(_value1) {
    _value2 = static_cast<T>(4);
  }
};

TYPED_TEST(memory, memzero) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memzero(&value, sizeof(TypeParam));
  wn::memory::memzero(values, sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(memory, memory_zero) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memory_zero(&value);
  wn::memory::memory_zero(values, 4);
  wn::memory::memory_zero(static_cast<void*>(values), sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(memory, memory_set) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memory_set(&value, 0);
  wn::memory::memory_set(values, 0, 4);
  wn::memory::memory_set(static_cast<void*>(values), 0, sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(memory, memory_copy) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
      static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  wn::memory::memory_copy(&value1, &value2);
  wn::memory::memory_copy(values1, values2, 4);
  wn::memory::memory_copy(static_cast<void*>(values1),
      static_cast<void*>(values2), sizeof(TypeParam) * 4);

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

TYPED_TEST(memory, memory_move) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
      static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  wn::memory::memory_move(&value1, &value2);
  wn::memory::memory_move(values1, values2, 4);
  wn::memory::memory_move(static_cast<void*>(values1),
      static_cast<void*>(values2), sizeof(TypeParam) * 4);

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

TYPED_TEST(memory, memory_compare) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
      static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
      static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  int32_t compare1 = wn::memory::memory_compare(&value1, &value2);
  int32_t compare2 = wn::memory::memory_compare(values1, values2, 4);

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

TYPED_TEST(memory, prefetch) {
  TypeParam value = static_cast<TypeParam>(0);

  wn::memory::prefetch(&value);

  TypeParam values[4] = {static_cast<TypeParam>(0)};

  wn::memory::prefetch(&values, 2);

  const void* void_value = values;

  wn::memory::prefetch(&void_value, sizeof(TypeParam) * 3);
}
