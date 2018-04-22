// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_MUTEX_BASE_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_MUTEX_BASE_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"
#include "WNUtilities/inc/handle.h"

#include <atomic>

namespace wn {
namespace multi_tasking {
namespace internal {

class mutex_base : core::non_copyable {
protected:
  mutex_base() : m_lock_count(0) {
    m_semaphore = ::CreateSemaphoreW(NULL, 0, 1, NULL);

    WN_RELEASE_ASSERT(m_semaphore != NULL, "failed to create mutex object");
  }

  void lock() {
    if (m_lock_count.fetch_add(1, std::memory_order_acquire) > 0) {
      const DWORD result = ::WaitForSingleObject(m_semaphore.value(), INFINITE);

      WN_RELEASE_ASSERT(result == WAIT_OBJECT_0, "failed to lock mutex object");
    }
  }

  bool try_lock() {
    size_t expected = 0;

    return m_lock_count.compare_exchange_strong(
        expected, 1, std::memory_order_acquire);
  }

  void unlock() {
    if (m_lock_count.fetch_sub(1, std::memory_order_release) > 1) {
      const BOOL result = ::ReleaseSemaphore(m_semaphore.value(), 1, NULL);

      WN_RELEASE_ASSERT(result != FALSE, "failed to unlock mutex object");
    }
  }

private:
  utilities::internal::handle m_semaphore;
  std::atomic_size_t m_lock_count;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_MUTEX_BASE_H__
