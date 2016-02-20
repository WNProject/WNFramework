// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNPlatform/inc/Internal/XWindows/WNSurfaceXWindows.h"
#include "WNMultiTasking/inc/WNThread.h"

wn::WNSurfaceXWindows::WNSurfaceXWindows(
    wn::WNSurfaceManagerXWindows& _surfaceManager)
  : surface(),
    mManager(_surfaceManager),
    mDisplay(NULL),
    mWindow(0),
    mSurfaceThread(nullptr),
    mExiting(false) {}

wn::WNSurfaceXWindows::~WNSurfaceXWindows() {
  mExiting = true;

  if (mWindow && mSurfaceThread) {
    XDestroyWindow(mDisplay, mWindow);

    mSurfaceThread->join();
  }

  mWindow = 0;
  mSurfaceThread = nullptr;
}

bool wn::WNSurfaceXWindows::Initialize(uint32_t _x, uint32_t _y,
    uint32_t _width, uint32_t _height, Display* _display,
    XVisualInfo* _visualInfo) {
  WN_RELEASE_ASSERT(_display != NULL);

  mDisplay = _display;

  XSetWindowAttributes swa;

  swa.colormap =
      XCreateColormap(mDisplay, RootWindow(mDisplay, _visualInfo->screen),
          _visualInfo->visual, AllocNone);
  swa.background_pixmap = None;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;

  mWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, _visualInfo->screen),
      _x, _y, _width, _height, 0, _visualInfo->depth, InputOutput,
      _visualInfo->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);

  if (!mWindow) {
    return (false);
  }

  XSelectInput(_display, mWindow, StructureNotifyMask);
  XMapWindow(_display, mWindow);

  mDeleteMessage = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);

  XSetWMProtocols(mDisplay, mWindow, &mDeleteMessage, 1);

  mSurfaceThread = wn::memory::construct<wn::multi_tasking::thread<void>>(
      &m_allocator, &wn::WNSurfaceXWindows::SurfaceThread, this);

  return (true);
}

void wn::WNSurfaceXWindows::SurfaceThread() {
  for (;;) {
    XEvent e;

    while (XCheckWindowEvent(mDisplay, mWindow, StructureNotifyMask, &e)) {
      switch (e.type) {
        case ConfigureNotify: {
          XConfigureEvent* xce = &e.xconfigure;

          mWidth = xce->width;
          mHeight = xce->height;
          mX = xce->x;
          mY = xce->y;

          FireCallback(eWNSETResize);

          break;
        }
        case ClientMessage: {
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

    wn::multi_tasking::this_thread::sleep_for(std::chrono::milliseconds(1));

    if (mExiting) {
      return;
    }
  }
}

wn::WNSurfaceNativeHandle wn::WNSurfaceXWindows::GetNativeHandle() const {
  return (mWindow);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::Resize(uint32_t, uint32_t) {
  return (ok);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::Move(uint32_t, uint32_t) {
  return (ok);
}

bool wn::WNSurfaceXWindows::IsFullscreen() const {
  return (false);
}

wn::surface::WNSurfaceError wn::WNSurfaceXWindows::SetFullscreen(bool) {
  return (ok);
}

uint32_t wn::WNSurfaceXWindows::GetWidth() const {
  return (mWidth);
}

uint32_t wn::WNSurfaceXWindows::GetHeight() const {
  return (mHeight);
}

uint32_t wn::WNSurfaceXWindows::GetX() const {
  return (mX);
}

uint32_t wn::WNSurfaceXWindows::GetY() const {
  return (mY);
}
