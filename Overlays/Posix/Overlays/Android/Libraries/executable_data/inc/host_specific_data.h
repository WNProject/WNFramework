// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_EXECUTABLE_DATA_ANDROID_HOST_SPECIFIC_DATA_H__
#define __WN_EXECUTABLE_DATA_ANDROID_HOST_SPECIFIC_DATA_H__

#include "core/inc/base.h"

#include <android_native_app_glue.h>
#include <atomic>

namespace wn {
namespace utilities {

class WNAndroidEventPump;

}  // namespace utilities

namespace executable {

struct host_specific_data final {
  struct android_app* android_app;
  const char* package_name;
  std::atomic<bool>* window_initialized;
  utilities::WNAndroidEventPump* event_pump;
};

}  // namespace executable
}  // namespace wn

#endif  // __WN_EXECUTABLE_DATA_ANDROID_HOST_SPECIFIC_DATA_H__
