// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_SURFACE_H__
#define __WN_PLATFORM_SURFACE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNContainers/inc/WNCallback.h"
#include "WNConcurrency/inc/WNResourceBase.h"

#ifdef _WN_LINUX
    #include <X11/Xlib.h>
#endif
#ifdef _WN_ANDROID
    #include <EGL/egl.h>
#endif

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
#endif

#include <list>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNPlatform {
    #ifdef _WN_WINDOWS
        typedef HWND WNSurfaceNativeHandle;
    #elif defined _WN_LINUX
        typedef Window WNSurfaceNativeHandle;
    #elif defined _WN_ANDROID
        typedef EGLSurface WNSurfaceNativeHandle;
    #endif
    class WNSurface : public WNConcurrency::WNResourceBase {
    public:
        WNSurface();
        virtual ~WNSurface();

        enum WNSurfaceError {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNSEStaticSize,
            eWNSETooLarge,
            eWNSEStaticPlacement,
            eWNSEInvalidPosition,
            eWNSEFullscreenOnly,
            eWNSEMAX
        };

        enum WNSurfaceEventType {
            eWNSETResize,
            eWNSETFullscreen,
            eWNSETDestroyed,
            eWNSETRecreated,
            eWNSETMAX
        };

        enum WNRegisteredDataTypes {
            eWNRDTGraphics,
            eWNRDTMAX
        };

        typedef WNContainers::WNCallback2<WN_BOOL, WNSurfaceEventType, WNSurface*> WNSurfaceEventCallback;

        virtual WNSurfaceNativeHandle GetNativeHandle() const = 0;
        virtual WNSurfaceError Resize(WN_UINT32 _width, WN_UINT32 _height) = 0;
        virtual WNSurfaceError Move(WN_UINT32 _x, WN_UINT32 _y) = 0;
        virtual WN_BOOL IsFullscreen() const = 0;
        virtual WNSurfaceError SetFullscreen(WN_BOOL _fullscreen) = 0;
        virtual WN_UINT32 GetWidth() const = 0;
        virtual WN_UINT32 GetHeight() const = 0;
        virtual WN_UINT32 GetX() const = 0;
        virtual WN_UINT32 GetY() const = 0;
        
        WN_VOID RegisterCallback(WNSurfaceEventType _event, const WNSurfaceEventCallback& _callback);
        WN_VOID RegisterData(WNRegisteredDataTypes _type, WN_VOID* _data);

        template <typename Type>
        WN_INLINE Type* GetRegisteredData(const WNRegisteredDataTypes _type) {
            WN_RELEASE_ASSERT(_type < eWNRDTMAX);

            Type* t = reinterpret_cast<Type*>(mRegisteredTypes[_type]);

            return(t);
        }

    protected:
        WN_VOID FireCallback(WNSurfaceEventType _event);

    private:
        std::list<WNSurfaceEventCallback> mEventCallbacks[eWNSETMAX];
        WN_VOID* mRegisteredTypes[eWNRDTMAX];
    };
}

#ifndef _WN_DEBUG
    #include "WNPlatform/inc/Internal/WNSurface.inl"
#endif

#endif // __WN_PLATFORM_SURFACE_H___
