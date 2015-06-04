// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_SURFACE_H__
#define __WN_PLATFORM_SURFACE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMemory/inc/WNIntrusivePtrBase.h"

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

namespace wn {
    #ifdef _WN_WINDOWS
        typedef HWND WNSurfaceNativeHandle;
    #elif defined _WN_LINUX
        typedef Window WNSurfaceNativeHandle;
    #elif defined _WN_ANDROID
        typedef EGLSurface WNSurfaceNativeHandle;
    #endif

    class surface : public memory::intrusive_ptr_base {
    public:
        surface() :
            memory::intrusive_ptr_base() {
            for (wn_size_t i = 0; i < eWNRDTMAX; ++i) {
                mRegisteredTypes[i] = wn_nullptr;
            }
        }

        virtual ~surface() {
            FireCallback(eWNSETDestroyed);
        }

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

        typedef wn::containers::function<wn_bool(WNSurfaceEventType, surface*)> WNSurfaceEventCallback;

        virtual WNSurfaceNativeHandle GetNativeHandle() const = 0;
        virtual WNSurfaceError Resize(wn_uint32 _width, wn_uint32 _height) = 0;
        virtual WNSurfaceError Move(wn_uint32 _x, wn_uint32 _y) = 0;
        virtual wn_bool IsFullscreen() const = 0;
        virtual WNSurfaceError SetFullscreen(wn_bool _fullscreen) = 0;
        virtual wn_uint32 GetWidth() const = 0;
        virtual wn_uint32 GetHeight() const = 0;
        virtual wn_uint32 GetX() const = 0;
        virtual wn_uint32 GetY() const = 0;

        wn_void RegisterCallback(WNSurfaceEventType _type, const WNSurfaceEventCallback& _callback) {
            mEventCallbacks[_type].push_back(_callback);
        }

        wn_void RegisterData(WNRegisteredDataTypes _type, wn_void* _data) {
            WN_RELEASE_ASSERT(_type < eWNRDTMAX);
            WN_RELEASE_ASSERT(mRegisteredTypes[_type] == wn_nullptr);

            mRegisteredTypes[_type] = _data;
        }

        template <typename type>
        type* GetRegisteredData(const WNRegisteredDataTypes _type) {
            WN_RELEASE_ASSERT(_type < eWNRDTMAX);

            type* t = reinterpret_cast<type*>(mRegisteredTypes[_type]);

            return(t);
        }

    protected:
        wn_void FireCallback(WNSurfaceEventType _type) {
            for (std::list<WNSurfaceEventCallback>::iterator i = mEventCallbacks[_type].begin(); i != mEventCallbacks[_type].end(); ++i) {
                (*i)(_type, this);
            }
        }

    private:
        std::list<WNSurfaceEventCallback> mEventCallbacks[eWNSETMAX];
        wn_void* mRegisteredTypes[eWNRDTMAX];
    };

    typedef memory::intrusive_ptr<surface> surface_handle;
}
#endif // __WN_PLATFORM_SURFACE_H___
