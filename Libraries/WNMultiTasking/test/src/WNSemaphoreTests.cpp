// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNMultiTasking/inc/WNThread.h"

#include <memory>
#include <sstream>

TEST(semaphore, wait_notify) {
  wn::testing::allocator allocator;

  {
    std::stringstream numbers;
    wn_uint32 count = 0;
    wn::multi_tasking::semaphore semaphore;

    const auto thread_function = [&numbers, &count, &semaphore]() -> wn_void {
      semaphore.wait();

      numbers << count;

      count++;

      semaphore.notify();
    };

    std::vector<std::shared_ptr<wn::multi_tasking::thread<wn_void>>> threads;

    for (auto i = 0; i < 10; ++i) {
      threads.push_back(std::make_shared<wn::multi_tasking::thread<wn_void>>(
          &allocator, thread_function));
    }

    ASSERT_EQ(count, 0);

    semaphore.notify();

    for (auto i = 0; i < 10; ++i) {
      threads[i]->join();
    }

    ASSERT_EQ(count, 10);
    ASSERT_EQ(numbers.str(), "0123456789");
  }

  EXPECT_TRUE(allocator.empty());
}

TEST(semaphore, try_wait) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::semaphore semaphore;

    ASSERT_FALSE(semaphore.try_wait());

    semaphore.notify();

    wn_bool result = wn_false;

    const auto thread_function = [&semaphore, &result]() -> wn_void {
      result = semaphore.try_wait();
    };

    wn::multi_tasking::thread<wn_void> thread(&allocator, thread_function);

    thread.join();

    ASSERT_TRUE(result);
  }

  EXPECT_TRUE(allocator.empty());
}

TEST(semaphore, initial_count) {
  wn::testing::allocator allocator;

  {
    std::atomic_int count = {0};
    wn::multi_tasking::semaphore semaphore(10);

    const auto thread_function = [&count, &semaphore]() -> wn_void {
      semaphore.wait();

      count++;
    };

    std::vector<std::shared_ptr<wn::multi_tasking::thread<wn_void>>> threads;

    for (auto i = 0; i < 15; ++i) {
      threads.push_back(std::make_shared<wn::multi_tasking::thread<wn_void>>(
          &allocator, thread_function));
    }

    semaphore.notify(5);

    for (auto i = 0; i < 15; ++i) {
      threads[i]->join();
    }

    ASSERT_EQ(count, 15);
  }

  EXPECT_TRUE(allocator.empty());
}
