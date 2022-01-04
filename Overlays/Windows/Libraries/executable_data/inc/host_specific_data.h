// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_EXECUTABLE_DATA_WINDOWS_HOST_SPECIFIC_DATA_H__
#define __WN_EXECUTABLE_DATA_WINDOWS_HOST_SPECIFIC_DATA_H__

#include "core/inc/base.h"

namespace wn {
namespace executable {

struct host_specific_data final {
  const char* full_executable_path;
  HMODULE module;
};

}  // namespace executable
}  // namespace wn

#endif  // __WN_EXECUTABLE_DATA_WINDOWS_HOST_SPECIFIC_DATA_H__
