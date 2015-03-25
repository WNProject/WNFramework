// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"

template <typename _Type>
struct allocation : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64> allocation_testing_types;

TYPED_TEST_CASE(allocation, allocation_testing_types);

template <typename _Type>
struct dummy_construct {
    dummy_construct(_Type& _value) {
        _value = static_cast<_Type>(2);
    }

    dummy_construct(_Type& _value1, _Type& _value2) :
        dummy_construct(_value1) {
        _value2 = static_cast<_Type>(4);
    }
};

TYPED_TEST(allocation, malloc_realloc_free) {
    TypeParam* memory1 = static_cast<TypeParam*>(wn::memory::malloc(sizeof(TypeParam)));
    TypeParam* memory2 = static_cast<TypeParam*>(wn::memory::malloc(sizeof(TypeParam) * 4));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);

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

    memory1 = static_cast<TypeParam*>(wn::memory::realloc(memory1, sizeof(TypeParam) * 2));

    ASSERT_NE(memory1, wn_nullptr);

    memory1[1] = static_cast<TypeParam>(100);

    ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
    ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));

    wn_void* null_memory = wn_nullptr;
    TypeParam* memory3 = static_cast<TypeParam*>(wn::memory::realloc(null_memory, sizeof(TypeParam)));

    ASSERT_NE(memory3, wn_nullptr);

    memory3[0] = static_cast<TypeParam>(66);

    ASSERT_EQ(memory3[0], static_cast<TypeParam>(66));

    wn::memory::free(memory1);
    wn::memory::free(memory2);
    wn::memory::free(memory3);
    wn::memory::free(null_memory);
}

TYPED_TEST(allocation, aligned_malloc_realloc_free) {
    TypeParam* memory1 = static_cast<TypeParam*>(wn::memory::aligned_malloc(sizeof(TypeParam), 16));
    TypeParam* memory2 = static_cast<TypeParam*>(wn::memory::aligned_malloc(sizeof(TypeParam) * 4, 16));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);

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

    memory1 = static_cast<TypeParam*>(wn::memory::aligned_realloc(memory1, sizeof(TypeParam) * 2, 16));

    ASSERT_NE(memory1, wn_nullptr);

    memory1[1] = static_cast<TypeParam>(100);

    ASSERT_EQ(memory1[0], static_cast<TypeParam>(1));
    ASSERT_EQ(memory1[1], static_cast<TypeParam>(100));

    wn_void* null_memory = wn_nullptr;
    TypeParam* memory3 = static_cast<TypeParam*>(wn::memory::aligned_realloc(null_memory, sizeof(TypeParam), 16));

    ASSERT_NE(memory3, wn_nullptr);

    memory3[0] = static_cast<TypeParam>(66);

    ASSERT_EQ(memory3[0], static_cast<TypeParam>(66));

    wn::memory::aligned_free(memory1);
    wn::memory::aligned_free(memory2);
    wn::memory::aligned_free(memory3);
    wn::memory::aligned_free(null_memory);
}

TYPED_TEST(allocation, heap_allocate_reallocate_free) {
    TypeParam* memory1 = wn::memory::heap_allocate<TypeParam>();
    TypeParam* memory2 = wn::memory::heap_allocate<TypeParam>(4);
    TypeParam* memory3 = static_cast<TypeParam*>(wn::memory::heap_allocate<wn_void>(sizeof(TypeParam)));
    TypeParam* memory4 = static_cast<TypeParam*>(wn::memory::heap_allocate<wn_void>(sizeof(TypeParam) * 4));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);
    ASSERT_NE(memory3, wn_nullptr);
    ASSERT_NE(memory4, wn_nullptr);

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
    memory2 = static_cast<TypeParam*>(wn::memory::heap_reallocate(static_cast<wn_void*>(memory2), 8 * sizeof(TypeParam)));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);

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

    TypeParam* null_memory = wn_nullptr;
    TypeParam* memory5 = wn::memory::heap_reallocate(null_memory, 1);

    ASSERT_NE(memory5, wn_nullptr);

    memory5[0] = static_cast<TypeParam>(66);

    ASSERT_EQ(memory5[0], static_cast<TypeParam>(66));

    wn::memory::heap_free(memory1);
    wn::memory::heap_free(memory2);
    wn::memory::heap_free(memory3);
    wn::memory::heap_free(memory4);
    wn::memory::heap_free(memory5);
    wn::memory::heap_free(null_memory);
}

TYPED_TEST(allocation, heap_aligned_allocate_reallocate_free) {
    TypeParam* memory1 = wn::memory::heap_aligned_allocate<TypeParam>(16);
    TypeParam* memory2 = wn::memory::heap_aligned_allocate<TypeParam>(4, 16);
    TypeParam* memory3 = static_cast<TypeParam*>(wn::memory::heap_aligned_allocate<wn_void>(sizeof(TypeParam), 16));
    TypeParam* memory4 = static_cast<TypeParam*>(wn::memory::heap_aligned_allocate<wn_void>(sizeof(TypeParam) * 4, 16));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);
    ASSERT_NE(memory3, wn_nullptr);
    ASSERT_NE(memory4, wn_nullptr);

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
    memory2 = static_cast<TypeParam*>(wn::memory::heap_aligned_reallocate(static_cast<wn_void*>(memory2), 8 * sizeof(TypeParam), 16));

    ASSERT_NE(memory1, wn_nullptr);
    ASSERT_NE(memory2, wn_nullptr);

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

    TypeParam* null_memory = wn_nullptr;
    TypeParam* memory5 = wn::memory::heap_aligned_reallocate(null_memory, 1, 16);

    ASSERT_NE(memory5, wn_nullptr);

    memory5[0] = static_cast<TypeParam>(66);

    ASSERT_EQ(memory5[0], static_cast<TypeParam>(66));

    wn::memory::heap_aligned_free(memory1);
    wn::memory::heap_aligned_free(memory2);
    wn::memory::heap_aligned_free(memory3);
    wn::memory::heap_aligned_free(memory4);
    wn::memory::heap_aligned_free(memory5);
    wn::memory::heap_aligned_free(null_memory);
}

TYPED_TEST(allocation, construct_destroy) {
    TypeParam value1 = static_cast<TypeParam>(1);
    TypeParam value2 = static_cast<TypeParam>(1);
    TypeParam value3 = static_cast<TypeParam>(2);
    dummy_construct<TypeParam>* dummy1 = wn::memory::construct<dummy_construct<TypeParam>>(value1);
    dummy_construct<TypeParam>* dummy2 = wn::memory::construct<dummy_construct<TypeParam>>(value2, value3);

    ASSERT_NE(dummy1, wn_nullptr);
    ASSERT_NE(dummy2, wn_nullptr);
    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(value3, static_cast<TypeParam>(4));

    value1 = static_cast<TypeParam>(1);
    value2 = static_cast<TypeParam>(1);
    value3 = static_cast<TypeParam>(2);

    dummy_construct<TypeParam>* dummy3 = wn::memory::heap_allocate<dummy_construct<TypeParam>>();
    dummy_construct<TypeParam>* dummy4 = wn::memory::heap_allocate<dummy_construct<TypeParam>>();

    ASSERT_NE(dummy3, wn_nullptr);
    ASSERT_NE(dummy4, wn_nullptr);

    dummy3 = wn::memory::construct_at(dummy3, value1);
    dummy4 = wn::memory::construct_at(dummy4, value2, value3);

    dummy_construct<TypeParam>* dummy5 = wn_nullptr;

    dummy5 = wn::memory::construct_at(dummy5, value1);

    ASSERT_NE(dummy3, wn_nullptr);
    ASSERT_NE(dummy4, wn_nullptr);
    ASSERT_EQ(dummy5, wn_nullptr);
    ASSERT_EQ(value1, static_cast<TypeParam>(2));
    ASSERT_EQ(value2, static_cast<TypeParam>(2));
    ASSERT_EQ(value3, static_cast<TypeParam>(4));

    wn::memory::destroy(dummy1);
    wn::memory::destroy(dummy2);
    wn::memory::destroy(dummy3);
    wn::memory::destroy(dummy4);
    wn::memory::destroy(dummy5);
}