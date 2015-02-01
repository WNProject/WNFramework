// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_TIME_INL__
#define __WN_PLATFORM_INTERNAL_TIME_INL__

#ifndef __WN_PLATFORM_TIME_H__
    #error "WNTime.inl should never be included directly. Please include WNTime.h instead"
#endif

namespace wn {
    WN_FORCE_INLINE wn_uint32 WNGetTime() {
        return(static_cast<wn_uint32>(WNGetBigTime()));
    }
}

#ifdef _WN_WINDOWS
    #include "WNPlatform/inc/Internal/Windows/WNTime.inl"
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include "WNPlatform/inc/Internal/Linux/WNTime.inl"
#endif

#endif // __WN_PLATFORM_INTERNAL_TIME_INL__