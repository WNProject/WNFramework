// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNUtils/inc/Android/WNAppData.h"

namespace wn {

WNSurfaceAndroid::WNSurfaceAndroid(WNSurfaceManagerAndroid& _surfaceManager)
  : surface(), mManager(_surfaceManager), mExiting(false), mInitialized(false) {
  mDisplay = EGL_NO_DISPLAY;
  mSurface = EGL_NO_SURFACE;
  mConfig = 0;
}

WNSurfaceAndroid::~WNSurfaceAndroid() {
  if (mInitialized) {
    WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(
        WNUtils::WNAndroidEventPump::eDisplayCreated, NULL, NULL);
    WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(
        WNUtils::WNAndroidEventPump::eDisplayDestroyed, NULL, NULL);
  }
  mExiting = true;
  CleanupWindow(WNUtils::gAndroidApp);
  mManager.SurfaceDestroyed();
}

bool WNSurfaceAndroid::Initialize() {
  android_app* app = WNUtils::gAndroidApp;
  if (!app) {
    return (false);
  }

  WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(
      WNUtils::WNAndroidEventPump::eDisplayCreated,
      &WNSurfaceAndroid::HandleWindowCommand, this);
  WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(
      WNUtils::WNAndroidEventPump::eDisplayDestroyed,
      &WNSurfaceAndroid::HandleWindowCommand, this);
  mCreationSemaphore.wait();
  mInitialized = (mSurface != EGL_NO_SURFACE);
  return (true);
}

void WNSurfaceAndroid::HandleWindowCommand(
    WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app,
    uint32_t _val, void* appData) {
  WNSurfaceAndroid* surface = reinterpret_cast<WNSurfaceAndroid*>(appData);
  switch (_msg) {
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

bool WNSurfaceAndroid::InitializeWindow(android_app* state) {
  if (!mManager.InitializeDisplay(mDisplay, mConfig)) {
    mCreationSemaphore.notify();
    return (false);
  }

  EGLint format;
  if (EGL_FALSE ==
      eglGetConfigAttrib(mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &format)) {
    mCreationSemaphore.notify();
    return (false);
  }
  if (0 > ANativeWindow_setBuffersGeometry(state->window, 0, 0, format)) {
    mCreationSemaphore.notify();
    return (false);
  }
  if (EGL_NO_SURFACE == (mSurface = eglCreateWindowSurface(
                             mDisplay, mConfig, state->window, NULL))) {
    mCreationSemaphore.notify();
    return (false);
  }

  EGLint w, h;
  eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
  eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);
  mWidth = w;
  mHeight = h;
  mCreationSemaphore.notify();
  return (true);
}

bool WNSurfaceAndroid::CleanupWindow(android_app* state) {
  FireCallback(eWNSETDestroyed);  // callback also not locked
  if (mDisplay != EGL_NO_DISPLAY) {
    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (mSurface) {
      eglDestroySurface(mDisplay, mSurface);
    }
  }
  mSurface = EGL_NO_SURFACE;
  mDisplay = EGL_NO_DISPLAY;
  mConfig = nullptr;
  if (!mManager.DestroyDisplay()) {
    return (false);
  }
  return (true);
}
WNSurfaceNativeHandle WNSurfaceAndroid::GetNativeHandle() const {
  return (mSurface);
}

surface::WNSurfaceError WNSurfaceAndroid::Resize(uint32_t, uint32_t) {
  return (ok);
}

surface::WNSurfaceError WNSurfaceAndroid::Move(uint32_t, uint32_t) {
  return (ok);
}

bool WNSurfaceAndroid::IsFullscreen() const {
  return (true);
}

surface::WNSurfaceError WNSurfaceAndroid::SetFullscreen(bool) {
  return (ok);
}

uint32_t WNSurfaceAndroid::GetWidth() const {
  return (mWidth);
}

uint32_t WNSurfaceAndroid::GetHeight() const {
  return (mHeight);
}

uint32_t WNSurfaceAndroid::GetX() const {
  return (0);
}

uint32_t WNSurfaceAndroid::GetY() const {
  return (0);
}
}
