// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/basic_allocator.h"
#include "executable_test/inc/WNTestHarness.h"

TEST(basic_allocator, basic_usage) {
  wn::memory::basic_allocator basic_allocator;
  wn::memory::allocator* allocator = &basic_allocator;

  void* ptr = allocator->allocate(1);

  ASSERT_NE(ptr, nullptr);

  char* c = static_cast<char*>(ptr);

  c[0] = 'a';

  allocator->deallocate(ptr);
}

TEST(basic_allocator, multiple_times) {
  wn::memory::basic_allocator basic_allocator;
  wn::memory::allocator* allocator = &basic_allocator;
  void* ptr = allocator->allocate(10);

  ASSERT_NE(ptr, nullptr);

  char* c = static_cast<char*>(ptr);

  for (size_t i = 0; i < 10; ++i) {
    c[i] = 'a';
  }

  allocator->deallocate(ptr);
}

TEST(basic_allocator, page_size) {
  wn::memory::basic_allocator basic_allocator;
  wn::memory::allocator* allocator = &basic_allocator;
  void* ptr = allocator->allocate(4096);

  ASSERT_NE(ptr, nullptr);

  char* c = static_cast<char*>(ptr);

  for (size_t i = 0; i < 4096; ++i) {
    c[i] = 'a';
  }

  allocator->deallocate(ptr);
}

TEST(basic_allocator, reallocate) {
  wn::memory::basic_allocator basic_allocator;
  wn::memory::allocator* allocator = &basic_allocator;
  void* ptr = allocator->allocate(1);

  ASSERT_NE(ptr, nullptr);

  char* c = static_cast<char*>(ptr);

  c[0] = 'a';

  ptr = allocator->reallocate(c, 100);

  c = static_cast<char*>(ptr);

  ASSERT_EQ(c[0], 'a');

  for (size_t i = 1; i < 100; ++i) {
    c[i] = 'a';
  }

  ASSERT_NE(ptr, nullptr);

  allocator->deallocate(ptr);
}

TEST(basic_allocator, page_reallocate) {
  wn::memory::basic_allocator basic_allocator;
  wn::memory::allocator* allocator = &basic_allocator;
  void* ptr = allocator->allocate(4096);

  EXPECT_NE(ptr, nullptr);

  char* c = static_cast<char*>(ptr);

  for (size_t i = 0; i < 4096; ++i) {
    c[i] = 'a';
  }

  ptr = allocator->reallocate(c, 4096 * 2);

  EXPECT_NE(ptr, nullptr);

  c = static_cast<char*>(ptr);

  for (size_t i = 0; i < 4096; ++i) {
    EXPECT_EQ(c[i], 'a');
  }

  for (size_t i = 0; i < 4096; ++i) {
    c[i + 4096] = 'a';
  }

  EXPECT_NE(ptr, nullptr);

  allocator->deallocate(ptr);
}
