// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/types.h"
#include "WNFileSystem/src/WNSystemPaths.h"

#include <limits.h>
#include <unistd.h>

namespace wn {
namespace executable {

struct executable_data;

}  // namespace executable

namespace memory {

class allocator;

}  // namespace memory

namespace file_system {
namespace internal {
containers::string get_dev_assets_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data) {
  return get_current_working_path(_allocator, _executable_data);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
