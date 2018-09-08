// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemTempPath.h"
#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/types.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNMemory/inc/allocator.h"
#include "executable_data/inc/executable_data.h"
#include "executable_entry/inc/executable_entry.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {

containers::string get_temp_path(
    memory::allocator* _allocator, const executable::executable_data*) {
  static const char* vars[4] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

  for (const char* var : vars) {
    const char* current_path = ::getenv(var);

    if (current_path) {
      struct stat dstat;

      if (::stat(current_path, &dstat) == 0) {
        if (S_ISDIR(dstat.st_mode)) {
          containers::string path(_allocator, current_path);

          internal::sanitize_path(path);

          if (!path.empty()) {
            if (path.back() != '/') {
              path += '/';
            }

            return core::move(path);
          }
        }
      }
    }
  }

  static const char default_path[] = "/tmp/";
  struct stat dstat;

  if (::stat(default_path, &dstat) == 0) {
    if (S_ISDIR(dstat.st_mode)) {
      return containers::string(_allocator, default_path);
    }
  }

  return nullptr;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
