// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_SHARED_MUTEX_BASE_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_SHARED_MUTEX_BASE_H__

#include "WNCore/inc/utilities.h"

namespace wn {
namespace multi_tasking {
namespace internal {

class shared_mutex_base : core::non_copyable {
protected:
  shared_mutex_base() : m_slim_read_write_lock(SRWLOCK_INIT) {}

  void lock() {
    ::AcquireSRWLockExclusive(&m_slim_read_write_lock);
  }

  bool try_lock() {
    return (::TryAcquireSRWLockExclusive(&m_slim_read_write_lock) != FALSE);
  }

  void unlock() {
    ::ReleaseSRWLockExclusive(&m_slim_read_write_lock);
  }

  void lock_shared() {
    ::AcquireSRWLockShared(&m_slim_read_write_lock);
  }

  bool try_lock_shared() {
    return (::TryAcquireSRWLockShared(&m_slim_read_write_lock) != FALSE);
  }

  void unlock_shared() {
    ::ReleaseSRWLockShared(&m_slim_read_write_lock);
  }

private:
  SRWLOCK m_slim_read_write_lock;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_SHARED_MUTEX_BASE_H__
