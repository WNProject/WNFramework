// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_MUTEX_INL__
#define __WN_CONCURRENCY_INTERNAL_MUTEX_INL__

#ifndef __WN_CONCURRENCY_MUTEX_H__
    #error "WNMutex.inl should never be included directly. Please include WNMutex.h instead"
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNConcurrency/inc/WNThread.h"

#ifdef _WN_RELEASE
    #include "WNCore/inc/WNMacros.h"
#endif

#define __WN_MUTEX_CREATE_ERR_MSG "Error: Failed to create mutex object."
#define __WN_MUTEX_LOCK_ERR_MSG "Error: Failed to lock mutex object."
#define __WN_MUTEX_UNLOCK_ERR_MSG "Error: Failed to unlock mutex object."
#define __WN_MUTEX_DESTROY_WARN_MSG "Warning: Failed to destroy mutex object."

namespace WNConcurrency {
    WN_FORCE_INLINE WNMutex::WNMutex(const WN_UINT32 _spinCount) :
        mSpinCount(_spinCount),
        mLockCount(0) {
        #ifdef _WN_WINDOWS
            mSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

            WN_RELEASE_ASSERT_DESC(mSemaphore != NULL, __WN_MUTEX_CREATE_ERR_MSG);
        #elif defined _WN_POSIX
            pthread_mutexattr_t attributes;
            int result;

            if ((result = pthread_mutexattr_init(&attributes)) == 0) {
                if ((result = pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_NORMAL)) == 0) {
                    if ((result = pthread_mutex_init(&mMutex, &attributes)) == 0) {
                        const int temp = pthread_mutexattr_destroy(&attributes);

                        WN_DEBUG_ASSERT_DESC(temp == 0, __WN_MUTEX_CREATE_ERR_MSG);

                        #ifdef _WN_RELEASE
                            WN_UNUSED_ARG(temp);
                        #endif
                    }
                }
            }

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_MUTEX_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNMutex::~WNMutex() {
        #ifdef _WN_WINDOWS
            const BOOL result = CloseHandle(mSemaphore);

            WN_DEBUG_ASSERT_DESC(result != FALSE, __WN_MUTEX_DESTROY_WARN_MSG);
        #elif defined _WN_POSIX
            const int result = pthread_mutex_destroy(&mMutex);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_MUTEX_DESTROY_WARN_MSG);
        #endif

        #ifdef _WN_RELEASE
            WN_UNUSED_ARG(result);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNMutex::Lock() {
        for (WN_UINT32 i = 0; i < static_cast<WN_UINT32>(mSpinCount); ++i) {
            if (WNAtomicCompareSwap(&mLockCount, 1, 0) == 0) {
                return;
            }

            WNThreadYield();
        }

        if (WNAtomicIncrement(&mLockCount) > 1) {
            #ifdef _WN_WINDOWS
                const DWORD result = WaitForSingleObject(mSemaphore, INFINITE);

                WN_RELEASE_ASSERT_DESC(result == WAIT_OBJECT_0, __WN_MUTEX_LOCK_ERR_MSG);
            #elif defined _WN_POSIX
                const int result = pthread_mutex_lock(&mMutex);

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_MUTEX_LOCK_ERR_MSG);
            #endif
        }
    }

    WN_FORCE_INLINE WN_BOOL WNMutex::TryLock() {
        return(WNAtomicCompareSwap(&mLockCount, 1, 0) == 0 ? WN_TRUE : WN_FALSE);
    }

    WN_FORCE_INLINE WN_VOID WNMutex::Unlock() {
        if (WNAtomicDecrement(&mLockCount) > 0) {
            #ifdef _WN_WINDOWS
                const BOOL result = ReleaseSemaphore(mSemaphore, 1, NULL);

                WN_RELEASE_ASSERT_DESC(result != FALSE, __WN_MUTEX_UNLOCK_ERR_MSG);
            #elif defined _WN_POSIX
                const int result = pthread_mutex_unlock(&mMutex);

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_MUTEX_UNLOCK_ERR_MSG);
            #endif
        }
    }

    WN_FORCE_INLINE WN_VOID WNMutex::SetSpinCount(const WN_UINT32 _spinCount) {
        WNAtomicSwap(&mSpinCount, static_cast<WN_ATOM_T>(_spinCount));
    }

    WN_FORCE_INLINE WN_UINT32 WNMutex::GetSpinCount() const {
        return(static_cast<WN_UINT32>(mSpinCount));
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_MUTEX_INL__
