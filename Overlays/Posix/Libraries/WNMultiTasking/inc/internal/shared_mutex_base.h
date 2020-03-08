// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_SHARED_MUTEX_BASE_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_SHARED_MUTEX_BASE_H__

#include <pthread.h>

#include "WNCore/inc/assert.h"
#include "WNCore/inc/utilities.h"

namespace wn {
namespace multi_tasking {
namespace internal {

class shared_mutex_base : core::non_copyable {
protected:
  shared_mutex_base() {
    const int result = ::pthread_rwlock_init(&m_read_write_lock, NULL);

    WN_RELEASE_ASSERT(result == 0, "failed to create read write lock object");
  }

  ~shared_mutex_base() {
    const int result = ::pthread_rwlock_destroy(&m_read_write_lock);

    WN_DEBUG_ASSERT(result == 0, "failed to destroy read write lock object");

    (void)result;
  }

  void lock() {
    const int result = ::pthread_rwlock_wrlock(&m_read_write_lock);

    WN_RELEASE_ASSERT(
        result == 0, "failed to lock read write lock object for writer");
  }

  bool try_lock() {
    return (::pthread_rwlock_trywrlock(&m_read_write_lock) == 0);
  }

  void unlock() {
    const int result = ::pthread_rwlock_unlock(&m_read_write_lock);

    WN_RELEASE_ASSERT(
        result == 0, "failed to unlock read write lock object for writer");
  }

  void lock_shared() {
    const int result = ::pthread_rwlock_rdlock(&m_read_write_lock);

    WN_RELEASE_ASSERT(
        result == 0, "failed to lock read write lock object for reader");
  }

  bool try_lock_shared() {
    return (::pthread_rwlock_tryrdlock(&m_read_write_lock) == 0);
  }

  void unlock_shared() {
    const int result = ::pthread_rwlock_unlock(&m_read_write_lock);

    WN_RELEASE_ASSERT(
        result == 0, "failed to unlock read write lock object for reader");
  }

private:
  pthread_rwlock_t m_read_write_lock;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_SHARED_MUTEX_BASE_H__
