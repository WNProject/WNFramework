// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"

TEST(allocator, basic_usage) {
    wn::memory::default_allocator allocator;
    wn::memory::allocation_pair pair = allocator.allocate(1, 1);

    ASSERT_GE(pair.m_count, 1u);
    ASSERT_NE(pair.m_location, wn_nullptr);

    wn_char* c = static_cast<wn_char*>(pair.m_location);

    c[0] = 'a';

    allocator.deallocate(pair.m_location);
}

TEST(allocator, multiple_times) {
    wn::memory::default_allocator allocator;
    wn::memory::allocation_pair pair = allocator.allocate(1, 10);

    ASSERT_GE(pair.m_count, 10u);

    wn_char* c = static_cast<wn_char*>(pair.m_location);

    for (wn_size_t i = 0; i < 10; ++i) {
        c[i] = 'a';
    }

    ASSERT_NE(pair.m_location, wn_nullptr);

    allocator.deallocate(pair.m_location);
}

TEST(allocator, page_size) {
    wn::memory::default_allocator allocator;
    wn::memory::allocation_pair pair = allocator.allocate(4096, 1);

    ASSERT_GE(pair.m_count, 1u);

    wn_char* c = static_cast<wn_char*>(pair.m_location);

    for (wn_size_t i = 0; i < 4096; ++i) {
        c[i] = 'a';
    }

    ASSERT_NE(pair.m_location, wn_nullptr);

    allocator.deallocate(pair.m_location);
}

TEST(allocator, reallocate) {
    wn::memory::default_allocator allocator;
    wn::memory::allocation_pair pair = allocator.allocate(1, 1);

    ASSERT_GE(pair.m_count, 1u);
    ASSERT_NE(pair.m_location, wn_nullptr);

    wn_char* c = static_cast<wn_char*>(pair.m_location);

    c[0] = 'a';

    pair = allocator.reallocate(c, 1, 100);

    c = static_cast<wn_char*>(pair.m_location);

    ASSERT_EQ(c[0], 'a');

    for (wn_size_t i = 1; i < 100; ++i) {
        c[i] = 'a';
    }

    ASSERT_NE(pair.m_location, wn_nullptr);

    allocator.deallocate(pair.m_location);
}

TEST(allocator, page_reallocate) {
    wn::memory::default_test_allocator allocator;
    wn::memory::allocation_pair pair = allocator.allocate(4096, 1);

    EXPECT_GE(pair.m_count, 1u);
    EXPECT_NE(pair.m_location, wn_nullptr);

    wn_char* c = static_cast<wn_char*>(pair.m_location);

    for (wn_size_t i = 0; i < 4096; ++i) {
        c[i] = 'a';
    }

    pair = allocator.reallocate(c, 4096, 2);

    EXPECT_GE(pair.m_count, 2u);
    EXPECT_NE(pair.m_location, wn_nullptr);

    c = static_cast<wn_char*>(pair.m_location);

    for (wn_size_t i = 0; i < 4096; ++i) {
      EXPECT_EQ(c[i], 'a');
    }

    for (wn_size_t i = 0; i < 4096; ++i) {
        c[i + 4096] = 'a';
    }

    EXPECT_NE(pair.m_location, wn_nullptr);

    allocator.deallocate(pair.m_location);
    EXPECT_EQ(allocator.allocated(), allocator.freed());
}