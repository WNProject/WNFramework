// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNNetworkFactory.h"
#include "WNMemory/inc/WNBasic.h"

#ifdef _WN_WINDOWS
#include "WNNetworking/inc/Internal/Windows/WNNetworkManagerWindows.h"
#elif defined _WN_LINUX || defined _WN_ANDROID
#include "WNNetworking/inc/Internal/Linux/WNNetworkManagerLinux.h"
#endif

using namespace WNNetworking;

WNNetworkManager* WNNetworkFactory::CreateNetworkManager() {
#ifdef _WN_WINDOWS
  return (wn::memory::construct<WNNetworkManagerWindows>());
#elif defined _WN_LINUX || defined _WN_ANDROID
  return (wn::memory::construct<WNNetworkManagerLinux>());
#endif
}
