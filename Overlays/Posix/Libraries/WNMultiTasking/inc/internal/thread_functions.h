// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_FUNCTIONS_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_FUNCTIONS_H__

#include "WNCore/inc/WNBase.h"

#include <chrono>

#ifdef _WN_POSIX
#include <pthread.h>
#include <unistd.h>

#ifdef _WN_LINUX
#include <sys/syscall.h>
#elif defined _WN_ANDROID
#include <sys/types.h>
#endif
#endif

namespace wn {
namespace multi_tasking {
namespace this_thread {
namespace internal {

WN_INLINE void yield() {
#ifdef _WN_ANDROID
  const int result = ::sched_yield();

  WN_RELEASE_ASSERT(result == 0);
#elif defined _WN_POSIX
  const int result = ::pthread_yield();

  WN_RELEASE_ASSERT(result == 0);
#endif
}

template <typename Representation, typename Period>
WN_INLINE void sleep_for(
    const std::chrono::duration<Representation, Period>& _duration) {
  const std::chrono::seconds sec =
      std::chrono::duration_cast<std::chrono::seconds>(_duration);
  const std::chrono::nanoseconds nano_sec =
      std::chrono::duration_cast<std::chrono::nanoseconds>(_duration) -
      std::chrono::duration_cast<std::chrono::nanoseconds>(sec);
  timespec time_requested = {0};

  time_requested.tv_sec = static_cast<int>(sec.count());
  time_requested.tv_nsec = static_cast<int>(nano_sec.count());

  while (::nanosleep(&time_requested, &time_requested) == -1) {
    WN_RELEASE_ASSERT_DESC(errno == EINTR, "failed to sleep");
  }
}

}  // namespace internal
}  // namespace this_thread
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_FUNCTIONS_H__
