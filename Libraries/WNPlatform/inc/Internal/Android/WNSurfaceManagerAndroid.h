// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__
#define __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__

#include "WNPlatform/inc/WNSurfaceManager.h"

#include <EGL/egl.h>

namespace WNPlatform {

    class WNSurfaceManagerAndroid: public WNSurfaceManager {
    public:
        WNSurfaceManagerAndroid();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerAndroid() {}

        virtual WNSurfaceManagerReturnCode::Type Initialize();
        virtual WNSurfaceManagerReturnCode::Type CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNConcurrency::WNResourcePointer<WNSurface>& _surface);
        virtual WNSurfaceManagerReturnCode::Type Release();

        WN_BOOL InitializeDisplay(EGLDisplay& _display, EGLConfig& _config); 
        WN_BOOL DestroyDisplay();
        WN_BOOL SurfaceDestroyed();
    private:
        WNSurfaceManagerAndroid(const WNSurfaceManagerAndroid&);
        WNSurfaceManagerAndroid& operator = (const WNSurfaceManagerAndroid&);

    private:
        EGLDisplay mDisplay;
        EGLConfig  mConfig;
        WN_BOOL    mValidSurface; 
    };
};

#endif // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_MANAGER_ANDROID_H__
