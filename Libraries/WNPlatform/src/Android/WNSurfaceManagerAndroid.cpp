// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"

using namespace WNPlatform;
using namespace WNConcurrency;

WNSurfaceManagerReturnCode::Type WNSurfaceManagerAndroid::Initialize() {
   return(WNSurfaceManagerReturnCode::eWNOK);
}

WNSurfaceManagerReturnCode::Type WNSurfaceManagerAndroid::CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNResourcePointer<WNSurface>& _surface) {
    if(mValidSurface)
    {
        return(WNSurfaceManagerReturnCode::eWNResourceLimitReached);
    }
    WNResourcePointer<WNSurfaceAndroid> ptr = WNAllocateResource<WNSurfaceAndroid, WNSurfaceManagerAndroid&>(*this);

    if (!ptr->Initialize()) {
        return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
    }

    _surface = ptr;
    mValidSurface = WN_TRUE;
    return(WNSurfaceManagerReturnCode::eWNOK);
}

WNSurfaceManagerReturnCode::Type WNSurfaceManagerAndroid::Release() {
    return(WNSurfaceManagerReturnCode::eWNOK);
}

WNSurfaceManagerAndroid::WNSurfaceManagerAndroid() :
    mDisplay(EGL_NO_DISPLAY),
    mConfig(WN_NULL),
    mValidSurface(WN_FALSE) {
}


WN_BOOL WNSurfaceManagerAndroid::InitializeDisplay(EGLDisplay& _display, EGLConfig& _config) {
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
    return(WN_TRUE);

}

WN_BOOL WNSurfaceManagerAndroid::DestroyDisplay() {
    if(mDisplay != EGL_NO_DISPLAY)
    {
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }
    return(WN_TRUE);
}

WN_BOOL WNSurfaceManagerAndroid::SurfaceDestroyed() {
    DestroyDisplay();
    mValidSurface = WN_FALSE;
    return(WN_TRUE);
}
