// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNFileSystem/src/WNMemoryBackedMapping.h"

#ifdef _WN_WINDOWS
#include "WNFileSystem/src/Windows/WNMappingWindows.h"
#include "WNFileSystem/src/Windows/WNSystemPaths.h"
#elif defined _WN_ANDROID
#include "WNFileSystem/src/Android/WNSystemPaths.h"
#elif defined _WN_POSIX
#include "WNFileSystem/src/Posix/WNSystemPaths.h"
#endif

#if defined _WN_POSIX
#include "WNFileSystem/src/Posix/WNMappingPosix.h"
#endif

namespace wn {
namespace file_system {

mapping_ptr factory::make_mapping(
    const mapping_type  _mapping_type, memory::allocator* _allocator) const {
  containers::string path(_allocator);
  bool cleanup = false;

  switch (_mapping_type ) {
    case mapping_type::scratch:
      if (!internal::get_scratch_path(path)) {
        return nullptr;
      }

      cleanup = true;

      break;
    case mapping_type::memory_backed:
      return memory::make_allocated_ptr<memory_backed_mapping>(
        _allocator, _allocator);
    default:
      return nullptr;
  }

  internal::sanitize_path(path);

#ifdef _WN_WINDOWS
  return memory::make_allocated_ptr<internal::mapping_windows>(
      _allocator, std::move(path), _allocator, cleanup);
#elif defined _WN_POSIX
  return memory::make_allocated_ptr<internal::mapping_posix>(
      _allocator, std::move(path), _allocator, cleanup);
#endif
}

}  // namespace file_system
}  // namespace wn
