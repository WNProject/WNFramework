// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONCURRENCY_SPIN_LOCK_H__
#define __WN_CONCURRENCY_SPIN_LOCK_H__

#include "WNCore/inc/WNUtility.h"
#include "WNThreading/inc/WNConfig.h"
#include "WNThreading/inc/WNThread.h"

#ifdef _WN_WINDOWS
  #include <Windows.h>
#elif defined _WN_POSIX
  #include <pthread.h>
#endif

#ifdef __WN_USE_PTHREAD_SPIN_LOCK
  #include "WNCore/inc/WNAssert.h"

  #include <errno.h>
#else
  #include <atomic>
#endif

namespace wn {
namespace threading {

class spin_lock final : public core::non_copyable {
public:
  WN_FORCE_INLINE spin_lock() {
    #ifdef __WN_USE_PTHREAD_SPIN_LOCK
      const int result_type = ::pthread_spin_init(&m_spin_lock,
                                                  PTHREAD_PROCESS_PRIVATE);

      WN_RELEASE_ASSERT_DESC(result_type == 0,
        "failed to create spin lock object.");
    #else
      m_flag.clear(std::memory_order_release);
    #endif
  }

  WN_FORCE_INLINE ~spin_lock() {
    #ifdef __WN_USE_PTHREAD_SPIN_LOCK
      const int result_type = ::pthread_spin_destroy(&m_spin_lock);

      WN_DEBUG_ASSERT_DESC(result_type == 0,
        "failed to destroy spin lock object.");

      #ifdef _WN_RELEASE
        WN_UNUSED_ARGUMENT(result_type);
      #endif
    #endif
  }

  WN_FORCE_INLINE wn_void lock() {
    #ifdef __WN_USE_PTHREAD_SPIN_LOCK
      const int result_type = ::pthread_spin_lock(&m_spin_lock);

      WN_RELEASE_ASSERT_DESC(result_type == 0,
        "failed to lock spin lock object.");
    #else
      while (!try_lock()) {
        this_thread::yield();
      }
    #endif
  }

  WN_FORCE_INLINE wn_bool try_lock() {
    #ifdef __WN_USE_PTHREAD_SPIN_LOCK
      return(::pthread_spin_trylock(&m_spin_lock) != EBUSY);
    #else
      return(!m_flag.test_and_set(std::memory_order_acquire));
    #endif
  }

  WN_FORCE_INLINE wn_void unlock() {
    #ifdef __WN_USE_PTHREAD_SPIN_LOCK
      const int result_type = ::pthread_spin_unlock(&m_spin_lock);

      WN_RELEASE_ASSERT_DESC(result_type == 0,
        "failed to unlock spin lock object.");
    #else
      m_flag.clear(std::memory_order_release);
    #endif
  }

private:
  #ifdef __WN_USE_PTHREAD_SPIN_LOCK
    pthread_spinlock_t m_spin_lock;
  #else
    std::atomic_flag m_flag;
  #endif
};

class recursive_spin_lock final : public core::non_copyable {
public:
  WN_FORCE_INLINE recursive_spin_lock() :
    #ifdef _WN_WINDOWS
      m_owning_thread(0),
    #elif defined _WN_POSIX
      m_owned(wn_false),
    #endif
    m_recursion_count(0) {
  }

  WN_FORCE_INLINE wn_void lock() {
    #ifdef _WN_WINDOWS
      const DWORD current_thread = ::GetCurrentThreadId();
      const wn_bool owner = (current_thread == m_owning_thread);
    #elif defined _WN_POSIX
      const pthread_t current_thread = ::pthread_self();
      const wn_bool owner = ::pthread_equal(current_thread,
                                            m_owning_thread) != 0 && m_owned;
    #endif

    if (!owner) {
      m_spin_lock.lock();
    }

    #ifdef _WN_POSIX
      m_owned = wn_true;
    #endif

    m_owning_thread = current_thread;
    m_recursion_count++;
  }

  WN_FORCE_INLINE wn_bool try_lock() {
    #ifdef _WN_WINDOWS
      const DWORD current_thread = ::GetCurrentThreadId();
      const wn_bool owner = (current_thread == m_owning_thread);
    #elif defined _WN_POSIX
      const pthread_t current_thread = ::pthread_self();
      const wn_bool owner = ::pthread_equal(current_thread,
                                            m_owning_thread) != 0 && m_owned;
    #endif

    if (!owner) {
      if (!m_spin_lock.try_lock()) {
        return(wn_false);
      }

      #ifdef _WN_POSIX
        m_owned = wn_true;
      #endif

      m_owning_thread = current_thread;
    }

    m_recursion_count++;

    return(wn_true);
  }

  WN_FORCE_INLINE wn_void unlock() {
    const wn_size_t recursion_count = --m_recursion_count;

    if (recursion_count == 0) {
      #ifdef _WN_WINDOWS
        m_owning_thread = 0;
      #elif defined _WN_POSIX
        m_owned = wn_false;
      #endif

      m_spin_lock.unlock();
    }
  }

private:
  #ifdef _WN_WINDOWS
    DWORD m_owning_thread;
  #elif defined _WN_POSIX
    pthread_t m_owning_thread;
    std::atomic_bool m_owned;
  #endif

  std::atomic_size_t m_recursion_count;
  spin_lock m_spin_lock;
};

} // namespace threading
} // namespace wn

#endif // __WN_CONCURRENCY_SPIN_LOCK_H__
