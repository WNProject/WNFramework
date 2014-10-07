////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_SPIN_LOCK_INL__
#define __WN_CONCURRENCY_SPIN_LOCK_INL__

#ifndef __WN_CONCURRENCY_SPIN_LOCK_H__
    #error "WNSpinLock.inl should never be included directly. Please include WNSpinLock.h instead"
#endif

#ifdef __WN_PTHREAD_SPIN_LOCKS
    #include "WNCore/inc/WNAssert.h"

    #include <errno.h>

    #define __WN_SPIN_LOCK_CREATE_ERR_MSG "Error: Failed to create spin lock object."
    #define __WN_SPIN_LOCK_LOCK_ERR_MSG "Error: Failed to lock spin lock object."
    #define __WN_SPIN_LOCK_UNLOCK_ERR_MSG "Error: Failed to unlock spin lock object."
    #define __WN_SPIN_LOCK_DESTROY_WARN_MSG "Warning: Failed to destroy spin lock object."
#else
    #include "WNConcurrency/inc/WNAtomic.h"
    #include "WNConcurrency/inc/WNThread.h"
#endif

namespace WNConcurrency {
    WN_FORCE_INLINE WNSpinLock::WNSpinLock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_init(&mSpinLock, PTHREAD_PROCESS_PRIVATE);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SPIN_LOCK_CREATE_ERR_MSG);
        #else
            mSpinFlag = 0;
        #endif
    }

    WN_FORCE_INLINE WNSpinLock::~WNSpinLock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_destroy(&mSpinLock);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_SPIN_LOCK_DESTROY_WARN_MSG);

            #ifdef _WN_RELEASE
                WN_UNUSED_ARG(result);
            #endif
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNSpinLock::Lock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_lock(&mSpinLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SPIN_LOCK_LOCK_ERR_MSG);
        #else
            while (WNAtomicCompareSwap(&mSpinFlag, 1, 0) != 0) {
                WNThreadYield();
            }
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNSpinLock::TryLock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            return(pthread_spin_trylock(&mSpinLock) != EBUSY ? WN_TRUE : WN_FALSE);
        #else
            return(WNAtomicCompareSwap(&mSpinFlag, 1, 0) == 0 ? WN_TRUE : WN_FALSE);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNSpinLock::Unlock() {
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            const int result = pthread_spin_unlock(&mSpinLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SPIN_LOCK_UNLOCK_ERR_MSG);
        #else
            WNAtomicSwap(&mSpinFlag, 0);
        #endif
    }
}

#endif // __WN_CONCURRENCY_SPIN_LOCK_INL__