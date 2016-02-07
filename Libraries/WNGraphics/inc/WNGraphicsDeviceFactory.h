// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__
#define __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__

namespace WNGraphics {
class WNGraphicsDevice;
class WNGraphicsResourceFactory;

class WNGraphicsDeviceFactory {
public:
  static WNGraphicsDevice* CreateGraphicsDevice(
      WNGraphicsResourceFactory* _resourceFactory);
};
}

#endif  // __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__