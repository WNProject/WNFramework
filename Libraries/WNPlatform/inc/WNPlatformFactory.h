// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_PLATFORM_FACTORY_H__
#define __WN_PLATFORM_PLATFORM_FACTORY_H__

#include "WNCore/inc/WNBase.h"

namespace WNPlatform {
    class WNSurfaceManager;

    class WNPlatformFactory {
    public:
        static WNSurfaceManager* CreateSurfaceManager();

    private:
        WN_FORCE_INLINE WNPlatformFactory() {}
    };
}

#endif // __WN_PLATFORM_PLATFORM_FACTORY_H__
