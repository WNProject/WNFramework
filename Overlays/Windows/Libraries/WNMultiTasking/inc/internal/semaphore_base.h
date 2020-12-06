// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_SEMAPHORE_BASE_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_SEMAPHORE_BASE_H__

#include "WNUtilities/inc/handle.h"
#include "core/inc/assert.h"
#include "core/inc/types.h"
#include "core/inc/utilities.h"

namespace wn {
namespace multi_tasking {
namespace internal {

class semaphore_base : core::non_copyable {
protected:
  semaphore_base() = delete;

  semaphore_base(uint32_t _count) {
    m_handle =
        ::CreateSemaphoreW(NULL, static_cast<LONG>(_count), LONG_MAX, NULL);

    WN_RELEASE_ASSERT(m_handle != NULL, "failed to create semaphore object");
  }

  semaphore_base(semaphore_base&& _other) {
    m_handle = _other.m_handle;
    _other.m_handle = nullptr;
  }

  void wait() {
    const DWORD result = ::WaitForSingleObject(m_handle.value(), INFINITE);

    WN_RELEASE_ASSERT(
        result == WAIT_OBJECT_0, "failed to wait on semaphore object");
  }

  bool try_wait() {
    return (::WaitForSingleObject(m_handle.value(), 0) != WAIT_TIMEOUT);
  }

  void notify(uint32_t _count) {
    if (_count > 0) {
      const BOOL result =
          ::ReleaseSemaphore(m_handle.value(), static_cast<LONG>(_count), NULL);

      WN_RELEASE_ASSERT(
          result != FALSE, "failed to post desired count to semaphore object");
    }
  }

  void swap(semaphore_base& _other) {
    m_handle.swap(_other.m_handle);
  }

private:
  utilities::internal::handle m_handle;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_SEMAPHORE_BASE_H__
