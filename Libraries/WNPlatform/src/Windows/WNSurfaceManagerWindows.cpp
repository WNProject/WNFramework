////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "WNPlatform/inc/Internal/Windows/WNSurfaceManagerWindows.h"
#include "WNPlatform/inc/Internal/Windows/WNSurfaceWindows.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNMemory/inc/WNManipulation.h"

#include <tchar.h>
#include <vector>

using namespace WNPlatform;
using namespace WNMemory;
using namespace WNConcurrency;

#define WN_WINDOW_CLASS_NAME _T("WNWindowClass")

WNSurfaceManagerReturnCode::Type WNSurfaceManagerWindows::Initialize() {
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
        WNMemClr(&wcex, sizeof(WNDCLASSEX));

        if (GetClassInfoEx(GetModuleHandle(NULL), WN_WINDOW_CLASS_NAME, &wcex) == 0) {
            return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
        }
    }

    return(WNSurfaceManagerReturnCode::eWNOK);
}

WNSurfaceManagerWindows::WNSurfaceManagerWindows() :
    mPendingHwnd(0) {
}

WNSurfaceManagerReturnCode::Type WNSurfaceManagerWindows::Release() {
    for (WN_SIZE_T i = 0; i < mMessagePumps.size(); ++i) {
        SendNotifyMessage(mMessagePumps[i]->mWindow->GetNativeHandle(), WM_USER, 0, 0);
    }

    for (WN_SIZE_T i = 0; i < mMessagePumps.size(); ++i) {
        mMessagePumps[i]->mThread->WaitForCompletion();

        WN_DELETE(mMessagePumps[i]);
    }

    mMessagePumps.clear();

    return(WNSurfaceManagerReturnCode::eWNOK);
}

WNSurfaceManagerReturnCode::Type WNSurfaceManagerWindows::CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNResourcePointer<WNSurface>& _surface) {
    WNResourcePointer<WNSurfaceWindows> ptr = WNAllocateResource<WNSurfaceWindows, WNSurfaceManagerWindows&>(*this);

    ptr->Resize(_width, _height);
    ptr->Move(_x, _y);

    WNWindowThreadData* dat = WN_NEW WNWindowThreadData(ptr);

    mWindowCreationLock.Lock();
    mPendingHwnd = 0;

    WNConcurrency::WNThread<WN_BOOL>* thread = WN_NEW WNConcurrency::WNThread<WN_BOOL>();

    if (thread->Initialize(MessagePump, dat) != WNConcurrency::WNThreadReturnCode::eWNOK) {
        WN_DELETE(thread);

        return(WNSurfaceManagerReturnCode::eWNError);
    }

    dat->mThread = thread;

    mCreatedWindowLock.Wait();

    const HWND wnd = mPendingHwnd;

    mWindowCreationLock.Unlock();

    if (wnd == 0) {
        thread->WaitForCompletion();

        WN_DELETE(thread);

        return(WNSurfaceManagerReturnCode::eWNError);
    }

    mMessagePumps.push_back(dat);

    _surface = ptr;

    return(WNSurfaceManagerReturnCode::eWNOK);
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
                    data->mExit = WN_TRUE;
                }

                return(-1);
            }
        case WM_DESTROY:
            if (data) {
                data->mExit = WN_TRUE;

                return(DefWindowProc(hwnd, uMsg, wParam, lParam));
            }
        default:
            return(DefWindowProc(hwnd, uMsg, wParam, lParam));
    }
}


WN_BOOL WNSurfaceManagerWindows::MessagePump(WNWindowThreadData* _data) {
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
        manager.mCreatedWindowLock.Post();

        return(WN_FALSE);
    }

    _data->mWindow->SetNativeHandle(wnd);
    manager.mCreatedWindowLock.Post();

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

    return(WN_TRUE);
}


WNSurfaceManagerWindows::WNWindowThreadData::WNWindowThreadData(WNResourcePointer<WNSurfaceWindows> _wnd) :
    mWindow(_wnd),
    mExit(WN_FALSE) {
}
