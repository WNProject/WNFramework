// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_SYSTEM_TEMP_PATH_H__
#define __WN_FILE_SYSTEM_POSIX_SYSTEM_TEMP_PATH_H__

#include "WNContainers/inc/WNString.h"

namespace wn {
namespace executable {

struct executable_data;

}  // namespace executable

namespace memory {

class allocator;

}  // namespace memory

namespace file_system {
namespace internal {

containers::string get_temp_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data);

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_POSIX_SYSTEM_TEMP_PATH_H__
