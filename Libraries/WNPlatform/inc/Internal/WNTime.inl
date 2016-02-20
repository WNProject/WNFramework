// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_TIME_INL__
#define __WN_PLATFORM_INTERNAL_TIME_INL__

#ifndef __WN_PLATFORM_TIME_H__
#error                                                                         \
    "WNTime.inl should never be included directly. Please include WNTime.h instead"
#endif

namespace wn {
WN_FORCE_INLINE uint32_t WNGetTime() {
  return (static_cast<uint32_t>(WNGetBigTime()));
}
}

#ifdef _WN_WINDOWS
#include "WNPlatform/inc/Internal/Windows/WNTime.inl"
#elif defined _WN_LINUX || defined _WN_ANDROID
#include "WNPlatform/inc/Internal/Linux/WNTime.inl"
#endif

#endif  // __WN_PLATFORM_INTERNAL_TIME_INL__