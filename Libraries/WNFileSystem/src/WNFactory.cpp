// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/src/WNMemoryBackedMapping.h"
#include "WNFileSystem/src/WNUtilities.h"

#ifdef _WN_WINDOWS
#include "WNFileSystem/src/WNMappingWindows.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#elif defined _WN_ANDROID
#include "WNFileSystem/src/WNSystemPaths.h"
#elif defined _WN_POSIX
#include "WNFileSystem/src/WNSystemPaths.h"
#endif

#ifdef _WN_POSIX
#include "WNFileSystem/src/WNMappingPosix.h"
#endif

namespace wn {
namespace file_system {

mapping_ptr factory::make_mapping(
    const mapping_type _mapping_type, memory::allocator* _allocator) const {
  containers::string path(_allocator);
  bool cleanup = false;

  switch (_mapping_type) {
    case mapping_type::memory_backed:
      return memory::make_unique<memory_backed_mapping>(_allocator, _allocator);
    case mapping_type::scratch:
      if (!internal::get_scratch_path(path)) {
        return nullptr;
      }

      cleanup = true;

      break;
    default:
      return nullptr;
  }

  internal::sanitize_path(path);

#ifdef _WN_WINDOWS
  return memory::make_unique<internal::mapping_windows>(
      _allocator, std::move(path), _allocator, cleanup);
#elif defined _WN_POSIX
  return memory::make_unique<internal::mapping_posix>(
      _allocator, std::move(path), _allocator, cleanup);
#endif
}

}  // namespace file_system
}  // namespace wn
