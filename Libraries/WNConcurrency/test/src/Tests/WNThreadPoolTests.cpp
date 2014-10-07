#include "WNConcurrency/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
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
#endif

TEST(WNThreadPoolValidation, Creation) {
    for(WN_UINT32 i = 0; i < 50; ++i) {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(i), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    }
}

static WN_VOID SimpleCallback() {
    return;
}

TEST(WNThreadPoolValidation, CaughtUninitialized) {
    WNConcurrency::WNThreadPool threadPool;

    ASSERT_NE(threadPool.AddJob(WNConcurrency::WNAllocateCallbackJob(&SimpleCallback)), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    ASSERT_EQ(threadPool.Initialize(10), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    ASSERT_EQ(threadPool.AddJob(WNConcurrency::WNAllocateCallbackJob(&SimpleCallback)), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
}

TEST(WNThreadPoolValidation, CreationMore) {
    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(100), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    }

    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(100), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    }

    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(100), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    }

    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(100), WNConcurrency::WNThreadPoolReturnCode::eWNOK);
    }
}

static WN_ATOM_T SimpleS1 = 0;

WN_VOID SimpleCallback1() {
    WNConcurrency::WNAtomicIncrement(&SimpleS1);
}

TEST(WNThreadPoolValidation, SimpleCallback) {
    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(16), WNConcurrency::WNThreadPoolReturnCode::eWNOK);

        for (WN_SIZE_T i = 0; i < 10000; ++i) {
            threadPool.AddJob(WNConcurrency::WNAllocateCallbackJob(&SimpleCallback1));
        }
    }

    ASSERT_EQ(SimpleS1, 10000);
}

static WN_ATOM_T SimpleS2Vals[1000];

WN_VOID SimpleCallback2(WN_UINT32 _val) {
    WN_ATOM_T val = _val;

    WNConcurrency::WNAtomicSwap(&SimpleS2Vals[_val], val);
}

TEST(WNThreadPoolValidation, OneParameterCallback) {
    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(16), WNConcurrency::WNThreadPoolReturnCode::eWNOK);

        for (WN_UINT32 i = 0; i < 1000; ++i) {
            threadPool.AddJob(WNConcurrency::WNAllocateCallbackJob(&SimpleCallback2, i));
        }
    }

    for (WN_SIZE_T i = 0; i < 1000; ++i) {
        ASSERT_EQ(SimpleS2Vals[i], i);
    }
}

WN_UINT32 SimpleCallback3(WN_UINT32 _val) {
    return(_val);
}

CREATE_CBJOB_TYPEDEF1(SimpleCallbackJob3, WN_UINT32, WN_UINT32);

TEST(WNThreadPoolValidation, OneParameterReturnCallback) {
    std::vector<SimpleCallbackJob3> jobCallbacks;

    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(16), WNConcurrency::WNThreadPoolReturnCode::eWNOK);

        for (WN_UINT32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = WNConcurrency::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            threadPool.AddJob(j);
        }
    }

    for (WN_SIZE_T i = 0; i < 1000; ++i) {
        ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
    }
}


TEST(WNThreadPoolValidation, JobsGetCleaned) {
    std::vector<SimpleCallbackJob3> jobCallbacks;

    {
        WNConcurrency::WNThreadPool threadPool;

        ASSERT_EQ(threadPool.Initialize(16), WNConcurrency::WNThreadPoolReturnCode::eWNOK);

        for (WN_UINT32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = WNConcurrency::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            threadPool.AddJob(j);
        }

        for (WN_UINT32 i = 0; i < 1000; ++i) {
            jobCallbacks[i]->WaitForCompletion();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }

        for (WN_INT32 i = 1000 - 1; i >= 0; --i) {
            jobCallbacks[i]->WaitForCompletion();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }

        jobCallbacks.clear();

        for (WN_UINT32 i = 0; i < 1000; ++i) {
            SimpleCallbackJob3 j = WNConcurrency::WNAllocateCallbackJob(&SimpleCallback3, i);

            jobCallbacks.push_back(j);
            threadPool.AddJob(j);
        }

        for (WN_INT32 i = 1000 - 1; i >= 0; --i) {
            jobCallbacks[i]->WaitForCompletion();

            ASSERT_EQ(jobCallbacks[i]->GetResult(), i);
        }
    }
}
