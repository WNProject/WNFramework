// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once


#ifndef __WN_CONCURRENCY_CONDITION_VARIABLE_H__
#define __WN_CONCURRENCY_CONDITION_VARIABLE_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include <pthread.h>
#endif

#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNSemaphore.h"

namespace WNConcurrency {
    template <typename Lock>
    class WNConditionVariable {
    public:
        WNConditionVariable();

        WN_VOID Wait(Lock& _lock);

        WN_VOID Signal();
        WN_VOID Broadcast();

    private:
        volatile WN_UINT32 mWaitCount;
        WNSpinLock mSpinLock;
        WNSemaphore mSemaphore;
    };

    class WNRecursiveMutex;

    template <>
    class WNConditionVariable<WNRecursiveMutex> {
    public:
        WNConditionVariable();
        ~WNConditionVariable();

        WN_VOID Wait(WNRecursiveMutex& _lock);

        WN_VOID Signal();
        WN_VOID Broadcast();

    private:
        #ifdef _WN_WINDOWS
            CONDITION_VARIABLE mConditionVariable;
        #elif defined _WN_POSIX
            pthread_cond_t mConditionVariable;
        #endif
    };
}

#include "WNConcurrency/inc/Internal/WNConditionVariable.inl"

#endif // __WN_CONCURRENCY_CONDITION_VARIABLE_H__