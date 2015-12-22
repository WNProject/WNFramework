// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONCURRENCY_MUTEX_H__
#define __WN_CONCURRENCY_MUTEX_H__

#include "WNCore/inc/WNUtility.h"
#include "WNCore/inc/WNAssert.h"

#ifdef _WN_WINDOWS
  #include <Windows.h>
#elif defined _WN_POSIX
  #include <pthread.h>
#else
  #include <mutex>
#endif

#include <atomic>

namespace wn {
namespace threads {

class mutex final : public core::non_copyable {
public:
  WN_FORCE_INLINE mutex() :
    mutex(4000) {
  }

  WN_FORCE_INLINE mutex(const wn_uint32 spin_count) :
    m_spin_count(spin_count),
    m_lock_count(0) {
    #ifdef _WN_WINDOWS
      m_semaphore = ::CreateSemaphoreW(NULL, 0, 1, NULL);

      WN_RELEASE_ASSERT_DESC(m_semaphore != NULL,
        "failed to create mutex object");
    #elif defined _WN_POSIX
      pthread_mutexattr_t attributes;
      int result;

      if ((result = ::pthread_mutexattr_init(&attributes)) == 0) {
        if ((result = ::pthread_mutexattr_settype(&attributes,
                                                  PTHREAD_MUTEX_NORMAL)) == 0) {
          if ((result = ::pthread_mutex_init(&m_mutex, &attributes)) == 0) {
            const int destroy_result = ::pthread_mutexattr_destroy(&attributes);

            WN_DEBUG_ASSERT_DESC(destroy_result == 0,
              "failed to create mutex object");

            #ifdef _WN_RELEASE
              WN_UNUSED_ARGUMENT(destroy_result);
            #endif
          }
        }
      }

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to create mutex object");
    #endif
  }

  WN_FORCE_INLINE ~mutex() {
    #ifdef _WN_WINDOWS
      const BOOL close_result = ::CloseHandle(m_semaphore);

      WN_DEBUG_ASSERT_DESC(close_result != FALSE,
        "failed to destroy mutex object");

      #ifndef _WN_DEBUG
        WN_UNUSED_ARGUMENT(close_result);
      #endif
    #elif defined _WN_POSIX
      const int destroy_result = ::pthread_mutex_destroy(&m_mutex);

      WN_DEBUG_ASSERT_DESC(destroy_result == 0,
        "failed to destroy mutex object");

      #ifndef _WN_DEBUG
        WN_UNUSED_ARGUMENT(destroy_result);
      #endif
    #endif
  }

  WN_FORCE_INLINE wn_void lock() {
    for (wn_uint32 i = 0; i < m_spin_count; ++i) {
      if (try_lock()) {
        return;
      }
    }

    if (m_lock_count.fetch_add(1, std::memory_order_acquire) > 0) {
      #ifdef _WN_WINDOWS
        const DWORD result = ::WaitForSingleObject(m_semaphore, INFINITE);

        WN_RELEASE_ASSERT_DESC(result == WAIT_OBJECT_0,
          "failed to lock mutex object");
      #elif defined _WN_POSIX
        const int result = ::pthread_mutex_lock(&m_mutex);

        WN_RELEASE_ASSERT_DESC(result == 0,
          "failed to lock mutex object");
      #endif
    }
  }

  WN_FORCE_INLINE wn_bool try_lock() {
    wn_size_t expected = 0;

    return(m_lock_count.compare_exchange_strong(expected, 1,
                                                std::memory_order_acquire));
  }

  WN_FORCE_INLINE wn_void unlock() {
    if (m_lock_count.fetch_sub(1, std::memory_order_release) > 1) {
      #ifdef _WN_WINDOWS
        const BOOL result = ::ReleaseSemaphore(m_semaphore, 1, NULL);

        WN_RELEASE_ASSERT_DESC(result != FALSE,
          "failed to unlock mutex object");
      #elif defined _WN_POSIX
        const int result = ::pthread_mutex_unlock(&m_mutex);

        WN_RELEASE_ASSERT_DESC(result == 0,
          "failed to unlock mutex object");
      #endif
    }
  }

  WN_FORCE_INLINE wn_uint32 spin_count() const {
    return(m_spin_count);
  }

  WN_FORCE_INLINE wn_void set_spin_count(const wn_uint32 spin_count) {
    m_spin_count.store(spin_count, std::memory_order_acquire);
  }

private:
  #ifdef _WN_WINDOWS
    HANDLE m_semaphore;
  #elif defined _WN_POSIX
    pthread_mutex_t m_mutex;
  #endif

  std::atomic<wn_uint32> m_spin_count;
  std::atomic<wn_size_t> m_lock_count;
};

class recursive_mutex final : public core::non_copyable {
public:
  WN_FORCE_INLINE recursive_mutex() :
    recursive_mutex(4000) {
  }

  WN_FORCE_INLINE recursive_mutex(const wn_uint32 spin_count) :
    m_spin_count(spin_count) {
    #ifdef _WN_WINDOWS
      const BOOL result = ::InitializeCriticalSectionAndSpinCount(
        &m_critical_section, static_cast<DWORD>(m_spin_count));

      WN_RELEASE_ASSERT_DESC(result, "failed to create mutex object");
    #elif defined _WN_POSIX
      pthread_mutexattr_t attributes;
      int result;

      if ((result = ::pthread_mutexattr_init(&attributes)) == 0) {
        if ((result = ::pthread_mutexattr_settype(&attributes,
                                                  PTHREAD_MUTEX_RECURSIVE)) == 0) {
          if ((result = ::pthread_mutex_init(&m_mutex, &attributes)) == 0) {
            const int destroy_result = ::pthread_mutexattr_destroy(&attributes);

            WN_DEBUG_ASSERT_DESC(destroy_result == 0,
              "failed to create mutex object");

            #ifdef _WN_RELEASE
              WN_UNUSED_ARGUMENT(destroy_result);
            #endif
          }
        }
      }

      WN_RELEASE_ASSERT_DESC(result == 0,
        "failed to create mutex object");
    #endif
  }

  WN_FORCE_INLINE ~recursive_mutex() {
    #ifdef _WN_WINDOWS
      ::DeleteCriticalSection(&m_critical_section);
    #elif defined _WN_POSIX
      const int destroy_result = ::pthread_mutex_destroy(&m_mutex);

      WN_DEBUG_ASSERT_DESC(destroy_result == 0,
        "failed to destroy mutex object");

      #ifndef WN_DEBUG
        WN_UNUSED_ARGUMENT(destroy_result);
      #endif
    #endif
  }

  WN_FORCE_INLINE wn_void lock() {
    #ifdef _WN_WINDOWS
      ::EnterCriticalSection(&m_critical_section);
    #elif defined _WN_POSIX
      for (wn_uint32 i = 0; i < m_spin_count; ++i) {
        if (try_lock()) {
          return;
        }
      }

      const int lock_result = ::pthread_mutex_lock(&m_mutex);

      WN_RELEASE_ASSERT_DESC(lock_result == 0,
        "failed to lock mutex object");
    #endif
  }

  WN_FORCE_INLINE wn_bool try_lock() {
    #ifdef _WN_WINDOWS
      return(::TryEnterCriticalSection(&m_critical_section) == TRUE);
    #elif defined _WN_POSIX
      return(::pthread_mutex_trylock(&m_mutex) == 0);
    #endif
  }

  WN_FORCE_INLINE wn_void unlock() {
    #ifdef _WN_WINDOWS
      ::LeaveCriticalSection(&m_critical_section);
    #elif defined _WN_POSIX
      const int unlock_result = ::pthread_mutex_unlock(&m_mutex);

      WN_RELEASE_ASSERT_DESC(unlock_result == 0,
        "failed to unlock mutex object");
    #endif
  }

  WN_FORCE_INLINE wn_uint32 spin_count() const {
    return(m_spin_count);
  }

  WN_FORCE_INLINE wn_void set_spin_count(const wn_uint32 spin_count) {
    m_spin_count.store(spin_count, std::memory_order_acquire);
  }

private:
  #ifdef _WN_WINDOWS
    CRITICAL_SECTION m_critical_section;
  #elif defined _WN_POSIX
    pthread_mutex_t m_mutex;
  #endif

  std::atomic<wn_uint32> m_spin_count;
};

} // namespace threads
} // namespace wn

#endif // __WN_CONCURRENCY_MUTEX_H__