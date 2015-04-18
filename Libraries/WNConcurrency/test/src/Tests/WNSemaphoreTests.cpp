// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNConcurrency/test/inc/Common.h"

#include <memory>

TEST(semaphore, wait_notify) {
    std::string numbers;
    wn_uint32 count = 0;
    wn_bool start = wn_false;
    wn::semaphore semaphore;

    const auto thread_function = [&numbers, &count, &semaphore, &start]() -> wn_void {
        semaphore.wait();

        ASSERT_TRUE(start);

        numbers += std::to_string(count);

        count++;

        semaphore.notify();
    };

    std::vector<std::shared_ptr<wn::thread<wn_void>>> threads;

    for (auto i = 0; i < 10; ++i) {
        threads.push_back(std::make_shared<wn::thread<wn_void>>(thread_function));
    }

    wn::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(count, 0);

    start = wn_true;

    wn::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(count, 0);

    semaphore.notify();

    for (auto i = 0; i < 10; ++i) {
        threads[i]->join();
    }

    ASSERT_EQ(count, 10);
    ASSERT_EQ(numbers, "0123456789");
}

TEST(semaphore, try_wait) {
    wn::semaphore semaphore;

    ASSERT_FALSE(semaphore.try_wait());

    semaphore.notify();

    const auto thread_function = [&semaphore]() -> wn_void {
        ASSERT_TRUE(semaphore.try_wait());
    };

    wn::thread<wn_void> thread(thread_function);

    thread.join();
}

TEST(semaphore, initial_count) {
    std::atomic_int count = { 0 };
    wn::semaphore semaphore(10);

    const auto thread_function = [&count, &semaphore]() -> wn_void {
        semaphore.wait();

        count++;
    };

    std::vector<std::shared_ptr<wn::thread<wn_void>>> threads;

    for (auto i = 0; i < 20; ++i) {
        threads.push_back(std::make_shared<wn::thread<wn_void>>(thread_function));
    }

    wn::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(count, 10);

    semaphore.notify(5);

    wn::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(count, 15);

    semaphore.notify(5);

    for (auto i = 0; i < 20; ++i) {
        threads[i]->join();
    }

    ASSERT_EQ(count, 20);
}