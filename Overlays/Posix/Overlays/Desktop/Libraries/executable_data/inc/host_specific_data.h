// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_EXECUTABLE_DATA_LINUX_HOST_SPECIFIC_DATA_H__
#define __WN_EXECUTABLE_DATA_LINUX_HOST_SPECIFIC_DATA_H__

#include "WNCore/inc/base.h"

namespace wn {
namespace executable {

struct host_specific_data final {
  const char* full_executable_path;
};

}  // namespace executable
}  // namespace wn

#endif  // __WN_EXECUTABLE_DATA_LINUX_HOST_SPECIFIC_DATA_H__
