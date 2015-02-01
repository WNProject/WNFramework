// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_READ_WRITE_LOCK_H__
#define __WN_CONCURRENCY_READ_WRITE_LOCK_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include <pthread.h>
#endif

namespace WNConcurrency {
    class WNReadWriteLock {
    public:
        WNReadWriteLock();
        ~WNReadWriteLock();

        WN_VOID LockReader();
        WN_BOOL TryLockReader();
        WN_VOID UnlockReader();

        WN_VOID LockWriter();
        WN_BOOL TryLockWriter();
        WN_VOID UnlockWriter();

    private:
        WNReadWriteLock(const WNReadWriteLock&);
        WNReadWriteLock& operator = (const WNReadWriteLock&);

    private:
        #ifdef _WN_WINDOWS
            SRWLOCK mSlimReadWriteLock;
        #elif defined _WN_POSIX
            pthread_rwlock_t mReadWriteLock;
        #endif
    };
}

#include "WNConcurrency/inc/Internal/WNReadWriteLock.inl"

#endif // __WN_CONCURRENCY_READ_WRITE_LOCK_H__