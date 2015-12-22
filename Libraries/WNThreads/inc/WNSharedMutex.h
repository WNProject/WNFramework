// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONCURRENCY_SHARED_MUTEX_H__
#define __WN_CONCURRENCY_SHARED_MUTEX_H__

#include "WNCore/inc/WNUtility.h"

#ifdef _WN_WINDOWS
  #include <Windows.h>
#elif defined _WN_POSIX
  #include "WNCore/inc/WNAssert.h"

  #include <pthread.h>
#endif

namespace wn {
namespace threads {

class shared_mutex final : public core::non_copyable {
public:
  WN_FORCE_INLINE shared_mutex() {
    #ifdef _WN_WINDOWS
      ::InitializeSRWLock(&m_slim_read_write_lock);
    #elif defined _WN_POSIX
      const int result = ::pthread_rwlock_init(&m_read_write_lock, NULL);

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to create read write lock object");
    #endif
  }

  WN_FORCE_INLINE ~shared_mutex() {
    #ifdef _WN_POSIX
      const int result = ::pthread_rwlock_destroy(&m_read_write_lock);

      WN_DEBUG_ASSERT_DESC(result == 0,
        "failed to destroy read write lock object");
    #endif
  }

  WN_FORCE_INLINE wn_void lock() {
    #ifdef _WN_WINDOWS
      ::AcquireSRWLockExclusive(&m_slim_read_write_lock);
    #elif defined _WN_POSIX
      const int result = ::pthread_rwlock_wrlock(&m_read_write_lock);

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to lock read write lock object for writer");
    #endif
  }

  WN_FORCE_INLINE wn_bool try_lock() {
    #ifdef _WN_WINDOWS
      return(::TryAcquireSRWLockExclusive(&m_slim_read_write_lock) != FALSE);
    #elif defined _WN_POSIX
      return(::pthread_rwlock_trywrlock(&m_read_write_lock) == 0);
    #endif
  }

  WN_FORCE_INLINE wn_void unlock() {
    #ifdef _WN_WINDOWS
      ::ReleaseSRWLockExclusive(&m_slim_read_write_lock);
    #elif defined _WN_POSIX
      const int result = ::pthread_rwlock_unlock(&m_read_write_lock);

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to unlock read write lock object for writer");
    #endif
  }

  WN_FORCE_INLINE wn_void lock_shared() {
    #ifdef _WN_WINDOWS
      ::AcquireSRWLockShared(&m_slim_read_write_lock);
    #elif defined _WN_POSIX
      const int result = ::pthread_rwlock_rdlock(&m_read_write_lock);

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to lock read write lock object for reader");
    #endif
  }

  WN_FORCE_INLINE wn_bool try_lock_shared() {
    #ifdef _WN_WINDOWS
      return(::TryAcquireSRWLockShared(&m_slim_read_write_lock) != FALSE);
    #elif defined _WN_POSIX
      return(::pthread_rwlock_tryrdlock(&m_read_write_lock) == 0);
    #endif
  }

  WN_FORCE_INLINE wn_void unlock_shared() {
    #ifdef _WN_WINDOWS
      ::ReleaseSRWLockShared(&m_slim_read_write_lock);
    #elif defined _WN_POSIX
      const int result = ::pthread_rwlock_unlock(&m_read_write_lock);

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to unlock read write lock object for reader");
    #endif
  }

private:
  #ifdef _WN_WINDOWS
    SRWLOCK m_slim_read_write_lock;
  #elif defined _WN_POSIX
    pthread_rwlock_t m_read_write_lock;
  #endif
};

} // namespace threads
} // namesapce wn

#endif // __WN_CONCURRENCY_SHARED_MUTEX_H__
