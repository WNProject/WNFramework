// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNThreadPool.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/WNCallbackTask.h"

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#endif

#include <vector>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

TEST(WNThreadPoolValidation, Creation) {
  wn::testing::allocator allocator;

  {
    for (uint32_t i = 0; i < 50; ++i) {
      wn::multi_tasking::thread_pool thread_pool(&allocator);

      ASSERT_EQ(thread_pool.initialize(i),
          wn::multi_tasking::thread_pool::result::ok);
    }
  }
}

static void SimpleCallback() {
  return;
}

TEST(WNThreadPoolValidation, CaughtUninitialized) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_NE(thread_pool.enqueue(wn::multi_tasking::make_callback_task<void>(
                  &allocator, &SimpleCallback)),
        wn::multi_tasking::thread_pool::result::ok);
    ASSERT_EQ(
        thread_pool.initialize(10), wn::multi_tasking::thread_pool::result::ok);
    ASSERT_EQ(thread_pool.enqueue(wn::multi_tasking::make_callback_task<void>(
                  &allocator, &SimpleCallback)),
        wn::multi_tasking::thread_pool::result::ok);
  }
}

TEST(WNThreadPoolValidation, CreationMore) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(10), wn::multi_tasking::thread_pool::result::ok);
  }

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(10), wn::multi_tasking::thread_pool::result::ok);
  }

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(10), wn::multi_tasking::thread_pool::result::ok);
  }

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(10), wn::multi_tasking::thread_pool::result::ok);
  }
}

// TODO: remove global state
static std::atomic<size_t> SimpleS1(0);

void SimpleCallback1() {
  SimpleS1++;
}

TEST(WNThreadPoolValidation, SimpleCallback) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(16), wn::multi_tasking::thread_pool::result::ok);

    for (size_t i = 0; i < 10000; ++i) {
      thread_pool.enqueue(wn::multi_tasking::make_callback_task<void>(
          &allocator, &SimpleCallback1));
    }
  }

  ASSERT_EQ(SimpleS1, 10000);
}

// TODO: remove global state
static std::atomic<size_t> SimpleS2Vals[1000];

void SimpleCallback2(uint32_t _val) {
  SimpleS2Vals[_val] = _val;
}

TEST(WNThreadPoolValidation, OneParameterCallback) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(16), wn::multi_tasking::thread_pool::result::ok);

    for (uint32_t i = 0; i < 1000; ++i) {
      thread_pool.enqueue(wn::multi_tasking::make_callback_task<void>(
          &allocator, &SimpleCallback2, i));
    }
  }

  for (size_t i = 0; i < 1000; ++i) {
    ASSERT_EQ(SimpleS2Vals[i], i);
  }
}

uint32_t SimpleCallback3(uint32_t _val) {
  return (_val);
}

TEST(WNThreadPoolValidation, OneParameterReturnCallback) {
  wn::testing::allocator allocator;

  {
    std::vector<wn::multi_tasking::callback_task_ptr<uint32_t>> jobCallbacks;

    {
      wn::multi_tasking::thread_pool thread_pool(&allocator);

      ASSERT_EQ(thread_pool.initialize(16),
          wn::multi_tasking::thread_pool::result::ok);

      for (uint32_t i = 0; i < 1000; ++i) {
        wn::multi_tasking::callback_task_ptr<uint32_t> j =
            wn::multi_tasking::make_callback_task<uint32_t>(
                &allocator, &SimpleCallback3, i);

        jobCallbacks.push_back(j);
        thread_pool.enqueue(j);
      }
    }

    for (size_t i = 0; i < 1000; ++i) {
      uint32_t result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }
  }
}

TEST(WNThreadPoolValidation, JobsGetCleaned) {
  wn::testing::allocator allocator;

  {
    std::vector<wn::multi_tasking::callback_task_ptr<uint32_t>> jobCallbacks;
    wn::multi_tasking::thread_pool thread_pool(&allocator);

    ASSERT_EQ(
        thread_pool.initialize(16), wn::multi_tasking::thread_pool::result::ok);

    for (uint32_t i = 0; i < 1000; ++i) {
      wn::multi_tasking::callback_task_ptr<uint32_t> j(
          wn::multi_tasking::make_callback_task<uint32_t>(
              &allocator, &SimpleCallback3, i));

      jobCallbacks.push_back(j);
      thread_pool.enqueue(j);
    }

    for (uint32_t i = 0; i < 1000; ++i) {
      uint32_t result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }

    for (int32_t i = 1000 - 1; i >= 0; --i) {
      uint32_t result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }

    jobCallbacks.clear();

    for (uint32_t i = 0; i < 1000; ++i) {
      wn::multi_tasking::callback_task_ptr<uint32_t> j(
          wn::multi_tasking::make_callback_task<uint32_t>(
              &allocator, &SimpleCallback3, i));

      jobCallbacks.push_back(j);
      thread_pool.enqueue(j);
    }

    for (int32_t i = 1000 - 1; i >= 0; --i) {
      uint32_t result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }
  }
}
