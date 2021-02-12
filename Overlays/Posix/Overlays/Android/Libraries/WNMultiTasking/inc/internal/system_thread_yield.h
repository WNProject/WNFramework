// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_ANDROID_INTERNAL_SYSTEM_THREAD_YIELD_H__
#define __WN_MULTI_TASKING_ANDROID_INTERNAL_SYSTEM_THREAD_YIELD_H__

#include <sched.h>

namespace wn {
namespace multi_tasking {
namespace internal {

inline int thread_yield() {
  return ::sched_yield();
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_ANDROID_INTERNAL_SYSTEM_THREAD_YIELD_H__
