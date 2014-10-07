////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__

#include "WNPlatform/inc/WNSurfaceManager.h"
#include "WNConcurrency/inc/WNThread.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNConcurrency/inc/WNMutex.h"

#include <vector>

namespace WNPlatform {
    class WNSurfaceWindows;

    class WNSurfaceManagerWindows : public WNSurfaceManager {
    public:
        WNSurfaceManagerWindows();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerWindows() {}

        virtual WNSurfaceManagerReturnCode::Type Initialize();
        virtual WNSurfaceManagerReturnCode::Type CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNConcurrency::WNResourcePointer<WNSurface>& _surface);
        virtual WNSurfaceManagerReturnCode::Type Release();

    private:
        WNSurfaceManagerWindows(const WNSurfaceManagerWindows&);
        WNSurfaceManagerWindows& operator = (const WNSurfaceManagerWindows&);

    private:
        class WNWindowThreadData {
        public:
            WNWindowThreadData(WNConcurrency::WNResourcePointer<WNPlatform::WNSurfaceWindows> _wnd);

        public:
            WNConcurrency::WNThread<WN_BOOL>* mThread;
            WNConcurrency::WNResourcePointer<WNPlatform::WNSurfaceWindows> mWindow;
            WN_ATOM_T mExit;
        };

    private:
        HWND mPendingHwnd;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static WN_BOOL MessagePump(WNWindowThreadData* _data);
        std::vector<WNWindowThreadData*> mMessagePumps;

        WNConcurrency::WNSemaphore mCreatedWindowLock;
        WNConcurrency::WNMutex mCallbackLock;
        WNConcurrency::WNMutex mWindowCreationLock;
    };
};

#endif // __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__