// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNThreads/inc/WNSemaphore.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

namespace wn {
namespace threads {

template <typename R>
class thread;

} // namespace threads
} // namesapce wn

namespace wn {
    class WNSurfaceManagerXWindows;

    class WNSurfaceXWindows : public surface {
    public:
        WNSurfaceXWindows(wn::WNSurfaceManagerXWindows& _surfaceManager);
        virtual ~WNSurfaceXWindows();

        wn_bool Initialize(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, Display* _display, XVisualInfo* _visualInfo);

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
        wn_void SurfaceThread();

    private:
        wn::memory::basic_allocator m_allocator;
        Atom mDeleteMessage;
        wn_bool mExiting;
        wn::WNSurfaceManagerXWindows& mManager;
        wn::threads::semaphore mThreadCreationMutex;
        Display* mDisplay;
        Window mWindow;
        wn::threads::thread<wn_void>* mSurfaceThread;
        wn_bool mFullscreen;
        wn_uint32 mX;
        wn_uint32 mY;
        wn_uint32 mWidth;
        wn_uint32 mHeight;
    };
}

#endif // __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__
