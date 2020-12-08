// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_SYSTEM_UTILITIES_H__
#define __WN_FILE_SYSTEM_POSIX_SYSTEM_UTILITIES_H__

#include "WNContainers/inc/WNStringView.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {

inline bool exists_file(const containers::string& _path) {
  struct stat fstates;

  if (::stat(_path.data(), &fstates) == 0) {
    if (S_ISREG(fstates.st_mode)) {
      return true;
    }
  }

  return false;
}

inline bool exists_directory(const containers::string& _path) {
  struct stat dstates;

  if (::stat(_path.data(), &dstates) == 0) {
    if (S_ISDIR(dstates.st_mode)) {
      return true;
    }
  }

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_POSIX_SYSTEM_UTILITIES_H__
