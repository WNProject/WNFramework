// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNArray.h"
#include "WNMultiTasking/inc/shared_mutex.h"
#include "WNMultiTasking/inc/thread.h"
#include "executable_test/inc/WNTestHarness.h"

namespace {

const uint32_t k_expected_count = 100u;

}  // namespace

TEST(shared_mutex, construction) {
  wn::multi_tasking::shared_mutex shared_mutex;
}

TEST(shared_mutex, lock_unlock) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::shared_mutex shared_mutex;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      threads[i] = wn::multi_tasking::thread(
          &allocator, [&numbers, &count, &shared_mutex]() {
            shared_mutex.lock();

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;

            shared_mutex.unlock();
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

TEST(shared_mutex, try_lock) {
  wn::testing::allocator allocator;

  {
    bool got_lock = true;
    wn::multi_tasking::shared_mutex shared_mutex;

    shared_mutex.lock();

    wn::multi_tasking::thread thread(&allocator,
        [&got_lock, &shared_mutex]() { got_lock = shared_mutex.try_lock(); });

    thread.join();

    EXPECT_FALSE(got_lock);

    shared_mutex.unlock();

    thread =
        wn::multi_tasking::thread(&allocator, [&got_lock, &shared_mutex]() {
          got_lock = shared_mutex.try_lock();

          if (got_lock) {
            shared_mutex.unlock();
          }
        });

    thread.join();

    EXPECT_TRUE(got_lock);
  }
}
