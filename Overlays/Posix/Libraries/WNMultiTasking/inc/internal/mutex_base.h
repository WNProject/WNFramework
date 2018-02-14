// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_MUTEX_BASE_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_MUTEX_BASE_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"

#include <pthread.h>

#include <atomic>

namespace wn {
namespace multi_tasking {
namespace internal {

class mutex_base : core::non_copyable {
protected:
  mutex_base() {
    pthread_mutexattr_t attributes;
    int result;

    if ((result = ::pthread_mutexattr_init(&attributes)) == 0) {
      if ((result = ::pthread_mutexattr_settype(
               &attributes, PTHREAD_MUTEX_NORMAL)) == 0) {
        if ((result = ::pthread_mutex_init(&m_mutex, &attributes)) == 0) {
          const int destroy_result = ::pthread_mutexattr_destroy(&attributes);

          WN_DEBUG_ASSERT(destroy_result == 0, "failed to create mutex object");

          (void)destroy_result;
        }
      }
    }

    WN_RELEASE_ASSERT(result == 0, "failed to create mutex object");
  }

  ~mutex_base() {
    const int destroy_result = ::pthread_mutex_destroy(&m_mutex);

    WN_DEBUG_ASSERT(destroy_result == 0, "failed to destroy mutex object");

    (void)destroy_result;
  }

  void lock() {
    const int result = ::pthread_mutex_lock(&m_mutex);

    WN_RELEASE_ASSERT(result == 0, "failed to lock mutex object");
  }

  bool try_lock() {
    return (::pthread_mutex_trylock(&m_mutex) == 0);
  }

  void unlock() {
    const int result = ::pthread_mutex_unlock(&m_mutex);

    WN_RELEASE_ASSERT(result == 0, "failed to unlock mutex object");
  }

private:
  pthread_mutex_t m_mutex;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_MUTEX_BASE_H__
