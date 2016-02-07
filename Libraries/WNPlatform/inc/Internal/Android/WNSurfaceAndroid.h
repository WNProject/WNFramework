// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
#define __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__

#include <EGL/egl.h>
#include <GLES/gl.h>
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNPlatform/inc/WNSurface.h"
#include "WNUtils/inc/Android/WNAndroidEventPump.h"

struct android_app;
namespace WNConcurrency {
template <typename Return>
class WNThread;
}

namespace wn {
class WNSurfaceManagerAndroid;

class WNSurfaceAndroid : public surface {
public:
  WNSurfaceAndroid(WNSurfaceManagerAndroid& _surfaceManager);
  virtual ~WNSurfaceAndroid();

  bool Initialize();
  virtual WNSurfaceNativeHandle GetNativeHandle() const;
  virtual WNSurfaceError Resize(uint32_t _width, uint32_t _height);
  virtual WNSurfaceError Move(uint32_t _x, uint32_t _y);
  virtual bool IsFullscreen() const;
  virtual WNSurfaceError SetFullscreen(bool _fullscreen);

  virtual uint32_t GetWidth() const;
  virtual uint32_t GetHeight() const;
  virtual uint32_t GetX() const;
  virtual uint32_t GetY() const;

private:
  static void HandleWindowCommand(
      WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app,
      uint32_t _val, void* appData);
  bool InitializeWindow(android_app* app);
  bool CleanupWindow(android_app* app);

private:
  wn::multi_tasking::semaphore mCreationSemaphore;
  bool mExiting;
  bool mInitialized;
  WNSurfaceManagerAndroid& mManager;
  EGLDisplay mDisplay;
  EGLConfig mConfig;
  EGLSurface mSurface;
  bool mFullscreen;
  uint32_t mWidth;
  uint32_t mHeight;
};
}

#endif  // __WN_PLATFORM_INTERNAL_ANDROID_SURFACE_ANDROID_H__
