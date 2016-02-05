// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__

#include "WNPlatform/inc/WNSurfaceManager.h"
#include "WNPlatform/inc/Internal/Windows/WNSurfaceWindows.h"
#include "WNMultiTasking/inc/WNThread.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNMultiTasking/inc/WNMutex.h"

#include <vector>

namespace wn {
    class WNSurfaceManagerWindows : public WNSurfaceManager {
    public:
        WNSurfaceManagerWindows();
        virtual WN_FORCE_INLINE ~WNSurfaceManagerWindows() {}

        virtual WNSurfaceManagerReturnCode::type Initialize();
        virtual WNSurfaceManagerReturnCode::type CreateSurface(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, wn::memory::intrusive_ptr<surface>& _surface);
        virtual WNSurfaceManagerReturnCode::type Release();

    private:
        WNSurfaceManagerWindows(const WNSurfaceManagerWindows&);
        WNSurfaceManagerWindows& operator = (const WNSurfaceManagerWindows&);

    private:
        class WNWindowThreadData {
        public:
            WNWindowThreadData(wn::memory::intrusive_ptr<wn::WNSurfaceWindows> _wnd);

        public:
            wn::multi_tasking::thread<bool>* mThread;
            wn::memory::intrusive_ptr<wn::WNSurfaceWindows> mWindow;
            bool mExit;
        };

    private:
		wn::memory::basic_allocator allocator;
        HWND mPendingHwnd;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static bool MessagePump(WNWindowThreadData* _data);
        std::vector<WNWindowThreadData*> mMessagePumps;

        wn::multi_tasking::semaphore mCreatedWindowLock;
        wn::multi_tasking::mutex mCallbackLock;
        wn::multi_tasking::mutex mWindowCreationLock;
    };
};

#endif // __WN_PLATFORM_INTERNAL_WINDOWS_SURFACE_MANAGER_WINDOWS_H__