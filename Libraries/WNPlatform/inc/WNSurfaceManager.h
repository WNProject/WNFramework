// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_SURFACE_MANAGER_H__
#define __WN_PLATFORM_SURFACE_MANAGER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNPlatform/inc/WNSurface.h"

namespace wn {
    namespace WNSurfaceManagerReturnCode {
        enum type {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNInitializationFailure,
            eWNResourceLimitReached
        };
    }

    class WNSurfaceManager {
    public:
        virtual WN_FORCE_INLINE ~WNSurfaceManager() {}

        virtual WNSurfaceManagerReturnCode::type Initialize() = 0;
        virtual WNSurfaceManagerReturnCode::type Release() = 0;
        virtual WNSurfaceManagerReturnCode::type CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, wn::surface_handle& _surface) = 0;
    };
}

#endif // __WN_PLATFORM_SURFACE_MANAGER_H__
