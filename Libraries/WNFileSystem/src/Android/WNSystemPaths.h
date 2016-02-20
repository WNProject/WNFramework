// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_ANDROID_SYSTEM_PATHS_H__
#define __WN_FILE_SYSTEM_ANDROID_SYSTEM_PATHS_H__

#include "WNContainers/inc/WNString.h"

namespace wn {
namespace file_system {
namespace internal {

bool get_scratch_path(containers::string& _path);

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_ANDROID_SYSTEM_PATHS_H__