// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_SEMAPHORE_H__
#define __WN_MULTI_TASKING_SEMAPHORE_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"

#ifdef _WN_WINDOWS
#include <Windows.h>
#elif defined _WN_POSIX
#include <errno.h>
#include <semaphore.h>
#endif

namespace wn {
namespace multi_tasking {

class semaphore final : public core::non_copyable {
public:
  WN_FORCE_INLINE semaphore() : semaphore(0) {}

  WN_FORCE_INLINE semaphore(const uint16_t count) {
#ifdef _WN_WINDOWS
    m_handle =
        ::CreateSemaphoreW(NULL, static_cast<LONG>(count), LONG_MAX, NULL);

    WN_RELEASE_ASSERT_DESC(
        m_handle != NULL, "failed to create semaphore object");
#elif defined _WN_POSIX
    const int result =
        ::sem_init(&m_semaphore, 0, static_cast<unsigned int>(count));

    WN_RELEASE_ASSERT_DESC(result == 0, "failed to create semaphore object");
#endif
  }

  WN_FORCE_INLINE ~semaphore() {
#ifdef _WN_WINDOWS
    const BOOL result = ::CloseHandle(m_handle);

    WN_DEBUG_ASSERT_DESC(result != FALSE, "failed to destroy semaphore object");

#ifndef _WN_DEBUG
    WN_UNUSED_ARGUMENT(result);
#endif
#elif defined _WN_POSIX
    const int result = ::sem_destroy(&m_semaphore);

    WN_DEBUG_ASSERT_DESC(result == 0, "failed to destroy semaphore object");

#ifndef _WN_DEBUG
    WN_UNUSED_ARGUMENT(result);
#endif
#endif
  }

  WN_FORCE_INLINE void wait() {
#ifdef _WN_WINDOWS
    const DWORD result = ::WaitForSingleObject(m_handle, INFINITE);

    WN_RELEASE_ASSERT_DESC(
        result == WAIT_OBJECT_0, "failed to wait on semaphore object");
#elif defined _WN_POSIX
    int result;

    while ((result = ::sem_wait(&m_semaphore)) == -1) {
      WN_RELEASE_ASSERT_DESC(
          errno == EINTR, "failed to wait on semaphore object");
    }
#endif
  }

  WN_FORCE_INLINE bool try_wait() {
#ifdef _WN_WINDOWS
    return (::WaitForSingleObject(m_handle, 0) != WAIT_TIMEOUT);
#elif defined _WN_POSIX
    int result;

    while ((result = ::sem_trywait(&m_semaphore)) == -1 && errno != EAGAIN) {
      WN_RELEASE_ASSERT_DESC(
          errno == EINTR, "failed to try to wait on semaphore object");
    }

    return (result == 0);
#endif
  }

  WN_FORCE_INLINE void notify() {
    notify(1);
  }

  WN_FORCE_INLINE void notify(const uint16_t count) {
    WN_DEBUG_ASSERT_DESC(
        count != 0, "posted a semaphore object with a count of 0");

#ifdef _WN_WINDOWS
    if (count > 0) {
      const BOOL result =
          ::ReleaseSemaphore(m_handle, static_cast<LONG>(count), NULL);

      WN_RELEASE_ASSERT_DESC(
          result != FALSE, "failed to post desired count to semaphore object");
    }
#elif defined _WN_POSIX
    for (uint16_t i = 0; i < count; ++i) {
      const int result = ::sem_post(&m_semaphore);

      WN_RELEASE_ASSERT_DESC(
          result == 0, "failed to post desired count to semaphore object");
    }
#endif
  }

private:
#ifdef _WN_WINDOWS
  HANDLE m_handle;
#elif defined _WN_POSIX
  sem_t m_semaphore;
#endif
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SEMAPHORE_H__
