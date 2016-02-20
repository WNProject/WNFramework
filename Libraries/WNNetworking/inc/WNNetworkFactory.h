// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_FACTORY_H__
#define __WN_NETWORKING_NETWORK_FACTORY_H__

#include "WNCore/inc/WNBase.h"

namespace WNNetworking {
class WNNetworkManager;

class WNNetworkFactory {
public:
  static WNNetworkManager* CreateNetworkManager();

private:
  WN_FORCE_INLINE WNNetworkFactory() {}
};
}

#endif  // __WN_NETWORKING_NETWORK_FACTORY_H__
