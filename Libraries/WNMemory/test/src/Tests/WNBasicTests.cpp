// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNMemory/inc/WNBasic.h"

template <typename T>
struct basic : ::testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
  basic_testing_types;

TYPED_TEST_CASE(basic, basic_testing_types);

template <typename T> struct dummy_construct {
  dummy_construct(T& _value) {
    _value = static_cast<T>(2);
  }

  dummy_construct(T& _value1, T& _value2) :
    dummy_construct(_value1) {
    _value2 = static_cast<T>(4);
  }
};

TYPED_TEST(basic, malloc_realloc_free) {
  TypeParam* memory1 =
      static_cast<TypeParam*>(wn::memory::malloc(sizeof(TypeParam)));
  TypeParam* memory2 =
      static_cast<TypeParam*>(wn::memory::malloc(sizeof(TypeParam) * 4));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);

  memory1[0] = static_cast<TypeParam>(1);
  memory2[0] = static_cast<TypeParam>(2);
  memory2[1] = static_cast<TypeParam>(3);
  memory2[2] = static_cast<TypeParam>(4);
  memory2[3] = static_cast<TypeParam>(5);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));

  memory1 = static_cast<TypeParam*>(
      wn::memory::realloc(memory1, sizeof(TypeParam) * 2));

  ASSERT_NE(memory1, nullptr);

  memory1[1] = static_cast<TypeParam>(100);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));

  void* null_memory = nullptr;
  TypeParam* memory3 = static_cast<TypeParam*>(
      wn::memory::realloc(null_memory, sizeof(TypeParam)));

  ASSERT_NE(memory3, nullptr);

  memory3[0] = static_cast<TypeParam>(66);

  ASSERT_EQ(memory3[0], static_cast<TypeParam>(66));

  wn::memory::free(memory1);
  wn::memory::free(memory2);
  wn::memory::free(memory3);
  wn::memory::free(null_memory);
}

TYPED_TEST(basic, aligned_malloc_realloc_free) {
  TypeParam* memory1 = static_cast<TypeParam*>(
      wn::memory::aligned_malloc(sizeof(TypeParam), 16));
  TypeParam* memory2 = static_cast<TypeParam*>(
      wn::memory::aligned_malloc(sizeof(TypeParam) * 4, 16));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);

  memory1[0] = static_cast<TypeParam>(1);
  memory2[0] = static_cast<TypeParam>(2);
  memory2[1] = static_cast<TypeParam>(3);
  memory2[2] = static_cast<TypeParam>(4);
  memory2[3] = static_cast<TypeParam>(5);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));

  memory1 = static_cast<TypeParam*>(
      wn::memory::aligned_realloc(memory1, sizeof(TypeParam) * 2, 16));

  ASSERT_NE(memory1, nullptr);

  memory1[1] = static_cast<TypeParam>(100);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));

  void* null_memory = nullptr;
  TypeParam* memory3 = static_cast<TypeParam*>(
      wn::memory::aligned_realloc(null_memory, sizeof(TypeParam), 16));

  ASSERT_NE(memory3, nullptr);

  memory3[0] = static_cast<TypeParam>(66);

  ASSERT_EQ(memory3[0], static_cast<TypeParam>(66));

  wn::memory::aligned_free(memory1);
  wn::memory::aligned_free(memory2);
  wn::memory::aligned_free(memory3);
  wn::memory::aligned_free(null_memory);
}

TYPED_TEST(basic, heap_allocate_reallocate_free) {
  TypeParam* memory1 = wn::memory::heap_allocate<TypeParam>();
  TypeParam* memory2 = wn::memory::heap_allocate<TypeParam>(4);
  TypeParam* memory3 = static_cast<TypeParam*>(
      wn::memory::heap_allocate<void>(sizeof(TypeParam)));
  TypeParam* memory4 = static_cast<TypeParam*>(
      wn::memory::heap_allocate<void>(sizeof(TypeParam) * 4));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);
  ASSERT_NE(memory3, nullptr);
  ASSERT_NE(memory4, nullptr);

  memory1[0] = static_cast<TypeParam>(1);
  memory2[0] = static_cast<TypeParam>(2);
  memory2[1] = static_cast<TypeParam>(3);
  memory2[2] = static_cast<TypeParam>(4);
  memory2[3] = static_cast<TypeParam>(5);
  memory3[0] = static_cast<TypeParam>(6);
  memory4[0] = static_cast<TypeParam>(7);
  memory4[1] = static_cast<TypeParam>(8);
  memory4[2] = static_cast<TypeParam>(9);
  memory4[3] = static_cast<TypeParam>(10);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));
  ASSERT_EQ(memory3[0], static_cast<TypeParam>(6));
  ASSERT_EQ(memory4[0], static_cast<TypeParam>(7));
  ASSERT_EQ(memory4[1], static_cast<TypeParam>(8));
  ASSERT_EQ(memory4[2], static_cast<TypeParam>(9));
  ASSERT_EQ(memory4[3], static_cast<TypeParam>(10));

  memory1 = wn::memory::heap_reallocate(memory1, 2);
  memory2 = static_cast<TypeParam*>(wn::memory::heap_reallocate(
      static_cast<void*>(memory2), 8 * sizeof(TypeParam)));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);

  memory1[1] = static_cast<TypeParam>(100);
  memory2[4] = static_cast<TypeParam>(101);
  memory2[5] = static_cast<TypeParam>(102);
  memory2[6] = static_cast<TypeParam>(103);
  memory2[7] = static_cast<TypeParam>(104);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));
  ASSERT_EQ(memory2[4], static_cast<TypeParam>(101));
  ASSERT_EQ(memory2[5], static_cast<TypeParam>(102));
  ASSERT_EQ(memory2[6], static_cast<TypeParam>(103));
  ASSERT_EQ(memory2[7], static_cast<TypeParam>(104));

  TypeParam* null_memory = nullptr;
  TypeParam* memory5 = wn::memory::heap_reallocate(null_memory, 1);

  ASSERT_NE(memory5, nullptr);

  memory5[0] = static_cast<TypeParam>(66);

  ASSERT_EQ(memory5[0], static_cast<TypeParam>(66));

  wn::memory::heap_free(memory1);
  wn::memory::heap_free(memory2);
  wn::memory::heap_free(memory3);
  wn::memory::heap_free(memory4);
  wn::memory::heap_free(memory5);
  wn::memory::heap_free(null_memory);
}

TYPED_TEST(basic, heap_aligned_allocate_reallocate_free) {
  TypeParam* memory1 = wn::memory::heap_aligned_allocate<TypeParam>(16);
  TypeParam* memory2 = wn::memory::heap_aligned_allocate<TypeParam>(4, 16);
  TypeParam* memory3 = static_cast<TypeParam*>(
      wn::memory::heap_aligned_allocate<void>(sizeof(TypeParam), 16));
  TypeParam* memory4 = static_cast<TypeParam*>(
      wn::memory::heap_aligned_allocate<void>(sizeof(TypeParam) * 4, 16));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);
  ASSERT_NE(memory3, nullptr);
  ASSERT_NE(memory4, nullptr);

  memory1[0] = static_cast<TypeParam>(1);
  memory2[0] = static_cast<TypeParam>(2);
  memory2[1] = static_cast<TypeParam>(3);
  memory2[2] = static_cast<TypeParam>(4);
  memory2[3] = static_cast<TypeParam>(5);
  memory3[0] = static_cast<TypeParam>(6);
  memory4[0] = static_cast<TypeParam>(7);
  memory4[1] = static_cast<TypeParam>(8);
  memory4[2] = static_cast<TypeParam>(9);
  memory4[3] = static_cast<TypeParam>(10);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));
  ASSERT_EQ(memory3[0], static_cast<TypeParam>(6));
  ASSERT_EQ(memory4[0], static_cast<TypeParam>(7));
  ASSERT_EQ(memory4[1], static_cast<TypeParam>(8));
  ASSERT_EQ(memory4[2], static_cast<TypeParam>(9));
  ASSERT_EQ(memory4[3], static_cast<TypeParam>(10));

  memory1 = wn::memory::heap_aligned_reallocate(memory1, 2, 16);
  memory2 = static_cast<TypeParam*>(wn::memory::heap_aligned_reallocate(
      static_cast<void*>(memory2), 8 * sizeof(TypeParam), 16));

  ASSERT_NE(memory1, nullptr);
  ASSERT_NE(memory2, nullptr);

  memory1[1] = static_cast<TypeParam>(100);
  memory2[4] = static_cast<TypeParam>(101);
  memory2[5] = static_cast<TypeParam>(102);
  memory2[6] = static_cast<TypeParam>(103);
  memory2[7] = static_cast<TypeParam>(104);

  ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
  ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));
  ASSERT_EQ(memory2[0], static_cast<TypeParam>(2));
  ASSERT_EQ(memory2[1], static_cast<TypeParam>(3));
  ASSERT_EQ(memory2[2], static_cast<TypeParam>(4));
  ASSERT_EQ(memory2[3], static_cast<TypeParam>(5));
  ASSERT_EQ(memory2[4], static_cast<TypeParam>(101));
  ASSERT_EQ(memory2[5], static_cast<TypeParam>(102));
  ASSERT_EQ(memory2[6], static_cast<TypeParam>(103));
  ASSERT_EQ(memory2[7], static_cast<TypeParam>(104));

  TypeParam* null_memory = nullptr;
  TypeParam* memory5 = wn::memory::heap_aligned_reallocate(null_memory, 1, 16);

  ASSERT_NE(memory5, nullptr);

  memory5[0] = static_cast<TypeParam>(66);

  ASSERT_EQ(memory5[0], static_cast<TypeParam>(66));

  wn::memory::heap_aligned_free(memory1);
  wn::memory::heap_aligned_free(memory2);
  wn::memory::heap_aligned_free(memory3);
  wn::memory::heap_aligned_free(memory4);
  wn::memory::heap_aligned_free(memory5);
  wn::memory::heap_aligned_free(null_memory);
}

TYPED_TEST(basic, construct_destroy) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(1);
  TypeParam value3 = static_cast<TypeParam>(2);
  dummy_construct<TypeParam>* dummy1 =
      wn::memory::construct<dummy_construct<TypeParam>>(value1);
  dummy_construct<TypeParam>* dummy2 =
      wn::memory::construct<dummy_construct<TypeParam>>(value2, value3);

  ASSERT_NE(dummy1, nullptr);
  ASSERT_NE(dummy2, nullptr);
  ASSERT_EQ(value1, static_cast<TypeParam>(2));
  ASSERT_EQ(value2, static_cast<TypeParam>(2));
  ASSERT_EQ(value3, static_cast<TypeParam>(4));

  value1 = static_cast<TypeParam>(1);
  value2 = static_cast<TypeParam>(1);
  value3 = static_cast<TypeParam>(2);

  dummy_construct<TypeParam>* dummy3 =
      wn::memory::heap_allocate<dummy_construct<TypeParam>>();
  dummy_construct<TypeParam>* dummy4 =
      wn::memory::heap_allocate<dummy_construct<TypeParam>>();

  ASSERT_NE(dummy3, nullptr);
  ASSERT_NE(dummy4, nullptr);

  dummy3 = wn::memory::construct_at(dummy3, value1);
  dummy4 = wn::memory::construct_at(dummy4, value2, value3);

  dummy_construct<TypeParam>* dummy5 = nullptr;

  dummy5 = wn::memory::construct_at(dummy5, value1);

  ASSERT_NE(dummy3, nullptr);
  ASSERT_NE(dummy4, nullptr);
  ASSERT_EQ(dummy5, nullptr);
  ASSERT_EQ(value1, static_cast<TypeParam>(2));
  ASSERT_EQ(value2, static_cast<TypeParam>(2));
  ASSERT_EQ(value3, static_cast<TypeParam>(4));

  wn::memory::destroy(dummy1);
  wn::memory::destroy(dummy2);
  wn::memory::destroy(dummy3);
  wn::memory::destroy(dummy4);
  wn::memory::destroy(dummy5);
}

TYPED_TEST(basic, memzero) {
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

TYPED_TEST(basic, memset) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                         static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memset(&value, 0, sizeof(TypeParam));
  wn::memory::memset(values, 0, sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(basic, memcpy) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                          static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
                          static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

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

TYPED_TEST(basic, memmove) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                          static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
                          static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

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

TYPED_TEST(basic, memcmp) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                          static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
                          static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  int32_t compare1 = wn::memory::memcmp(&value1, &value2, sizeof(TypeParam));
  int32_t compare2 =
      wn::memory::memcmp(values1, values2, sizeof(TypeParam) * 4);

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

TYPED_TEST(basic, memory_zero) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                         static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memory_zero(&value);
  wn::memory::memory_zero(values, 4);
  wn::memory::memory_zero(static_cast<void*>(values),
                          sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(basic, memory_set) {
  TypeParam value = static_cast<TypeParam>(1);
  TypeParam values[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                         static_cast<TypeParam>(3), static_cast<TypeParam>(4)};

  wn::memory::memory_set(&value, 0);
  wn::memory::memory_set(values, 0, 4);
  wn::memory::memory_set(static_cast<void*>(values), 0,
                         sizeof(TypeParam) * 4);

  ASSERT_EQ(value, static_cast<TypeParam>(0));
  ASSERT_EQ(values[0], static_cast<TypeParam>(0));
  ASSERT_EQ(values[1], static_cast<TypeParam>(0));
  ASSERT_EQ(values[2], static_cast<TypeParam>(0));
  ASSERT_EQ(values[3], static_cast<TypeParam>(0));
}

TYPED_TEST(basic, memory_copy) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                          static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
                          static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  wn::memory::memory_copy(&value1, &value2);
  wn::memory::memory_copy(values1, values2, 4);
  wn::memory::memory_copy(static_cast<void*>(values1),
                          static_cast<void*>(values2),
                          sizeof(TypeParam) * 4);

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

TYPED_TEST(basic, memory_move) {
  TypeParam value1 = static_cast<TypeParam>(1);
  TypeParam value2 = static_cast<TypeParam>(2);
  TypeParam values1[4] = {static_cast<TypeParam>(1), static_cast<TypeParam>(2),
                          static_cast<TypeParam>(3), static_cast<TypeParam>(4)};
  TypeParam values2[4] = {static_cast<TypeParam>(2), static_cast<TypeParam>(4),
                          static_cast<TypeParam>(6), static_cast<TypeParam>(8)};

  wn::memory::memory_move(&value1, &value2);
  wn::memory::memory_move(values1, values2, 4);
  wn::memory::memory_move(static_cast<void*>(values1),
                          static_cast<void*>(values2),
                          sizeof(TypeParam) * 4);

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

TYPED_TEST(basic, memory_compare) {
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

TYPED_TEST(basic, prefetch) {
  TypeParam value = static_cast<TypeParam>(0);

  wn::memory::prefetch(&value);

  TypeParam values[4] = {static_cast<TypeParam>(0)};

  wn::memory::prefetch(&values, 2);

  const void* void_value = values;

  wn::memory::prefetch(&void_value, sizeof(TypeParam) * 3);
}
