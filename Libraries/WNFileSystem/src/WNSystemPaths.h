// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_SYSTEM_PATHS_H__
#define __WN_FILE_SYSTEM_SYSTEM_PATHS_H__

#include "WNContainers/inc/WNString.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace executable {

struct executable_data;

}  // namespace executable

namespace file_system {
namespace internal {

containers::string get_scratch_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data);
containers::string get_dev_assets_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data);
containers::string get_executable_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data);
containers::string get_current_working_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data);

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_SYSTEM_PATHS_H__
