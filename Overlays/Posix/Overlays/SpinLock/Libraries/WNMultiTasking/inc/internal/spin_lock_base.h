// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_SPIN_LOCK_INTERNAL_SPIN_LOCK_BASE_H__
#define __WN_MULTI_TASKING_POSIX_SPIN_LOCK_INTERNAL_SPIN_LOCK_BASE_H__

#include <errno.h>
#include <pthread.h>

#include "WNCore/inc/assert.h"
#include "WNCore/inc/utilities.h"

namespace wn {
namespace multi_tasking {
namespace internal {

class spin_lock_base : core::non_copyable {
protected:
  spin_lock_base() {
    const int result =
        ::pthread_spin_init(&m_spin_lock, PTHREAD_PROCESS_PRIVATE);

    WN_RELEASE_ASSERT(result == 0, "failed to create spin lock object.");
  }

  ~spin_lock_base() {
    const int result = ::pthread_spin_destroy(&m_spin_lock);

    WN_DEBUG_ASSERT(result == 0, "failed to destroy spin lock object.");

    (void)result;
  }

  void lock() {
    const int result = ::pthread_spin_lock(&m_spin_lock);

    WN_RELEASE_ASSERT(result == 0, "failed to lock spin lock object.");
  }

  bool try_lock() {
    return (::pthread_spin_trylock(&m_spin_lock) != EBUSY);
  }

  void unlock() {
    const int result = ::pthread_spin_unlock(&m_spin_lock);

    WN_RELEASE_ASSERT(result == 0, "failed to unlock spin lock object.");
  }

private:
  pthread_spinlock_t m_spin_lock;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_SPIN_LOCK_INTERNAL_SPIN_LOCK_BASE_H__
