// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "core/inc/types.h"

#include <libproc.h>
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

containers::string get_executable_path(
    memory::allocator* _allocator, const executable::executable_data*) {
  pid_t pid = getpid();
  char raw_path[PATH_MAX];

  int result = proc_pidpath(pid, raw_path, sizeof(raw_path));

  if (result > 0) {
    containers::string_view view(raw_path, result);
    const size_t pos = view.find_last_of('/');

    if (pos != containers::string_view::npos) {
      view.remove_suffix(view.size() - pos);

      return view.to_string(_allocator);
    }
  }

  return nullptr;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
