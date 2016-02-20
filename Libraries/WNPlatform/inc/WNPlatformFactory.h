// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_PLATFORM_FACTORY_H__
#define __WN_PLATFORM_PLATFORM_FACTORY_H__

#include "WNCore/inc/WNBase.h"

namespace wn {
class WNSurfaceManager;

class WNPlatformFactory {
public:
  static WNSurfaceManager* CreateSurfaceManager();

private:
  WN_FORCE_INLINE WNPlatformFactory() {}
};
}

#endif  // __WN_PLATFORM_PLATFORM_FACTORY_H__
