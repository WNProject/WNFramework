// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNArray.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/thread.h"
#include "WNMultiTasking/inc/mutex.h"

namespace {

const uint32_t k_expected_count = 1000u;

}  // namespace

TEST(mutex, construction) {
  wn::multi_tasking::mutex mutex1;

  EXPECT_EQ(mutex1.spin_count(), wn::multi_tasking::mutex::default_spin_count);

  wn::multi_tasking::mutex mutex2(50u);

  EXPECT_EQ(mutex2.spin_count(), 50u);
}

TEST(mutex, lock_unlock) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::mutex mutex;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i] =
          wn::multi_tasking::thread(&allocator, [&numbers, &count, &mutex]() {
            mutex.lock();

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;

            mutex.unlock();
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

TEST(mutex, try_lock) {
  wn::testing::allocator allocator;

  {
    bool got_lock = true;
    wn::multi_tasking::mutex mutex;

    mutex.lock();

    wn::multi_tasking::thread thread(
        &allocator, [&got_lock, &mutex]() { got_lock = mutex.try_lock(); });

    thread.join();

    EXPECT_FALSE(got_lock);

    mutex.unlock();

    thread = wn::multi_tasking::thread(&allocator, [&got_lock, &mutex]() {
      got_lock = mutex.try_lock();

      if (got_lock) {
        mutex.unlock();
      }
    });

    thread.join();

    EXPECT_TRUE(got_lock);
  }
}

TEST(mutex_guard, protect_scope) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::mutex mutex;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i] =
          wn::multi_tasking::thread(&allocator, [&numbers, &count, &mutex]() {
            wn::multi_tasking::mutex_guard guard(mutex);

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;
          });
    }

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    std::stringstream expected_string;

    EXPECT_EQ(count, k_expected_count);

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      EXPECT_EQ(numbers[i], i);
    }
  }
}
