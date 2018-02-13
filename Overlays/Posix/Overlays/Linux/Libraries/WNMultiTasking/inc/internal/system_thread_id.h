// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_ID_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_ID_H__

#include <sys/syscall.h>
#include <unistd.h>

namespace wn {
namespace multi_tasking {
namespace internal {

inline pid_t system_thread_id() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_SYSTEM_THREAD_ID_H__
