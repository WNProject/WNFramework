// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/XWindows/WNSurfaceXWindows.h"
#include "WNConcurrency/inc/WNThread.h"

WNPlatform::WNSurfaceXWindows::WNSurfaceXWindows(WNPlatform::WNSurfaceManagerXWindows& _surfaceManager) :
    WNSurface(),
    mManager(_surfaceManager),
    mDisplay(NULL),
    mWindow(0),
    mSurfaceThread(WN_NULL),
    mExiting(WN_FALSE) {
}

WNPlatform::WNSurfaceXWindows::~WNSurfaceXWindows() {
    mExiting = WN_TRUE;

    if (mWindow && mSurfaceThread) {
        XDestroyWindow(mDisplay, mWindow);

        mSurfaceThread->WaitForCompletion();
    }

    mWindow = 0;
    mSurfaceThread = WN_NULL;
}

WN_BOOL WNPlatform::WNSurfaceXWindows::Initialize(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, Display* _display, XVisualInfo* _visualInfo) {
    WN_RELEASE_ASSERT(_display != NULL);

    mDisplay = _display;

    XSetWindowAttributes swa;

    swa.colormap = XCreateColormap(mDisplay, RootWindow(mDisplay, _visualInfo->screen), _visualInfo->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    mWindow = XCreateWindow( mDisplay, RootWindow(mDisplay, _visualInfo->screen), _x, _y, _width, _height, 0, _visualInfo->depth, InputOutput, _visualInfo->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

    if (!mWindow) {
        return(WN_FALSE);
    }

    XSelectInput(_display, mWindow, StructureNotifyMask);
    XMapWindow(_display, mWindow);

    mDeleteMessage = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);

    XSetWMProtocols(mDisplay, mWindow, &mDeleteMessage, 1);

    mSurfaceThread = WN_NEW WNConcurrency::WNThread<WN_VOID>();

    if (mSurfaceThread->Initialize(this, &WNPlatform::WNSurfaceXWindows::SurfaceThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
        WN_DELETE(mSurfaceThread);

        return(WN_FALSE);
    }

    return(WN_TRUE);
}

WN_VOID WNPlatform::WNSurfaceXWindows::SurfaceThread() {
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
                        if (xcme->data.l[0] == mDeleteMessage) {
                            return;
                        }
                    }

                    break;
                }
            }
        }

        WNConcurrency::WNThreadSleep(1);

        if (mExiting) {
            return;
        }
    }
}

WNPlatform::WNSurfaceNativeHandle WNPlatform::WNSurfaceXWindows::GetNativeHandle() const {
    return(mWindow);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceXWindows::Resize(WN_UINT32, WN_UINT32) {
    return(eWNOK);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceXWindows::Move(WN_UINT32, WN_UINT32) {
    return(eWNOK);
}

WN_BOOL WNPlatform::WNSurfaceXWindows::IsFullscreen() const {
    return(WN_FALSE);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceXWindows::SetFullscreen(WN_BOOL) {
    return(eWNOK);
}

WN_UINT32 WNPlatform::WNSurfaceXWindows::GetWidth() const {
    return(mWidth);
}

WN_UINT32 WNPlatform::WNSurfaceXWindows::GetHeight() const {
    return(mHeight);
}

WN_UINT32 WNPlatform::WNSurfaceXWindows::GetX() const {
    return(mX);
}

WN_UINT32 WNPlatform::WNSurfaceXWindows::GetY() const {
    return(mY);
}
