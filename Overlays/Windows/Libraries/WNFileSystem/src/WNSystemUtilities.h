// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_SYSTEM_UTILITIES_H__
#define __WN_FILE_SYSTEM_WINDOWS_SYSTEM_UTILITIES_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/src/WNHelpers.h"

namespace wn {
namespace file_system {
namespace internal {

inline DWORD get_attributes(const containers::string_view _path) {
  containers::array<WCHAR, MAX_PATH> unicode_buffer = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(_path, unicode_buffer.data(), unicode_buffer_size)) {
    return INVALID_FILE_ATTRIBUTES;
  }

  return ::GetFileAttributesW(unicode_buffer.data());
}

inline bool exists_file(const containers::string& _path) {
  const DWORD attributes = get_attributes(_path);

  return (attributes != INVALID_FILE_ATTRIBUTES &&
          !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

inline bool exists_directory(const containers::string& _path) {
  const DWORD attributes = get_attributes(_path);

  return (attributes != INVALID_FILE_ATTRIBUTES &&
          (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_SYSTEM_UTILITIES_H__
