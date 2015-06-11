// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
#define __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNUtils/inc/Android/WNAndroidEventPump.h"
#include <EGL/egl.h>
#include <GLES/gl.h>


struct android_app;
namespace WNConcurrency {
    template <typename Return>
    class WNThread;
}

namespace wn {
    class WNSurfaceManagerAndroid;

    class WNSurfaceAndroid : public surface {
    public:
        WNSurfaceAndroid(WNSurfaceManagerAndroid& _surfaceManager);
        virtual ~WNSurfaceAndroid();

        wn_bool Initialize();
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
        static wn_void HandleWindowCommand(WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app, wn_uint32 _val, wn_void* appData);
        wn_bool InitializeWindow(android_app* app);
        wn_bool CleanupWindow(android_app* app);
    private:
        wn::concurrency::semaphore mCreationSemaphore;
        wn_bool mExiting;
        wn_bool mInitialized;
        WNSurfaceManagerAndroid& mManager;
        EGLDisplay mDisplay;
        EGLConfig mConfig;
        EGLSurface mSurface;
        wn_bool mFullscreen;
        wn_uint32 mWidth;
        wn_uint32 mHeight;
    };
}

#endif // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
