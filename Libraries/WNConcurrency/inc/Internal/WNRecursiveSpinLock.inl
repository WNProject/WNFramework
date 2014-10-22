// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_INL__
#define __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_INL__

#ifndef __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_H__
    #error "WNRecursiveSpinLock.inl should never be included directly. Please include WNRecursiveSpinLock.h instead"
#endif

#ifdef _WN_LINUX
    #include <sys/types.h>
    #include <sys/syscall.h>
#endif

#ifdef __WN_PTHREAD_SPIN_LOCKS
    #include "WNCore/inc/WNAssert.h"

    #define __WN_RECURSIVE_SPIN_LOCK_CREATE_ERR_MSG "Error: Failed to create spin lock object."
    #define __WN_RECURSIVE_SPIN_LOCK_LOCK_ERR_MSG "Error: Failed to lock spin lock object."
    #define __WN_RECURSIVE_SPIN_LOCK_UNLOCK_ERR_MSG "Error: Failed to unlock spin lock object."
    #define __WN_RECURSIVE_SPIN_LOCK_DESTROY_WARN_MSG "Warning: Failed to destroy spin lock object."
#else
    #include "WNConcurrency/inc/WNAtomic.h"
    #include "WNConcurrency/inc/WNThread.h"
#endif

namespace WNConcurrency {
    WN_FORCE_INLINE WNRecursiveSpinLock::WNRecursiveSpinLock() :
        #ifndef __WN_PTHREAD_SPIN_LOCKS
            mSpinFlag(0),
        #endif
        mRecursionCount(0),
        mOwningThread(0) {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_init(&mSpinLock, PTHREAD_PROCESS_PRIVATE);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_SPIN_LOCK_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNRecursiveSpinLock::~WNRecursiveSpinLock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_destroy(&mSpinLock);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_RECURSIVE_SPIN_LOCK_DESTROY_WARN_MSG);

            #ifdef _WN_RELEASE
                WN_UNUSED_ARG(result);
            #endif
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNRecursiveSpinLock::Lock() {
        #ifdef _WN_WINDOWS
            const DWORD currentThread = GetCurrentThreadId();
        #elif defined _WN_LINUX
            const pid_t currentThread = static_cast<pid_t>(syscall(SYS_gettid));
        #endif

        if (currentThread != mOwningThread) {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result = pthread_spin_lock(&mSpinLock);

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_SPIN_LOCK_LOCK_ERR_MSG);
            #else
                while (WNAtomicCompareSwap(&mSpinFlag, 1, 0) != 0) {
                    WNThreadYield();
                }
            #endif
        }

        mOwningThread = currentThread;
        mRecursionCount++;
    }

    WN_FORCE_INLINE WN_BOOL WNRecursiveSpinLock::TryLock() {
        #ifdef _WN_WINDOWS
            const DWORD currentThread = GetCurrentThreadId();
        #elif defined _WN_LINUX
            const pid_t currentThread = static_cast<pid_t>(syscall(SYS_gettid));
        #endif

        if (mOwningThread != currentThread) {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                if (pthread_spin_trylock(&mSpinLock) == EBUSY) {
                    return(WN_FALSE);
                }
            #else
                if (WNAtomicCompareSwap(&mSpinFlag, 1, 0) != 0) {
                    return(WN_FALSE);
                }
            #endif

            mOwningThread = currentThread;
        }

        mRecursionCount++;

        return(WN_TRUE);
    }

    WN_FORCE_INLINE WN_VOID WNRecursiveSpinLock::Unlock() {
        const WN_UINT32 currentRecursionCount = --mRecursionCount;

        if (currentRecursionCount == 0) {
            mOwningThread = 0;

            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result = pthread_spin_unlock(&mSpinLock);

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_SPIN_LOCK_UNLOCK_ERR_MSG);
            #else
                WNAtomicSwap(&mSpinFlag, 0);
            #endif
        }
    }
}

#endif // __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_INL__