// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
#define __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__

#include "WNPlatform/inc/WNSurfaceManager.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace wn {
class WNSurfaceXWindows;

class WNSurfaceManagerXWindows : public WNSurfaceManager {
public:
  WNSurfaceManagerXWindows();
  virtual WN_FORCE_INLINE ~WNSurfaceManagerXWindows() {}

  virtual WNSurfaceManagerReturnCode::type Initialize();
  virtual WNSurfaceManagerReturnCode::type CreateSurface(uint32_t _x,
      uint32_t _y, uint32_t _width, uint32_t _height,
      wn::memory::intrusive_ptr<surface>& _surface);
  virtual WNSurfaceManagerReturnCode::type Release();

private:
  WNSurfaceManagerXWindows(const WNSurfaceManagerXWindows&);
  WNSurfaceManagerXWindows& operator=(const WNSurfaceManagerXWindows&);

private:
  wn::memory::basic_allocator allocator;
  Display* mDisplay;
  XVisualInfo* mVisualInfo;
  GLXFBConfig mFBConfig;
  XSetWindowAttributes mWindowAttribs;
  int mScreen;
  Window mRootWindow;
};
};

#endif  // __WN_PLATFORM_INTERNAL_X_WINDOWs_SURFACE_MANAGER_X_WINDOWS_H__
