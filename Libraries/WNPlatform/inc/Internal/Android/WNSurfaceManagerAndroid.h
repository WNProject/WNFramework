// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__
#define __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__

#include "WNPlatform/inc/WNSurfaceManager.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#include <EGL/egl.h>

namespace wn {

    class WNSurfaceManagerAndroid: public WNSurfaceManager {
    public:
        WNSurfaceManagerAndroid();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerAndroid() {}

        virtual WNSurfaceManagerReturnCode::type Initialize();
        virtual WNSurfaceManagerReturnCode::type CreateSurface(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, memory::intrusive_ptr<surface>& _surface);
        virtual WNSurfaceManagerReturnCode::type Release();

        bool InitializeDisplay(EGLDisplay& _display, EGLConfig& _config);
        bool DestroyDisplay();
        bool SurfaceDestroyed();
    private:
        WNSurfaceManagerAndroid(const WNSurfaceManagerAndroid&);
        WNSurfaceManagerAndroid& operator = (const WNSurfaceManagerAndroid&);

    private:
		wn::memory::basic_allocator allocator;
        EGLDisplay mDisplay;
        EGLConfig  mConfig;
        bool    mValidSurface;
    };
};

#endif // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__
