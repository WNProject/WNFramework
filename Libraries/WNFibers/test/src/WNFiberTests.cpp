// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNFibers/inc/WNFiber.h"

void function(int32_t* x, wn::fibers::fiber* main_fiber) {
  *x *= 10;
  wn::fibers::this_fiber::swap_to(main_fiber);
}

void multi_function(int32_t* x, int32_t next_fiber,
    wn::containers::dynamic_array<wn::fibers::fiber>* all_fibers) {
  if (next_fiber == 5) {
    // Lets try to force a resize of the dynamic_array, all fibers
    // including this one, should continue to work, and swap correctl
    // etc.
    all_fibers->reserve(100);
  }
  *x += 10;
  wn::fibers::this_fiber::swap_to(&(*all_fibers)[next_fiber]);
}

TEST(fibers, simple_fiber){
  wn::memory::default_allocator m_allocator;
  int32_t x = 32;
  wn::fibers::this_fiber::convert_to_fiber(&m_allocator);

  wn::fibers::fiber f(
      &m_allocator, function, &x, wn::fibers::this_fiber::get_self());

  wn::fibers::this_fiber::swap_to(&f);
  wn::fibers::this_fiber::revert_from_fiber();

  EXPECT_EQ(320, x);
}

TEST(fibers, dynamic_array_of_fibers) {
  wn::memory::default_allocator m_allocator;
  int32_t x = 32;
  wn::fibers::this_fiber::convert_to_fiber(&m_allocator);

  wn::containers::dynamic_array<wn::fibers::fiber> fibers;
  fibers.reserve(11);
  for(size_t i = 0; i < 10; ++i) {
    fibers.emplace_back(
        &m_allocator, multi_function, &x, i+1, &fibers);
  }

  fibers.emplace_back(
      &m_allocator, function, &x, wn::fibers::this_fiber::get_self());

  wn::fibers::this_fiber::swap_to(&fibers[0]);
  wn::fibers::this_fiber::revert_from_fiber();

  EXPECT_EQ((32 + 10 * 10) * 10, x);
}
