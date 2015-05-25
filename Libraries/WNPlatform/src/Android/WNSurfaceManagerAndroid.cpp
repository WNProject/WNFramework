// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"


namespace wn {
  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::Initialize() {
     return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, memory::intrusive_ptr<surface>& _surface) {
      if(mValidSurface)
      {
          return(WNSurfaceManagerReturnCode::eWNResourceLimitReached);
      }
      memory::intrusive_ptr<WNSurfaceAndroid> ptr = memory::make_intrusive<WNSurfaceAndroid, WNSurfaceManagerAndroid&>(*this);

      if (!ptr->Initialize()) {
          return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
      }

      _surface = ptr;
      mValidSurface = wn_true;
      return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerReturnCode::type WNSurfaceManagerAndroid::Release() {
      return(WNSurfaceManagerReturnCode::ok);
  }

  WNSurfaceManagerAndroid::WNSurfaceManagerAndroid() :
      mDisplay(EGL_NO_DISPLAY),
      mConfig(wn_nullptr),
      mValidSurface(wn_false) {
  }


  wn_bool WNSurfaceManagerAndroid::InitializeDisplay(EGLDisplay& _display, EGLConfig& _config) {
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
      return(wn_true);

  }

  wn_bool WNSurfaceManagerAndroid::DestroyDisplay() {
      if(mDisplay != EGL_NO_DISPLAY)
      {
          eglTerminate(mDisplay);
          mDisplay = EGL_NO_DISPLAY;
      }
      return(wn_true);
  }

  wn_bool WNSurfaceManagerAndroid::SurfaceDestroyed() {
      DestroyDisplay();
      mValidSurface = wn_false;
      return(wn_true);
  }
}