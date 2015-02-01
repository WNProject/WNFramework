// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/Windows/WNSurfaceWindows.h"

wn::WNSurfaceWindows::WNSurfaceWindows(wn::WNSurfaceManagerWindows& _manager) :
    surface(),
    mSurfaceManager(_manager),
    mNativeWindowHandle(NULL),
    mWidth(0),
    mHeight(0) {
}

wn_void wn::WNSurfaceWindows::SetNativeHandle(HWND _handle) {
    mNativeWindowHandle = _handle;
}

wn::WNSurfaceNativeHandle wn::WNSurfaceWindows::GetNativeHandle() const {
    return(mNativeWindowHandle);
}

wn_void wn::WNSurfaceWindows::ProcessCallback(UINT _msg, LPARAM _lparam, WPARAM _wparam) {
    WN_UNUSED_ARGUMENT(_wparam);

    switch (_msg) {
    case WM_SIZE:
        mWidth = _lparam & 0x0000FFFF;
        mHeight = (_lparam >> 16) & 0x0000FFFF;

        FireCallback(surface::eWNSETResize);

        break;
    }
}

wn::surface::WNSurfaceError wn::WNSurfaceWindows::Resize(wn_uint32 _width, wn_uint32 _height) {
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

    return(ok);
}

wn::surface::WNSurfaceError wn::WNSurfaceWindows::Move(wn_uint32 _x, wn_uint32 _y) {
    WN_UNUSED_ARGUMENT(_x);
    WN_UNUSED_ARGUMENT(_y);

    return(ok);
}

wn_bool wn::WNSurfaceWindows::IsFullscreen() const {
    return(wn_false);
}

wn::surface::WNSurfaceError wn::WNSurfaceWindows::SetFullscreen(wn_bool _fullscreen) {
    WN_UNUSED_ARGUMENT(_fullscreen);

    return(ok);
}

wn_uint32 wn::WNSurfaceWindows::GetWidth() const {
    return(mWidth);
}

wn_uint32 wn::WNSurfaceWindows::GetHeight() const {
    return(mHeight);
}

wn_uint32 wn::WNSurfaceWindows::GetX() const {
    return(0);
}

wn_uint32 wn::WNSurfaceWindows::GetY() const {
    return(0);
}
