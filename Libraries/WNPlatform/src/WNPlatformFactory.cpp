// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNBase.h"
#include "WNPlatform/inc/WNPlatformFactory.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include "WNPlatform/inc/Internal/Windows/WNSurfaceManagerWindows.h"
#elif defined _WN_LINUX
    #include "WNPlatform/inc/Internal/XWindows/WNSurfaceManagerXWindows.h"
#elif defined _WN_ANDROID
    #include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#endif

wn::WNSurfaceManager* wn::WNPlatformFactory::CreateSurfaceManager() {
    #ifdef _WN_WINDOWS
        return(wn::memory::construct<wn::WNSurfaceManagerWindows>());
    #elif defined _WN_LINUX
        return(wn::memory::construct<wn::WNSurfaceManagerXWindows>());
    #elif defined _WN_ANDROID
        return(wn::memory::construct<wn::WNSurfaceManagerAndroid>());
    #endif
}
