// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_HELPERS_H__
#define __WN_FILE_SYSTEM_WINDOWS_HELPERS_H__

#include "WNContainers/inc/WNString.h"

namespace wn {
namespace file_system {
namespace internal {

inline bool convert_to_unicode(
    const containers::string_view _path, WCHAR* _buffer, DWORD& _buffer_size) {
  const int path_size = static_cast<int>(_buffer_size);
  const int converted_path_size = ::MultiByteToWideChar(CP_UTF8, 0,
      _path.data(), static_cast<int>(_path.size()), _buffer, path_size);

  if (converted_path_size > 0 && converted_path_size < path_size) {
    _buffer_size = converted_path_size;

    return true;
  }

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_HELPERS_H__
