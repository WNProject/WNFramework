// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_RECURSIVE_MUTEX_INL__
#define __WN_CONCURRENCY_INTERNAL_RECURSIVE_MUTEX_INL__

#ifndef __WN_CONCURRENCY_RECURSIVE_MUTEX_H__
    #error "WNRecursiveMutex.inl should never be included directly. Please include WNRecursiveMutex.h instead"
#endif

#include "WNConcurrency/inc/WNAtomic.h"

#ifdef _WN_POSIX
    #ifdef _WN_RELEASE
        #include "WNCore/inc/WNMacros.h"
    #endif

    #include "WNCore/inc/WNAssert.h"
    #include "WNConcurrency/inc/WNThread.h"

    #define __WN_RECURSIVE_MUTEX_CREATE_ERR_MSG "Error: Failed to create recursive mutex object."
    #define __WN_RECURSIVE_MUTEX_LOCK_ERR_MSG "Error: Failed to lock recursive mutex object."
    #define __WN_RECURSIVE_MUTEX_UNLOCK_ERR_MSG "Error: Failed to unlock recursive mutex object."
    #define __WN_RECURSIVE_MUTEX_DESTROY_WARN_MSG "Warning: Failed to destroy recursive mutex object."
#endif

namespace WNConcurrency {
    WN_FORCE_INLINE WNRecursiveMutex::WNRecursiveMutex(const WN_UINT32 _spinCount) :
        mSpinCount(_spinCount) {
        #ifdef _WN_WINDOWS
            InitializeCriticalSectionAndSpinCount(&mCriticalSection, static_cast<DWORD>(mSpinCount));
        #elif defined _WN_POSIX
            pthread_mutexattr_t attributes;
            int result;

            if ((result = pthread_mutexattr_init(&attributes)) == 0) {
                if ((result = pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE)) == 0) {
                    if ((result = pthread_mutex_init(&mMutex, &attributes)) == 0) {
                        const int temp = pthread_mutexattr_destroy(&attributes);

                        WN_DEBUG_ASSERT_DESC(temp == 0, __WN_RECURSIVE_MUTEX_CREATE_ERR_MSG);

                        #ifdef _WN_RELEASE
                            WN_UNUSED_ARG(temp);
                        #endif
                    }
                }
            }

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_MUTEX_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNRecursiveMutex::~WNRecursiveMutex() {
        #ifdef _WN_WINDOWS
            DeleteCriticalSection(&mCriticalSection);
        #elif defined _WN_POSIX
            const int result = pthread_mutex_destroy(&mMutex);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_RECURSIVE_MUTEX_DESTROY_WARN_MSG);

            #ifdef _WN_RELEASE
                WN_UNUSED_ARG(result);
            #endif
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNRecursiveMutex::Lock() {
        #ifdef _WN_WINDOWS
            EnterCriticalSection(&mCriticalSection);
        #elif defined _WN_POSIX
            for (WN_UINT32 i = 0; i < static_cast<WN_UINT32>(mSpinCount); ++i) {
                if (pthread_mutex_trylock(&mMutex) == 0) {
                    return;
                }

                WNThreadYield();
            }

            const int result = pthread_mutex_lock(&mMutex);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_MUTEX_LOCK_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNRecursiveMutex::TryLock() {
        #ifdef _WN_WINDOWS
            return(TryEnterCriticalSection(&mCriticalSection) != 0 ? WN_TRUE : WN_FALSE);
        #elif defined _WN_POSIX
            return(pthread_mutex_trylock(&mMutex) == 0 ? WN_TRUE : WN_FALSE);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNRecursiveMutex::Unlock() {
        #ifdef _WN_WINDOWS
            LeaveCriticalSection(&mCriticalSection);
        #elif defined _WN_POSIX
            const int result = pthread_mutex_unlock(&mMutex);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_RECURSIVE_MUTEX_UNLOCK_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNRecursiveMutex::SetSpinCount(const WN_UINT32 _spinCount) {
        WNAtomicSwap(&mSpinCount, static_cast<WN_ATOM_T>(_spinCount));

        #ifdef _WN_WINDOWS
            SetCriticalSectionSpinCount(&mCriticalSection, static_cast<DWORD>(mSpinCount));
        #endif
    }

    WN_FORCE_INLINE WN_UINT32 WNRecursiveMutex::GetSpinCount() const {
        return(static_cast<WN_UINT32>(mSpinCount));
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_RECURSIVE_MUTEX_INL__
