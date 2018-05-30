// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNArray.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/semaphore.h"
#include "WNMultiTasking/inc/thread.h"

namespace {

const uint16_t k_expected_count = 100u;

}  // namespace

TEST(semaphore, construction) {
  wn::multi_tasking::semaphore semaphore1;
  wn::multi_tasking::semaphore semaphore2(5u);
  wn::multi_tasking::semaphore semaphore3(wn::core::move(semaphore1));
}

TEST(semaphore, assignment) {
  wn::multi_tasking::semaphore semaphore1;
  wn::multi_tasking::semaphore semaphore2;

  semaphore2 = wn::core::move(semaphore1);
}

TEST(semaphore, swap) {
  wn::multi_tasking::semaphore semaphore1;
  wn::multi_tasking::semaphore semaphore2;

  semaphore1.swap(semaphore2);
}

TEST(semaphore, wait_notify) {
  wn::testing::allocator allocator;

  {
    uint16_t count = 0u;
    wn::multi_tasking::semaphore semaphore;
    wn::containers::array<uint32_t, k_expected_count> numbers;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i] = wn::multi_tasking::thread(
          &allocator, [&numbers, &count, &semaphore]() {
            semaphore.wait();

            const uint32_t current_count = count;

            numbers[current_count] = current_count;
            count++;

            semaphore.notify();
          });
    }

    ASSERT_EQ(count, 0u);

    semaphore.notify();

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, k_expected_count);

    for (uint32_t i = 0u; i < k_expected_count; ++i) {
      EXPECT_EQ(numbers[i], i);
    }
  }
}

TEST(semaphore, notify_count) {
  wn::testing::allocator allocator;

  {
    std::atomic<uint16_t> count = {0u};
    wn::multi_tasking::semaphore semaphore;
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i] =
          wn::multi_tasking::thread(&allocator, [&count, &semaphore]() {
            semaphore.wait();

            count++;
          });
    }

    ASSERT_EQ(count, 0u);

    semaphore.notify(k_expected_count);

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, k_expected_count);
  }
}

TEST(semaphore, try_wait) {
  wn::testing::allocator allocator;

  {
    bool got_lock = true;
    wn::multi_tasking::semaphore semaphore;
    wn::multi_tasking::thread thread(&allocator,
        [&got_lock, &semaphore]() { got_lock = semaphore.try_wait(); });

    thread.join();

    EXPECT_FALSE(got_lock);

    semaphore.notify();

    thread = wn::multi_tasking::thread(&allocator, [&got_lock, &semaphore]() {
      got_lock = semaphore.try_wait();

      if (got_lock) {
        semaphore.notify();
      }
    });

    thread.join();

    EXPECT_TRUE(got_lock);
  }
}

TEST(semaphore, initial_count) {
  wn::testing::allocator allocator;

  {
    std::atomic<uint16_t> count = {0u};
    wn::multi_tasking::semaphore semaphore(k_expected_count / 2u);
    wn::containers::array<wn::multi_tasking::thread, k_expected_count> threads;

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i] =
          wn::multi_tasking::thread(&allocator, [&count, &semaphore]() {
            semaphore.wait();

            count++;
          });
    }

    semaphore.notify(k_expected_count / 2u);

    for (uint16_t i = 0u; i < k_expected_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, k_expected_count);
  }
}
