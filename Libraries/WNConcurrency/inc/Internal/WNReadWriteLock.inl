// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_READ_WRITE_LOCK_INL__
#define __WN_CONCURRENCY_READ_WRITE_LOCK_INL__

#ifndef __WN_CONCURRENCY_READ_WRITE_LOCK_H__
    #error "WNReadWriteLock.inl should never be included directly. Please include WNReadWriteLock.h instead"
#endif

#ifdef _WN_POSIX
    #include "WNCore/inc/WNAssert.h"

    #define __WN_READ_WRITE_LOCK_CREATE_ERR_MSG "Error: Failed to create read write lock object."
    #define __WN_READ_WRITE_LOCK_LOCK_READER_ERR_MSG "Error: Failed to lock read write lock object for reader."
    #define __WN_READ_WRITE_LOCK_LOCK_WRITER_ERR_MSG "Error: Failed to lock read write lock object for writer."
    #define __WN_READ_WRITE_LOCK_UNLOCK_READER_ERR_MSG "Error: Failed to unlock read write lock object for reader."
    #define __WN_READ_WRITE_LOCK_UNLOCK_WRITER_ERR_MSG "Error: Failed to unlock read write lock object for writer."
    #define __WN_READ_WRITE_LOCK_DESTROY_WARN_MSG "Warning: Failed to destroy read write lock object."
#endif

namespace WNConcurrency {
    WN_FORCE_INLINE WNReadWriteLock::WNReadWriteLock() {
        #ifdef _WN_WINDOWS
            InitializeSRWLock(&mSlimReadWriteLock);
        #elif defined _WN_POSIX
            const int result = pthread_rwlock_init(&mReadWriteLock, NULL);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNReadWriteLock::~WNReadWriteLock() {
        #ifdef _WN_POSIX
            const int result = pthread_rwlock_destroy(&mReadWriteLock);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_DESTROY_WARN_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNReadWriteLock::LockReader() {
        #ifdef _WN_WINDOWS
            AcquireSRWLockShared(&mSlimReadWriteLock);
        #elif defined _WN_POSIX
            const int result = pthread_rwlock_rdlock(&mReadWriteLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_LOCK_READER_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNReadWriteLock::TryLockReader() {
        #ifdef _WN_WINDOWS
            return(TryAcquireSRWLockShared(&mSlimReadWriteLock) == TRUE);
        #elif defined _WN_POSIX
            return(pthread_rwlock_tryrdlock(&mReadWriteLock) == 0 ? WN_TRUE : WN_FALSE);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNReadWriteLock::UnlockReader() {
        #ifdef _WN_WINDOWS
            ReleaseSRWLockShared(&mSlimReadWriteLock);
        #elif defined _WN_POSIX
            const int result = pthread_rwlock_unlock(&mReadWriteLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_UNLOCK_READER_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNReadWriteLock::LockWriter() {
        #ifdef _WN_WINDOWS
            AcquireSRWLockExclusive(&mSlimReadWriteLock);
        #elif defined _WN_POSIX
            const int result = pthread_rwlock_wrlock(&mReadWriteLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_LOCK_WRITER_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNReadWriteLock::TryLockWriter() {
        #ifdef _WN_WINDOWS
            return(TryAcquireSRWLockExclusive(&mSlimReadWriteLock) == TRUE);
        #elif defined _WN_POSIX
            return(pthread_rwlock_trywrlock(&mReadWriteLock) == 0 ? WN_TRUE : WN_FALSE);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNReadWriteLock::UnlockWriter() {
        #ifdef _WN_WINDOWS
            ReleaseSRWLockExclusive(&mSlimReadWriteLock);
        #elif defined _WN_POSIX
            const int result = pthread_rwlock_unlock(&mReadWriteLock);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_READ_WRITE_LOCK_UNLOCK_WRITER_ERR_MSG);
        #endif
    }
}

#endif // __WN_CONCURRENCY_READ_WRITE_LOCK_INL__