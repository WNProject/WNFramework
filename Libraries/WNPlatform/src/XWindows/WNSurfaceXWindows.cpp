// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/XWindows/WNSurfaceXWindows.h"
#include "WNConcurrency/inc/WNThread.h"

wn::WNSurfaceXWindows::WNSurfaceXWindows(wn::WNSurfaceManagerXWindows& _surfaceManager) :
    surface(),
    mManager(_surfaceManager),
    mDisplay(NULL),
    mWindow(0),
    mSurfaceThread(wn_nullptr),
    mExiting(wn_false) {
}

wn::WNSurfaceXWindows::~WNSurfaceXWindows() {
    mExiting = wn_true;

    if (mWindow && mSurfaceThread) {
        XDestroyWindow(mDisplay, mWindow);

        mSurfaceThread->join();
    }

    mWindow = 0;
    mSurfaceThread = wn_nullptr;
}

wn_bool wn::WNSurfaceXWindows::Initialize(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, Display* _display, XVisualInfo* _visualInfo) {
    WN_RELEASE_ASSERT(_display != NULL);

    mDisplay = _display;

    XSetWindowAttributes swa;

    swa.colormap = XCreateColormap(mDisplay, RootWindow(mDisplay, _visualInfo->screen), _visualInfo->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    mWindow = XCreateWindow( mDisplay, RootWindow(mDisplay, _visualInfo->screen), _x, _y, _width, _height, 0, _visualInfo->depth, InputOutput, _visualInfo->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

    if (!mWindow) {
        return(wn_false);
    }

    XSelectInput(_display, mWindow, StructureNotifyMask);
    XMapWindow(_display, mWindow);

    mDeleteMessage = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);

    XSetWMProtocols(mDisplay, mWindow, &mDeleteMessage, 1);

    mSurfaceThread = wn::memory::construct<wn::concurrency::thread<wn_void> >(&wn::WNSurfaceXWindows::SurfaceThread, this);

    return(wn_true);
}

wn_void wn::WNSurfaceXWindows::SurfaceThread() {
    for(;;) {
        XEvent e;

        while (XCheckWindowEvent(mDisplay, mWindow, StructureNotifyMask, &e)) {
            switch (e.type) {
            case ConfigureNotify:
                {
                    XConfigureEvent* xce = &e.xconfigure;

                    mWidth = xce->width;
                    mHeight = xce->height;
                    mX = xce->x;
                    mY = xce->y;

                    FireCallback(eWNSETResize);

                    break;
                }
            case ClientMessage:
                {
                    XClientMessageEvent* xcme = &e.xclient;

                    if (xcme->format == 32) {
                        if (xcme->data.l[0] == static_cast<long>(mDeleteMessage)) {
                            return;
                        }
                    }

                    break;
                }
            }
        }

        wn::concurrency::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (mExiting) {
            return;
        }
    }
}

wn::WNSurfaceNativeHandle wn::WNSurfaceXWindows::GetNativeHandle() const {
    return(mWindow);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::Resize(wn_uint32, wn_uint32) {
    return(ok);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::Move(wn_uint32, wn_uint32) {
    return(ok);
}

wn_bool wn::WNSurfaceXWindows::IsFullscreen() const {
    return(wn_false);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::SetFullscreen(wn_bool) {
    return(ok);
}

wn_uint32 wn::WNSurfaceXWindows::GetWidth() const {
    return(mWidth);
}

wn_uint32 wn::WNSurfaceXWindows::GetHeight() const {
    return(mHeight);
}

wn_uint32 wn::WNSurfaceXWindows::GetX() const {
    return(mX);
}

wn_uint32 wn::WNSurfaceXWindows::GetY() const {
    return(mY);
}
