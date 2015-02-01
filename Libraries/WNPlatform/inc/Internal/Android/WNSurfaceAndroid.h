// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
#define __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNConcurrency/inc/WNResourcePointer.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNUtils/inc/Android/WNAndroidEventPump.h"
#include <EGL/egl.h>
#include <GLES/gl.h>


struct android_app;
namespace WNConcurrency {
    template <typename Return>
    class WNThread;
}

namespace WNPlatform {
    class WNSurfaceManagerAndroid;

    class WNSurfaceAndroid : public WNSurface {
    public:
        WNSurfaceAndroid(WNPlatform::WNSurfaceManagerAndroid& _surfaceManager);
        virtual ~WNSurfaceAndroid();

        WN_BOOL Initialize();
        virtual WNSurfaceNativeHandle GetNativeHandle() const;
        virtual WNSurfaceError Resize(WN_UINT32 _width, WN_UINT32 _height);
        virtual WNSurfaceError Move(WN_UINT32 _x, WN_UINT32 _y);
        virtual WN_BOOL IsFullscreen() const;
        virtual WNSurfaceError SetFullscreen(WN_BOOL _fullscreen);

        virtual WN_UINT32 GetWidth() const;
        virtual WN_UINT32 GetHeight() const;
        virtual WN_UINT32 GetX() const;
        virtual WN_UINT32 GetY() const;

    private:
        static WN_VOID HandleWindowCommand(WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app, WN_UINT32 _val, WN_VOID* appData);
        WN_BOOL InitializeWindow(android_app* app);
        WN_BOOL CleanupWindow(android_app* app);
    private:
        WNConcurrency::WNSemaphore mCreationSemaphore;
        WN_BOOL mExiting;
        WN_BOOL mInitialized;
        WNPlatform::WNSurfaceManagerAndroid& mManager;
        EGLDisplay mDisplay;
        EGLConfig mConfig;
        EGLSurface mSurface;
        WN_BOOL mFullscreen;
        WN_UINT32 mWidth;
        WN_UINT32 mHeight;
    };
}

#endif // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
