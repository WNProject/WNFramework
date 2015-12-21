// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNFibers/inc/WNFiber.h"

void function(int32_t* x, wn::fibers::fiber* main_fiber) {
  *x *= 10;
  wn::fibers::this_fiber::swap_to(main_fiber);
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
