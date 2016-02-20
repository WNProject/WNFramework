// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__
#define __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__

#ifndef __WN_PLATFORM_TIME_H__
#error                                                                         \
    "WNTime_Linux.inl should never be included directly. Please include WNTime.h instead"
#elif !defined _WN_POSIX
#error                                                                         \
    "WNTime_Linux.inl has been included on a non Linux platform. Please rectify this."
#endif

#include <sys/time.h>
#include <time.h>

namespace wn {
WN_FORCE_INLINE uint64_t WNGetBigTime() {
  struct timeval time;

  gettimeofday(&time, NULL);

  return (static_cast<uint64_t>(time.tv_sec * 1000LL + time.tv_usec / 1000LL));
}

WN_FORCE_INLINE uint32_t WNGetTickCount() {
  return (static_cast<uint32_t>(WNGetBigTickCount()));
}

WN_FORCE_INLINE uint64_t WNGetBigTickCount() {
  struct timespec time;

  clock_gettime(CLOCK_MONOTONIC, &time);

  return (
      static_cast<uint64_t>(time.tv_sec * 1000LL + time.tv_nsec / 1000000LL));
}
}

#endif  // __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__
