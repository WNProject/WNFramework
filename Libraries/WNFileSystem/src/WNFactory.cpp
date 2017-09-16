// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/src/WNMemoryBackedMapping.h"
#include "WNFileSystem/src/WNSystemMapping.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {

mapping_ptr factory::make_mapping(
    memory::allocator* _allocator, const mapping_type _mapping_type) const {
  switch (_mapping_type) {
    case mapping_type::memory_backed:
      return memory::make_unique<memory_backed_mapping>(_allocator, _allocator);
    case mapping_type::scratch: {
      containers::string path(_allocator);

      if (internal::get_scratch_path(path)) {
        internal::sanitize_path(path);

        return memory::make_unique<internal::system_mapping>(
            _allocator, _allocator, core::move(path), true);
      }
    }
    default:
      break;
  }

  return nullptr;
}

}  // namespace file_system
}  // namespace wn
