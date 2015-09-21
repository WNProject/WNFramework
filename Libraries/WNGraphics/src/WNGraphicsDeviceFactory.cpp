// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNGraphicsDeviceFactory.h"
#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNMemory/inc/WNBasic.h"

#ifdef _WN_WINDOWS
    #include "WNGraphics/inc/Internal/D3D11/WNGraphicsDeviceD3D11.h"
#elif defined _WN_LINUX
    #include "WNGraphics/inc/Internal/GLX/WNGraphicsDeviceGLX.h"
#elif defined _WN_ANDROID
    #include "WNGraphics/inc/Internal/EGL/WNGraphicsDeviceEGL.h"
#endif

WNGraphics::WNGraphicsDevice* WNGraphics::WNGraphicsDeviceFactory::CreateGraphicsDevice(WNGraphics::WNGraphicsResourceFactory* _resourceFactory) {
    #ifdef _WN_WINDOWS
        return(wn::memory::construct<WNGraphics::WNGraphicsDeviceD3D11>(_resourceFactory));
    #elif defined _WN_LINUX
        return(wn::memory::construct<WNGraphics::WNGraphicsDeviceGLX>(_resourceFactory));
    #elif defined _WN_ANDROID
        return(wn::memory::construct<WNGraphics::WNGraphicsDeviceEGL>(_resourceFactory));
    #endif
}
