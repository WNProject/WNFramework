// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_MUTEX_H__
#define __WN_CONCURRENCY_MUTEX_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include <pthread.h>
#endif

namespace WNConcurrency {
    class WNMutex {
    public:
        WNMutex(const WN_UINT32 _spinCount = 4000);
        ~WNMutex();

        WN_VOID Lock();
        WN_BOOL TryLock();
        WN_VOID Unlock();

        WN_VOID SetSpinCount(const WN_UINT32 _spinCount);
        WN_UINT32 GetSpinCount() const;

    private:
        WNMutex(const WNMutex&);
        WNMutex& operator = (const WNMutex&);

    private:
        template <typename Lock>
        friend class WNConditionVariable;

    private:
        #ifdef _WN_WINDOWS
            HANDLE mSemaphore;
        #elif defined _WN_POSIX
            pthread_mutex_t mMutex;
        #endif

        WN_ATOM_T mSpinCount;
        WN_ATOM_T mLockCount;
    };
}

#include "WNConcurrency/inc/Internal/WNMutex.inl"

#endif // __WN_CONCURRENCY_MUTEX_H__