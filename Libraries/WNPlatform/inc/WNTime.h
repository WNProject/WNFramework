// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_TIME_H__
#define __WN_PLATFORM_TIME_H__

#include "WNCore/inc/WNTypes.h"

typedef wn_uint32 (*WN_TICK_COUNT_FUNCTION_T)();
typedef wn_uint64 (*WN_BIG_TICK_COUNT_FUNCTION_T)();

namespace wn {
    wn_uint32 WNGetTime();
    wn_uint64 WNGetBigTime();

    wn_uint32 WNGetTickCount();
    wn_uint64 WNGetBigTickCount();
}

#include "WNPlatform/inc/Internal/WNTime.inl"

#endif // __WN_PLATFORM_TIME_H__