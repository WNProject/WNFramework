////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNConcurrency/inc/WNResourcePointer.h"
#include "WNConcurrency/inc/WNSemaphore.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

namespace WNConcurrency {
    template <typename Return>
    class WNThread;
}

namespace WNPlatform {
    class WNSurfaceManagerXWindows;

    class WNSurfaceXWindows : public WNSurface {
    public:
        WNSurfaceXWindows(WNPlatform::WNSurfaceManagerXWindows& _surfaceManager);
        virtual ~WNSurfaceXWindows();

        WN_BOOL Initialize(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, Display* _display, XVisualInfo* _visualInfo);

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
        WN_VOID SurfaceThread();

    private:
        Atom mDeleteMessage;
        WN_BOOL mExiting;
        WNPlatform::WNSurfaceManagerXWindows& mManager;
        WNConcurrency::WNSemaphore mThreadCreationMutex;
        Display* mDisplay;
        Window mWindow;
        WNConcurrency::WNThread<WN_VOID>* mSurfaceThread;
        WN_BOOL mFullscreen;
        WN_UINT32 mX;
        WN_UINT32 mY;
        WN_UINT32 mWidth;
        WN_UINT32 mHeight;
    };
}

#endif // __WN_PLATFORM_INTERNAL_X_WINDOWS_SURFACE_X_WINDOWS_H__