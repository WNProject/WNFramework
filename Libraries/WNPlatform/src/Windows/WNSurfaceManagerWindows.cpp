// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#include "WNPlatform/inc/Internal/Windows/WNSurfaceManagerWindows.h"
#include "WNPlatform/inc/Internal/Windows/WNSurfaceWindows.h"
#include "WNMemory/inc/WNBasic.h"

#include <tchar.h>
#include <vector>

using namespace wn;

#define WN_WINDOW_CLASS_NAME _T("WNWindowClass")

WNSurfaceManagerReturnCode::type WNSurfaceManagerWindows::Initialize() {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WN_WINDOW_CLASS_NAME;
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex)) {
        wn::memory::memory_zero(&wcex);

        if (GetClassInfoEx(GetModuleHandle(NULL), WN_WINDOW_CLASS_NAME, &wcex) == 0) {
            return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
        }
    }

    return(WNSurfaceManagerReturnCode::ok);
}

WNSurfaceManagerWindows::WNSurfaceManagerWindows() :
    mPendingHwnd(0) {
}

WNSurfaceManagerReturnCode::type WNSurfaceManagerWindows::Release() {
    for (wn_size_t i = 0; i < mMessagePumps.size(); ++i) {
        SendNotifyMessage(mMessagePumps[i]->mWindow->GetNativeHandle(), WM_USER, 0, 0);
    }

    for (wn_size_t i = 0; i < mMessagePumps.size(); ++i) {
        mMessagePumps[i]->mThread->join();

        wn::memory::destroy(mMessagePumps[i]);
    }

    mMessagePumps.clear();

    return(WNSurfaceManagerReturnCode::ok);
}

WNSurfaceManagerReturnCode::type WNSurfaceManagerWindows::CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, wn::memory::intrusive_ptr<surface>& _surface) {
    wn::memory::intrusive_ptr<WNSurfaceWindows> ptr = wn::memory::make_intrusive<WNSurfaceWindows, WNSurfaceManagerWindows&>(&allocator, *this);

    ptr->Resize(_width, _height);
    ptr->Move(_x, _y);

    WNWindowThreadData* dat = wn::memory::construct<WNWindowThreadData>(ptr);

    mWindowCreationLock.lock();
    mPendingHwnd = 0;

    wn::multi_tasking::thread<wn_bool>* thread = wn::memory::construct<wn::multi_tasking::thread<wn_bool>>(&allocator, MessagePump, dat);

    dat->mThread = thread;

    mCreatedWindowLock.wait();

    const HWND wnd = mPendingHwnd;

    mWindowCreationLock.unlock();

    if (wnd == 0) {
        thread->join();

        wn::memory::destroy(thread);

        return(WNSurfaceManagerReturnCode::error);
    }

    mMessagePumps.push_back(dat);

    _surface = ptr;

    return(WNSurfaceManagerReturnCode::ok);
}

LRESULT CALLBACK WNSurfaceManagerWindows::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WNWindowThreadData* data = reinterpret_cast<WNWindowThreadData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (data) {
        data->mWindow->ProcessCallback(uMsg, lParam, wParam);
    }

    switch (uMsg) {
        case WM_USER:
            if (wParam == static_cast<WPARAM>(lParam) && wParam == 0) {
                if (data) {
                    data->mExit = wn_true;
                }

                return(-1);
            }
        case WM_DESTROY:
            if (data) {
                data->mExit = wn_true;

                return(DefWindowProc(hwnd, uMsg, wParam, lParam));
            }
        default:
            return(DefWindowProc(hwnd, uMsg, wParam, lParam));
    }
}


wn_bool WNSurfaceManagerWindows::MessagePump(WNWindowThreadData* _data) {
    WNSurfaceManagerWindows& manager = _data->mWindow->mSurfaceManager;
    RECT r;

    r.left = _data->mWindow->GetX();
    r.right = _data->mWindow->GetX() + _data->mWindow->GetWidth();
    r.top = _data->mWindow->GetY();
    r.bottom = _data->mWindow->GetY() + _data->mWindow->GetHeight();

    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    manager.mPendingHwnd = CreateWindowEx(0, WN_WINDOW_CLASS_NAME, "", WS_OVERLAPPEDWINDOW,
                                          _data->mWindow->GetX(), _data->mWindow->GetY(), r.right - r.left, r.bottom - r.top,
                                          0, 0, GetModuleHandle(NULL), NULL);

    const HWND wnd = manager.mPendingHwnd;

    if (manager.mPendingHwnd == 0) {
        manager.mCreatedWindowLock.notify();

        return(wn_false);
    }

    _data->mWindow->SetNativeHandle(wnd);
    manager.mCreatedWindowLock.notify();

    SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_data));
    ShowWindow(wnd, SW_SHOW);

    MSG msg;

    for (;;) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(_data->mExit) {
            break;
        }

        Sleep(1);
    }

    return(wn_true);
}


WNSurfaceManagerWindows::WNWindowThreadData::WNWindowThreadData(wn::memory::intrusive_ptr<WNSurfaceWindows> _wnd) :
    mWindow(_wnd),
    mExit(wn_false) {
}
