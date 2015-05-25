// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNUtils/inc/Android/WNAppData.h"

namespace wn {

  WNSurfaceAndroid::WNSurfaceAndroid(WNSurfaceManagerAndroid& _surfaceManager) :
      surface(),
      mManager(_surfaceManager),
      mExiting(wn_false),
      mInitialized(wn_false) {
      mDisplay = EGL_NO_DISPLAY;
      mSurface = EGL_NO_SURFACE; 
      mConfig = 0;
  }

  WNSurfaceAndroid::~WNSurfaceAndroid() {
      if(!mInitialized) {
          WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayCreated, NULL, NULL);
          WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayDestroyed, NULL, NULL);
      }
      mExiting = wn_true;
      CleanupWindow(WNUtils::gAndroidApp);
      mManager.SurfaceDestroyed();
  }

  wn_bool WNSurfaceAndroid::Initialize() {
      android_app* app = WNUtils::gAndroidApp;
      if(!app)
      {
          return(wn_false);
      }

      WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayCreated, &WNSurfaceAndroid::HandleWindowCommand, this);
      WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayDestroyed, &WNSurfaceAndroid::HandleWindowCommand, this);
      mCreationSemaphore.wait();
      mInitialized = (mSurface != EGL_NO_SURFACE);
      return(wn_true);
  }

  wn_void WNSurfaceAndroid::HandleWindowCommand(WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app, wn_uint32 _val, wn_void* appData) {
      WNSurfaceAndroid* surface = reinterpret_cast<WNSurfaceAndroid*>(appData);
      switch(_msg){
          case WNUtils::WNAndroidEventPump::eDisplayCreated:
              surface->InitializeWindow(_app);
              break;
          case WNUtils::WNAndroidEventPump::eDisplayDestroyed:
              surface->CleanupWindow(_app);
              break;
          default:
              break;
      }
  }

  wn_bool WNSurfaceAndroid::InitializeWindow(android_app* state) {
      if(!mManager.InitializeDisplay(mDisplay, mConfig)) {
          mCreationSemaphore.notify();
          return(wn_false);
      }
      
      EGLint format;
      if(EGL_FALSE == eglGetConfigAttrib(mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &format)) {
          mCreationSemaphore.notify();
          return(wn_false);
      }
      if(0 > ANativeWindow_setBuffersGeometry(state->window, 0, 0, format)) {
          mCreationSemaphore.notify();
          return(wn_false);
      }
      if(EGL_NO_SURFACE == (mSurface = eglCreateWindowSurface(mDisplay, mConfig, state->window, NULL))) {
          mCreationSemaphore.notify();
          return(wn_false); 
      }
      
      EGLint w, h;
      eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
      eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);
      mWidth = w;
      mHeight = h;
      mCreationSemaphore.notify();
      return(wn_true); 
  }

  wn_bool WNSurfaceAndroid::CleanupWindow(android_app* state) {
      FireCallback(eWNSETDestroyed); //callback also not locked
      if(mDisplay != EGL_NO_DISPLAY) {
          eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
          if(mSurface) {
              eglDestroySurface(mDisplay, mSurface);
          }
      }
      mSurface = EGL_NO_SURFACE;
      mDisplay = EGL_NO_DISPLAY;
      mConfig = wn_nullptr;
      if(!mManager.DestroyDisplay()) {
         return(wn_false); 
      }
      return(wn_true);
  }
  WNSurfaceNativeHandle WNSurfaceAndroid::GetNativeHandle() const {
      return(mSurface);
  }

  surface::WNSurfaceError WNSurfaceAndroid::Resize(wn_uint32, wn_uint32) {
      return(ok);
  }

  surface::WNSurfaceError WNSurfaceAndroid::Move(wn_uint32, wn_uint32) {
      return(ok);
  }

  wn_bool WNSurfaceAndroid::IsFullscreen() const {
      return(wn_true);
  }

  surface::WNSurfaceError WNSurfaceAndroid::SetFullscreen(wn_bool) {
      return(ok);
  }

  wn_uint32 WNSurfaceAndroid::GetWidth() const {
      return(mWidth);
  }

  wn_uint32 WNSurfaceAndroid::GetHeight() const {
      return(mHeight);
  }

  wn_uint32 WNSurfaceAndroid::GetX() const {
      return(0);
  }

  wn_uint32 WNSurfaceAndroid::GetY() const {
      return(0);
  }
}