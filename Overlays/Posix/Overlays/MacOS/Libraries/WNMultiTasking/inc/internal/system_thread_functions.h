// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_FUNCTIONS_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_FUNCTIONS_H__

#include "WNMultiTasking/inc/internal/system_thread_yield.h"
#include "core/inc/assert.h"

#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace wn {
namespace multi_tasking {
namespace internal {

inline pid_t get_thread_id() {
  return static_cast<pid_t>(pthread_mach_thread_np(pthread_self()));
}

inline void thread_adjust_priority(const pid_t _thread_id) {
  pthread_t pt = pthread_from_mach_thread_np(_thread_id);
  sched_param param;
  int policy;
  int result = pthread_getschedparam(pt, &policy, &param);
  WN_RELEASE_ASSERT(result == 0, "failed to set thread priority");
  switch (policy) {
    case SCHED_FIFO:
      param.sched_priority = 0;
      break;
    case SCHED_RR:
      param.sched_priority = 0;
      break;
    case SCHED_OTHER:
      param.sched_priority = 19;
      break;
  }
  result = pthread_setschedparam(pt, policy, &param);
  WN_RELEASE_ASSERT(result == 0, "failed to set thread priority");
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_FUNCTIONS_H__
