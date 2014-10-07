////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__

#include "WNPlatform/inc/WNSurface.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

namespace WNPlatform {
    class WNSurfaceManagerWindows;

    class WNSurfaceWindows : public WNSurface {
    public:
        WNSurfaceWindows(WNSurfaceManagerWindows& _surfaceManager);

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
        friend class WNSurfaceManagerWindows;

    private:
        WNSurfaceWindows& operator = (const WNSurfaceWindows&);

        WN_VOID ProcessCallback(UINT _msg, LPARAM _lparam, WPARAM _wparam);
        WN_VOID SetNativeHandle(HWND _handle);

    private:
        WNSurfaceManagerWindows& mSurfaceManager;
        WN_BOOL mFullscreen;
        WN_UINT32 mXSize;
        WN_UINT32 mYSize;
        WN_UINT32 mWidth;
        WN_UINT32 mHeight;
        HWND mNativeWindowHandle;
    };
}

#endif // __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_WINDOWS_H__