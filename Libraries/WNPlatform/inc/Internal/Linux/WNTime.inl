// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__
#define __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__

#ifndef __WN_PLATFORM_TIME_H__
    #error "WNTime_Linux.inl should never be included directly. Please include WNTime.h instead"
#elif !defined _WN_POSIX
    #error "WNTime_Linux.inl has been included on a non Linux platform. Please rectify this."
#endif

#include <sys/time.h>
#include <time.h>

namespace WNPlatform {
    WN_FORCE_INLINE WN_UINT64 WNGetBigTime() {
        struct timeval time;

        gettimeofday(&time, NULL);

        return(static_cast<WN_UINT64>(time.tv_sec * 1000LL + time.tv_usec / 1000LL));
    }

    WN_FORCE_INLINE WN_UINT32 WNGetTickCount() {
        return(static_cast<WN_UINT32>(WNGetBigTickCount()));
    }

    WN_FORCE_INLINE WN_UINT64 WNGetBigTickCount() {
        struct timespec time;

        clock_gettime(CLOCK_MONOTONIC, &time);

        return(static_cast<WN_UINT64>(time.tv_sec * 1000LL + time.tv_nsec / 1000000LL));
    }
}

#endif // __WN_PLATFORM_INTERNAL_LINUX_TIME_INL__
