// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
        virtual WNSurfaceManagerReturnCode::type CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, memory::intrusive_ptr<surface>& _surface);
        virtual WNSurfaceManagerReturnCode::type Release();

        wn_bool InitializeDisplay(EGLDisplay& _display, EGLConfig& _config); 
        wn_bool DestroyDisplay();
        wn_bool SurfaceDestroyed();
    private:
        WNSurfaceManagerAndroid(const WNSurfaceManagerAndroid&);
        WNSurfaceManagerAndroid& operator = (const WNSurfaceManagerAndroid&);

    private:
        EGLDisplay mDisplay;
        EGLConfig  mConfig;
        wn_bool    mValidSurface; 
    };
};

#endif // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__
