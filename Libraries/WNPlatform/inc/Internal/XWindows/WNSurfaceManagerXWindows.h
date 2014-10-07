////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurfaceManager.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace WNPlatform {
    class WNSurfaceXWindows;

    class WNSurfaceManagerXWindows: public WNSurfaceManager {
    public:
        WNSurfaceManagerXWindows();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerXWindows() {}

        virtual WNSurfaceManagerReturnCode::Type Initialize();
        virtual WNSurfaceManagerReturnCode::Type CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNConcurrency::WNResourcePointer<WNSurface>& _surface);
        virtual WNSurfaceManagerReturnCode::Type Release();

    private:
        WNSurfaceManagerXWindows(const WNSurfaceManagerXWindows&);
        WNSurfaceManagerXWindows& operator = (const WNSurfaceManagerXWindows&);

    private:
        Display* mDisplay;
        XVisualInfo* mVisualInfo;
        GLXFBConfig  mFBConfig;
        XSetWindowAttributes mWindowAttribs;
        int mScreen;
        Window mRootWindow;
    };
};

#endif // __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
