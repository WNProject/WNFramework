// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"


namespace wn {
  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::Initialize() {
     return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::CreateSurface(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, memory::intrusive_ptr<surface>& _surface) {
      if(mValidSurface)
      {
          return(WNSurfaceManagerReturnCode::eWNResourceLimitReached);
      }
      memory::intrusive_ptr<WNSurfaceAndroid> ptr = memory::make_intrusive<WNSurfaceAndroid, WNSurfaceManagerAndroid&>(&allocator, *this);

      if (!ptr->Initialize()) {
          return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
      }

      _surface = ptr;
      mValidSurface = true;
      return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::Release() {
      return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerAndroid::WNSurfaceManagerAndroid() :
      mDisplay(EGL_NO_DISPLAY),
      mConfig(nullptr),
      mValidSurface(false) {
  }


  bool WNSurfaceManagerAndroid::InitializeDisplay(EGLDisplay& _display, EGLConfig& _config) {
      const EGLint attributes[] = {
          EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_NONE
      };

      mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
      eglInitialize(mDisplay, 0, 0);
      EGLint numConfigs;
      eglChooseConfig(mDisplay, attributes, &mConfig, 1, &numConfigs);
      _config = mConfig;
      _display = mDisplay;
      return(true);

  }

  bool WNSurfaceManagerAndroid::DestroyDisplay() {
      if(mDisplay != EGL_NO_DISPLAY)
      {
          eglTerminate(mDisplay);
          mDisplay = EGL_NO_DISPLAY;
      }
      return(true);
  }

  bool WNSurfaceManagerAndroid::SurfaceDestroyed() {
      DestroyDisplay();
      mValidSurface = false;
      return(true);
  }
}