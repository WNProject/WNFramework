// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurfaceManager.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace wn {
    class WNSurfaceXWindows;

    class WNSurfaceManagerXWindows: public WNSurfaceManager {
    public:
        WNSurfaceManagerXWindows();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerXWindows() {}

        virtual WNSurfaceManagerReturnCode::type Initialize();
        virtual WNSurfaceManagerReturnCode::type CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, wn::memory::intrusive_ptr<surface>& _surface);
        virtual WNSurfaceManagerReturnCode::type Release();

    private:
        WNSurfaceManagerXWindows(const WNSurfaceManagerXWindows&);
        WNSurfaceManagerXWindows& operator = (const WNSurfaceManagerXWindows&);

    private:
        Display* mDisplay;
        XVisualInfo* mVisualInfo;
        GLXFBConfig  mFBConfig;
        XSetWindowAttributes mWindowAttribs;
        int mScreen;
        Window mRootWindow;
    };
};

#endif // __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
