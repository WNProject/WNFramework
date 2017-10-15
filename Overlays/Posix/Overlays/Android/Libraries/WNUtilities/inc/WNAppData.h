// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_UTILITIES_APP_DATA_H__
#define __WN_UTILITIES_APP_DATA_H__

#include "WNCore/inc/WNTypes.h"

#include <android_native_app_glue.h>
#include <atomic>

namespace wn {
namespace utilities {

extern android_app* gAndroidApp;
extern ALooper* gMainLooper;
extern std::atomic<bool> gWindowInitialized;
}  // namespace utilities
}  // namespace wn

#endif  //__WN_UTILITIES_APP_DATA_H__
