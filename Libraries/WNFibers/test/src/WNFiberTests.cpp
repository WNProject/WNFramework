// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNFibers/inc/WNFiber.h"

using ::testing::ElementsAreArray;

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

TEST(fibers, dynamic_array_of_fibers) {
  wn::memory::default_allocator m_allocator;
  int32_t x = 32;
  wn::fibers::this_fiber::convert_to_fiber(&m_allocator);

  wn::containers::dynamic_array<wn::fibers::fiber> fibers;
  fibers.reserve(11);
  for(int32_t i = 0; i < 10; ++i) {
    fibers.emplace_back(
        &m_allocator, multi_function, &x, i+1, &fibers);
  }

  fibers.emplace_back(
      &m_allocator, function, &x, wn::fibers::this_fiber::get_self());

  wn::fibers::this_fiber::swap_to(&fibers[0]);
  wn::fibers::this_fiber::revert_from_fiber();

  EXPECT_EQ((32 + 10 * 10) * 10, x);
}


void deep_call(int32_t* x, int32_t depth, wn::fibers::fiber* next_fiber) {
  if (depth == 0) {
    wn::fibers::this_fiber::swap_to(next_fiber);
    // In order to appease the compiler, we call return here.
    // We will never actaully hit it because we never return to this method.
    return;
  }
  *x = *x + 1;
  deep_call(x, depth - 1, next_fiber);
}

TEST(fibers, deep_function) {
  wn::memory::default_allocator m_allocator;
  int32_t x = 32;
  wn::fibers::this_fiber::convert_to_fiber(&m_allocator);

  wn::fibers::fiber f(
      &m_allocator, deep_call, &x, 2048, wn::fibers::this_fiber::get_self());
  wn::fibers::this_fiber::swap_to(&f);
  wn::fibers::this_fiber::revert_from_fiber();

  EXPECT_EQ(32+2048, x);
}

// This generator generates 6 numbers every time it is swapped to
// and takes no action after that point.
void generator_function(int32_t* destination, wn::fibers::fiber* main_fiber){
  *destination = 32;
  wn::fibers::this_fiber::swap_to(main_fiber);
  *destination = 42;
  wn::fibers::this_fiber::swap_to(main_fiber);
  *destination = 10;
  wn::fibers::this_fiber::swap_to(main_fiber);
  *destination = 12;
  wn::fibers::this_fiber::swap_to(main_fiber);
  *destination = 96;
  wn::fibers::this_fiber::swap_to(main_fiber);
  *destination = 45;
  wn::fibers::this_fiber::swap_to(main_fiber);
  for(;;) {
    wn::fibers::this_fiber::swap_to(main_fiber);
  }
}

TEST(fibers, generator_function) {
  wn::memory::default_allocator m_allocator;
  int32_t destination = 0;

  wn::fibers::this_fiber::convert_to_fiber(&m_allocator);
  wn::fibers::fiber f(&m_allocator, generator_function, &destination,
      wn::fibers::this_fiber::get_self());
  wn::containers::dynamic_array<int32_t> arr(&m_allocator);

  for(size_t i = 0; i < 8; ++i) {
    arr.push_back(destination);
    wn::fibers::this_fiber::swap_to(&f);
  }

  wn::fibers::this_fiber::revert_from_fiber();
  int32_t expected[] = {0, 32, 42, 10, 12, 96, 45, 45};
  EXPECT_THAT(arr, ElementsAreArray(expected));
}
