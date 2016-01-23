// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNMapping.h"
#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {

bool mapping::sanitize_and_validate_path(
    const containers::string_view _path, containers::string& _full_path) const {
  containers::string path(_path.data(), _path.size(), m_allocator);

  internal::sanitize_path(path);

  if (internal::validate_relative_path(path)) {
    _full_path.append(m_root_path).append(path);

    return true;
  }

  return false;
}

}  // namespace file_system
}  // namespace wn
