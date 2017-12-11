// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNArray.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/WNThread.h"
#include "WNMultiTasking/inc/spin_lock.h"

namespace {

const uint32_t k_expected_count = 1000u;

}  // namespace

TEST(spin_lock, construction) {
  wn::multi_tasking::spin_lock spin_lock;
}

TEST(spin_lock, lock_unlock) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::spin_lock spin_lock;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i] = wn::multi_tasking::thread(
          &allocator, [&numbers, &count, &spin_lock]() {
            spin_lock.lock();

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;

            spin_lock.unlock();
          });
    }

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, k_expected_count);

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      EXPECT_EQ(numbers[i], i);
    }
  }
}

TEST(spin_lock, try_lock) {
  wn::testing::allocator allocator;

  {
    bool got_lock = true;
    wn::multi_tasking::spin_lock spin_lock;

    spin_lock.lock();

    wn::multi_tasking::thread thread(&allocator,
        [&got_lock, &spin_lock]() { got_lock = spin_lock.try_lock(); });

    thread.join();

    EXPECT_FALSE(got_lock);

    spin_lock.unlock();

    thread = wn::multi_tasking::thread(&allocator, [&got_lock, &spin_lock]() {
      got_lock = spin_lock.try_lock();

      if (got_lock) {
        spin_lock.unlock();
      }
    });

    thread.join();

    EXPECT_TRUE(got_lock);
  }
}

TEST(spin_lock_guard, protect_scope) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::spin_lock spin_lock;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i] = wn::multi_tasking::thread(
          &allocator, [&numbers, &count, &spin_lock]() {
            wn::multi_tasking::spin_lock_guard guard(spin_lock);

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;
          });
    }

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, k_expected_count);

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      EXPECT_EQ(numbers[i], i);
    }
  }
}
