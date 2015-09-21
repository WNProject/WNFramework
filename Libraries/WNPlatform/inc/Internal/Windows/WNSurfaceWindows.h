// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

namespace wn {
    class WNSurfaceManagerWindows;

    class WNSurfaceWindows : public surface {
    public:
        WNSurfaceWindows(WNSurfaceManagerWindows& _surfaceManager);

        virtual WNSurfaceNativeHandle GetNativeHandle() const;

        virtual WNSurfaceError Resize(wn_uint32 _width, wn_uint32 _height);
        virtual WNSurfaceError Move(wn_uint32 _x, wn_uint32 _y);
        virtual wn_bool IsFullscreen() const;
        virtual WNSurfaceError SetFullscreen(wn_bool _fullscreen);

        virtual wn_uint32 GetWidth() const;
        virtual wn_uint32 GetHeight() const;
        virtual wn_uint32 GetX() const;
        virtual wn_uint32 GetY() const;

    private:
        friend class WNSurfaceManagerWindows;

    private:
        WNSurfaceWindows& operator = (const WNSurfaceWindows&);

        wn_void ProcessCallback(UINT _msg, LPARAM _lparam, WPARAM _wparam);
        wn_void SetNativeHandle(HWND _handle);

    private:
        WNSurfaceManagerWindows& mSurfaceManager;
        wn_bool mFullscreen;
        wn_uint32 mXSize;
        wn_uint32 mYSize;
        wn_uint32 mWidth;
        wn_uint32 mHeight;
        HWND mNativeWindowHandle;
    };
}

#endif // __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__