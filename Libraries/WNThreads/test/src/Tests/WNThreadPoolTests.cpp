// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNThreads/inc/WNThreadPool.h"
#include "WNThreads/inc/WNCallbackTask.h"

#include "WNTesting/inc/WNTestHarness.h"

#ifdef _WN_MSVC
  #pragma warning(push)
  #pragma warning(disable: 4275)
#endif

#include <vector>

#ifdef _WN_MSVC
  #pragma warning(pop)
#endif

TEST(WNThreadPoolValidation, Creation) {
  for (wn_uint32 i = 0; i < 50; ++i) {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(i), wn::threads::thread_pool::result::ok);
  }
}

static wn_void SimpleCallback() {
  return;
}

TEST(WNThreadPoolValidation, CaughtUninitialized) {
  wn::threads::thread_pool thread_pool;

  ASSERT_NE(thread_pool.enqueue(wn::threads::make_callback_task<wn_void>(&SimpleCallback)), wn::threads::thread_pool::result::ok);
  ASSERT_EQ(thread_pool.initialize(10), wn::threads::thread_pool::result::ok);
  ASSERT_EQ(thread_pool.enqueue(wn::threads::make_callback_task<wn_void>(&SimpleCallback)), wn::threads::thread_pool::result::ok);
}

TEST(WNThreadPoolValidation, CreationMore) {
  {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(100), wn::threads::thread_pool::result::ok);
  }

    {
      wn::threads::thread_pool thread_pool;

      ASSERT_EQ(thread_pool.initialize(100), wn::threads::thread_pool::result::ok);
    }

    {
      wn::threads::thread_pool thread_pool;

      ASSERT_EQ(thread_pool.initialize(100), wn::threads::thread_pool::result::ok);
    }

    {
      wn::threads::thread_pool thread_pool;

      ASSERT_EQ(thread_pool.initialize(100), wn::threads::thread_pool::result::ok);
    }
}

static std::atomic<wn_size_t> SimpleS1(0);

wn_void SimpleCallback1() {
  SimpleS1++;
}

TEST(WNThreadPoolValidation, SimpleCallback) {
  {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(16), wn::threads::thread_pool::result::ok);

    for (wn_size_t i = 0; i < 10000; ++i) {
      thread_pool.enqueue(wn::threads::make_callback_task<wn_void>(&SimpleCallback1));
    }
  }

  ASSERT_EQ(SimpleS1, 10000);
}

static std::atomic<wn_size_t> SimpleS2Vals[1000];

wn_void SimpleCallback2(wn_uint32 _val) {
  wn_atom_t val = _val;

  SimpleS2Vals[_val] = val;
}

TEST(WNThreadPoolValidation, OneParameterCallback) {
  {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(16), wn::threads::thread_pool::result::ok);

    for (wn_uint32 i = 0; i < 1000; ++i) {
      thread_pool.enqueue(wn::threads::make_callback_task<wn_void>(&SimpleCallback2, i));
    }
  }

  for (wn_size_t i = 0; i < 1000; ++i) {
    ASSERT_EQ(SimpleS2Vals[i], i);
  }
}

wn_uint32 SimpleCallback3(wn_uint32 _val) {
  return(_val);
}

TEST(WNThreadPoolValidation, OneParameterReturnCallback) {
  std::vector<wn::threads::callback_task_ptr<wn_uint32>> jobCallbacks;

  {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(16), wn::threads::thread_pool::result::ok);

    for (wn_uint32 i = 0; i < 1000; ++i) {
      wn::threads::callback_task_ptr<wn_uint32> j = wn::threads::make_callback_task<wn_uint32>(&SimpleCallback3, i);

      jobCallbacks.push_back(j);
      thread_pool.enqueue(j);
    }
  }

  for (wn_size_t i = 0; i < 1000; ++i) {
    wn_uint32 result;

    jobCallbacks[i]->join(result);

    ASSERT_EQ(result, i);
  }
}

TEST(WNThreadPoolValidation, JobsGetCleaned) {
  std::vector<wn::threads::callback_task_ptr<wn_uint32>> jobCallbacks;

  {
    wn::threads::thread_pool thread_pool;

    ASSERT_EQ(thread_pool.initialize(16), wn::threads::thread_pool::result::ok);

    for (wn_uint32 i = 0; i < 1000; ++i) {
      wn::threads::callback_task_ptr<wn_uint32> j = wn::threads::make_callback_task<wn_uint32>(&SimpleCallback3, i);

      jobCallbacks.push_back(j);
      thread_pool.enqueue(j);
    }

    for (wn_uint32 i = 0; i < 1000; ++i) {
      wn_uint32 result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }

    for (wn_int32 i = 1000 - 1; i >= 0; --i) {
      wn_uint32 result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }

    jobCallbacks.clear();

    for (wn_uint32 i = 0; i < 1000; ++i) {
      wn::threads::callback_task_ptr<wn_uint32> j = wn::threads::make_callback_task<wn_uint32>(&SimpleCallback3, i);

      jobCallbacks.push_back(j);
      thread_pool.enqueue(j);
    }

    for (wn_int32 i = 1000 - 1; i >= 0; --i) {
      wn_uint32 result;

      jobCallbacks[i]->join(result);

      ASSERT_EQ(result, i);
    }
  }
}
