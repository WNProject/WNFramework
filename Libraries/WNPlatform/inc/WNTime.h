// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_TIME_H__
#define __WN_PLATFORM_TIME_H__

#include "WNCore/inc/WNTypes.h"

typedef uint32_t (*WN_TICK_COUNT_FUNCTION_T)();
typedef uint64_t (*WN_BIG_TICK_COUNT_FUNCTION_T)();

namespace wn {
uint32_t WNGetTime();
uint64_t WNGetBigTime();

uint32_t WNGetTickCount();
uint64_t WNGetBigTickCount();
}

#include "WNPlatform/inc/Internal/WNTime.inl"

#endif  // __WN_PLATFORM_TIME_H__