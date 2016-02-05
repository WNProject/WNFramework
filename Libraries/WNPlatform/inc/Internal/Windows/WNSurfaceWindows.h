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

        virtual WNSurfaceError Resize(uint32_t _width, uint32_t _height);
        virtual WNSurfaceError Move(uint32_t _x, uint32_t _y);
        virtual bool IsFullscreen() const;
        virtual WNSurfaceError SetFullscreen(bool _fullscreen);

        virtual uint32_t GetWidth() const;
        virtual uint32_t GetHeight() const;
        virtual uint32_t GetX() const;
        virtual uint32_t GetY() const;

    private:
        friend class WNSurfaceManagerWindows;

    private:
        WNSurfaceWindows& operator = (const WNSurfaceWindows&);

        void ProcessCallback(UINT _msg, LPARAM _lparam, WPARAM _wparam);
        void SetNativeHandle(HWND _handle);

    private:
        WNSurfaceManagerWindows& mSurfaceManager;
        bool mFullscreen;
        uint32_t mXSize;
        uint32_t mYSize;
        uint32_t mWidth;
        uint32_t mHeight;
        HWND mNativeWindowHandle;
    };
}

#endif // __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__