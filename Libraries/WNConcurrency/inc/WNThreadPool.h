// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_POOL_H__
#define __WN_CONCURRENCY_THREAD_POOL_H__

#include "WNConcurrency/inc/WNThreadJob.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
    #pragma warning(disable: 4996)
#endif

#include <vector>
#include <queue>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNConcurrency {
    template <typename Return>
    class WNThread;

    namespace WNThreadPoolReturnCode {
        enum Type {
            #include "WNCore/inc/Internal/WNErrors.inc"
        };
    }

    class WNThreadPool {
    public:
        WNThreadPool();
        ~WNThreadPool();

        WNThreadPoolReturnCode::Type Initialize(const WN_UINT32 _numWorkers);
        WNThreadPoolReturnCode::Type AddJob(const WNResourcePointer<WNThreadJob>& _job);

    private:
        WN_VOID Cleanup();
        WN_VOID JobRunnerThread(const WN_UINT32 _threadNum);

        WNThreadPool(const WNThreadPool&);
        WNThreadPool& operator = (const WNThreadPool&);

    private:
       #ifdef _WN_WINDOWS
            HANDLE mIOCPHandle;
            std::vector<WNThread<WN_VOID>*> mThreads;
            std::deque<WN_SIZE_T> mThreadFreelist;
            WNSpinLock mThreadMutex;
            WNSemaphore mThreadStartMutex;
            WN_UINT32 mNumThreads;
            WN_BOOL mShutdown;
        #else
            WN_BOOL mShutdown;
            std::vector<WNThread<WN_VOID>*> mThreads;
            WNSemaphore mThreadStartMutex;
            WNSemaphore mJobsAvailable;
            WNSpinLock  mJobMutex;
            std::deque<WNResourcePointer<WNThreadJob> > mThreadJobs;
        #endif
    };
}
#ifdef _WN_WINDOWS
#include "WNConcurrency/inc/Internal/WNThreadPoolWindows.inl"
#else
#include "WNConcurrency/inc/Internal/WNThreadPoolLinux.inl"
#endif
#endif // __WN_CONCURRENCY_THREAD_POOL_H__

