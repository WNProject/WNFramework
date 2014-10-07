////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_CONDITION_VARIABLE_INL__
#define __WN_CONCURRENCY_INTERNAL_CONDITION_VARIABLE_INL__

#include "WNConcurrency/inc/WNRecursiveMutex.h"

#if defined _WN_POSIX
    #ifdef _WN_RELEASE
        #include "WNCore/inc/WNMacros.h"
    #endif

    #include "WNConcurrency/inc/WNMutex.h"

    #define __WN_CONDITION_VARIABLE_CREATE_ERR_MSG "Error: Failed to create condition variable object."
    #define __WN_CONDITION_VARIABLE_WAIT_ERR_MSG "Error: Failed to wait on condition variable object."
    #define __WN_CONDITION_VARIABLE_SIGNAL_ERR_MSG "Error: Failed to signal condition variable object."
    #define __WN_CONDITION_VARIABLE_BROADCAST_ERR_MSG "Error: Failed to broadcast condition variable object."
    #define __WN_CONDITION_VARIABLE_DESTROY_WARN_MSG "Warning: Failed to destroy condition variable object."
#endif

namespace WNConcurrency {
    template <typename Lock>
    WN_FORCE_INLINE WNConditionVariable<Lock>::WNConditionVariable() :
        mWaitCount(0) {
    }

    template <typename Lock>
    WN_FORCE_INLINE WN_VOID WNConditionVariable<Lock>::Wait(Lock& _lock) {
        mSpinLock.Lock();

        mWaitCount++;

        mSpinLock.Unlock();
        _lock.Unlock();

        mSemaphore.Wait();
    }

    template <typename Lock>
    WN_FORCE_INLINE WN_VOID WNConditionVariable<Lock>::Signal() {
        mSpinLock.Lock();

        if (mWaitCount > 0) {
            mWaitCount--;

            mSemaphore.Post();
        }

        mSpinLock.Unlock();
    }

    template <typename Lock>
    WN_FORCE_INLINE WN_VOID WNConditionVariable<Lock>::Broadcast() {
        mSpinLock.Lock();

        if (mWaitCount > 0) {
            const WN_UINT32 waitCount = mWaitCount;

            mWaitCount = 0;

            mSemaphore.Post(waitCount);
        }

        mSpinLock.Unlock();
    }

    WN_FORCE_INLINE WNConditionVariable<WNRecursiveMutex>::WNConditionVariable() {
        #ifdef _WN_WINDOWS
            InitializeConditionVariable(&mConditionVariable);
        #elif defined _WN_POSIX
            const int result = pthread_cond_init(&mConditionVariable, NULL);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_CONDITION_VARIABLE_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNConditionVariable<WNRecursiveMutex>::~WNConditionVariable() {
        #ifdef _WN_POSIX
            const int result = pthread_cond_destroy(&mConditionVariable);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_CONDITION_VARIABLE_DESTROY_WARN_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNConditionVariable<WNRecursiveMutex>::Wait(WNRecursiveMutex& _mutex) {
        #ifdef _WN_WINDOWS
            SleepConditionVariableCS(&mConditionVariable, &(_mutex.mCriticalSection), INFINITE);
        #elif defined _WN_POSIX
            const int result = pthread_cond_wait(&mConditionVariable, &(_mutex.mMutex));

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_CONDITION_VARIABLE_WAIT_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNConditionVariable<WNRecursiveMutex>::Signal()  {
        #ifdef _WN_WINDOWS
            WakeConditionVariable(&mConditionVariable);
        #elif defined _WN_POSIX
            const int result = pthread_cond_signal(&mConditionVariable);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_CONDITION_VARIABLE_SIGNAL_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNConditionVariable<WNRecursiveMutex>::Broadcast() {
        #ifdef _WN_WINDOWS
            WakeAllConditionVariable(&mConditionVariable);
        #elif defined _WN_POSIX
            const int result = pthread_cond_broadcast(&mConditionVariable);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_CONDITION_VARIABLE_BROADCAST_ERR_MSG);
        #endif
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_CONDITION_VARIABLE_INL__