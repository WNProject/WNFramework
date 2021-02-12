// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_EXECUTABLE_DATA_SYSTEM_INFO_H__
#define __WN_EXECUTABLE_DATA_SYSTEM_INFO_H__

#include "core/inc/types.h"

namespace wn {
namespace executable {

struct host_specific_data;

struct executable_data final {
  host_specific_data* host_data;
  const char* program_name;
  int32_t argc;
  char** argv;
};

}  // namespace executable
}  // namespace wn

#endif  // __WN_EXECUTABLE_DATA_SYSTEM_INFO_H__
