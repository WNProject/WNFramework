// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/Windows/WNSurfaceWindows.h"

WNPlatform::WNSurfaceWindows::WNSurfaceWindows(WNPlatform::WNSurfaceManagerWindows& _manager) :
    WNSurface(),
    mSurfaceManager(_manager),
    mNativeWindowHandle(NULL),
    mWidth(0),
    mHeight(0) {
}

WN_VOID WNPlatform::WNSurfaceWindows::SetNativeHandle(HWND _handle) {
    mNativeWindowHandle = _handle;
}

WNPlatform::WNSurfaceNativeHandle WNPlatform::WNSurfaceWindows::GetNativeHandle() const {
    return(mNativeWindowHandle);
}

WN_VOID WNPlatform::WNSurfaceWindows::ProcessCallback(UINT _msg, LPARAM _lparam, WPARAM _wparam) {
    WN_UNUSED_ARG(_wparam);

    switch (_msg) {
    case WM_SIZE:
        mWidth = _lparam & 0x0000FFFF;
        mHeight = (_lparam >> 16) & 0x0000FFFF;

        FireCallback(WNSurface::eWNSETResize);

        break;
    }
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceWindows::Resize(WN_UINT32 _width, WN_UINT32 _height) {
    if (mNativeWindowHandle != NULL) {
        RECT rcClient, rcWind;
        POINT ptDiff;

        GetClientRect(mNativeWindowHandle, &rcClient);
        GetWindowRect(mNativeWindowHandle, &rcWind);

        ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
        ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;

        MoveWindow(mNativeWindowHandle, 0, 0, _width + ptDiff.x, _height + ptDiff.y, TRUE);
    } else {
        mWidth = _width;
        mHeight = _height;
    }

    return(eWNOK);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceWindows::Move(WN_UINT32 _x, WN_UINT32 _y) {
    WN_UNUSED_ARG(_x);
    WN_UNUSED_ARG(_y);

    return(eWNOK);
}

WN_BOOL WNPlatform::WNSurfaceWindows::IsFullscreen() const {
    return(WN_FALSE);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceWindows::SetFullscreen(WN_BOOL _fullscreen) {
    WN_UNUSED_ARG(_fullscreen);

    return(eWNOK);
}

WN_UINT32 WNPlatform::WNSurfaceWindows::GetWidth() const {
    return(mWidth);
}

WN_UINT32 WNPlatform::WNSurfaceWindows::GetHeight() const {
    return(mHeight);
}

WN_UINT32 WNPlatform::WNSurfaceWindows::GetX() const {
    return(0);
}

WN_UINT32 WNPlatform::WNSurfaceWindows::GetY() const {
    return(0);
}
