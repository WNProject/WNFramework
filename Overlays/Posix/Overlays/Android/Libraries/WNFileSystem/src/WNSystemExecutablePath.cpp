// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/src/WNSystemPaths.h"

namespace wn {
namespace entry {

struct system_data;

}  // namespace entry

namespace memory {

class allocator;

}  // namespace memory

namespace file_system {
namespace internal {

containers::string get_executable_path(
    memory::allocator* _allocator, const entry::system_data* _system_data) {
  return get_current_working_path(_allocator, _system_data);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
