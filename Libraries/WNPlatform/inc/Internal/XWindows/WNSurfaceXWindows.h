// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/WNSemaphore.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

namespace wn {
namespace multi_tasking {

template <typename R>
class thread;

} // namespace multi_tasking
} // namesapce wn

namespace wn {
    class WNSurfaceManagerXWindows;

    class WNSurfaceXWindows : public surface {
    public:
        WNSurfaceXWindows(wn::WNSurfaceManagerXWindows& _surfaceManager);
        virtual ~WNSurfaceXWindows();

        bool Initialize(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, Display* _display, XVisualInfo* _visualInfo);

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
        void SurfaceThread();

    private:
        wn::memory::basic_allocator m_allocator;
        Atom mDeleteMessage;
        bool mExiting;
        wn::WNSurfaceManagerXWindows& mManager;
        wn::multi_tasking::semaphore mThreadCreationMutex;
        Display* mDisplay;
        Window mWindow;
        wn::multi_tasking::thread<void>* mSurfaceThread;
        bool mFullscreen;
        uint32_t mX;
        uint32_t mY;
        uint32_t mWidth;
        uint32_t mHeight;
    };
}

#endif // __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__
