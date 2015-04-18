// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNConcurrency/test/inc/Common.h"
#include "WNConcurrency/inc/WNThreadPool.h"
#include "WNConcurrency/inc/WNCallbackJob.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
#endif

#include <vector>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

TEST(WNThreadPoolValidation, Creation) {
    for(wn_uint32 i = 0; i < 50; ++i) {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(i), wn::thread_pool::result::ok);
    }
}

static wn_void SimpleCallback() {
    return;
}

TEST(WNThreadPoolValidation, CaughtUninitialized) {
    wn::thread_pool thread_pool;

    ASSERT_NE(thread_pool.enqueue(wn::WNAllocateCallbackJob(&SimpleCallback)), wn::thread_pool::result::ok);
    ASSERT_EQ(thread_pool.initialize(10), wn::thread_pool::result::ok);
    ASSERT_EQ(thread_pool.enqueue(wn::WNAllocateCallbackJob(&SimpleCallback)), wn::thread_pool::result::ok);
}

TEST(WNThreadPoolValidation, CreationMore) {
    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(100), wn::thread_pool::result::ok);
    }

    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(100), wn::thread_pool::result::ok);
    }

    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(100), wn::thread_pool::result::ok);
    }

    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(100), wn::thread_pool::result::ok);
    }
}

static std::atomic<wn_size_t> SimpleS1(0);

wn_void SimpleCallback1() {
    SimpleS1++;
}

TEST(WNThreadPoolValidation, SimpleCallback) {
    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(16), wn::thread_pool::result::ok);

        for (wn_size_t i = 0; i < 10000; ++i) {
            thread_pool.enqueue(wn::WNAllocateCallbackJob(&SimpleCallback1));
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
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(16), wn::thread_pool::result::ok);

        for (wn_uint32 i = 0; i < 1000; ++i) {
            thread_pool.enqueue(wn::WNAllocateCallbackJob(&SimpleCallback2, i));
        }
    }

    for (wn_size_t i = 0; i < 1000; ++i) {
        ASSERT_EQ(SimpleS2Vals[i], i);
    }
}

wn_uint32 SimpleCallback3(wn_uint32 _val) {
    return(_val);
}

CREATE_CBJOB_TYPEDEF1(SimpleCallbackJob3, wn_uint32, wn_uint32);

TEST(WNThreadPoolValidation, OneParameterReturnCallback) {
    std::vector<SimpleCallbackJob3> jobCallbacks;

    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(16), wn::thread_pool::result::ok);

        for (wn_uint32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = wn::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            thread_pool.enqueue(j);
        }
    }

    for (wn_size_t i = 0; i < 1000; ++i) {
        ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
    }
}


TEST(WNThreadPoolValidation, JobsGetCleaned) {
    std::vector<SimpleCallbackJob3> jobCallbacks;

    {
        wn::thread_pool thread_pool;

        ASSERT_EQ(thread_pool.initialize(16), wn::thread_pool::result::ok);

        for (wn_uint32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = wn::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            thread_pool.enqueue(j);
        }

        for (wn_uint32 i = 0; i < 1000; ++i) {
            jobCallbacks[i]->join();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }

        for (wn_int32 i = 1000 - 1; i >= 0; --i) {
            jobCallbacks[i]->join();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }

        jobCallbacks.clear();

        for (wn_uint32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = wn::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            thread_pool.enqueue(j);
        }

        for (wn_int32 i = 1000 - 1; i >= 0; --i) {
            jobCallbacks[i]->join();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }
    }
}
