// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNMemory/inc/WNAllocator.h"

#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {
namespace {

containers::string get_temp_path(memory::allocator* _allocator) {
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

  struct stat dstat;

  if (::stat("/tmp", &dstat) == 0) {
    if (S_ISDIR(dstat.st_mode)) {
      return containers::string(_allocator, "/tmp/");
    }
  }

  return nullptr;
}

}  // anonymous namespace

containers::string get_scratch_path(memory::allocator* _allocator) {
  containers::string temp_path(get_temp_path(_allocator));

  if (!temp_path.empty()) {
    for (;;) {
      char name_template[9] = {0};
      uint32_t t = ::rand() % 100000000;
      int pos = 0;

      while (t) {
        name_template[++pos] = (t % 10 + '0');
        t -= t % 10;
        t /= 10;
      }

      containers::string path(temp_path + "scratch." + name_template + "/");
      static const mode_t mode = S_IRWXU | S_IRWXG;

      if (::mkdir(path.data(), mode) == 0) {
        return core::move(path);
      } else if (errno == EEXIST) {
        continue;
      }

      break;
    }
  }

  return nullptr;
}

containers::string get_executable_path(memory::allocator* _allocator) {
  char raw_path[PATH_MAX] = {0};

  const ssize_t result =
      ::readlink("/proc/self/exe", raw_path, sizeof(raw_path));

  if (result != -1) {
    containers::string_view view(raw_path, result);
    const size_t pos = view.find_last_of('/');

    if (pos != containers::string_view::npos) {
      view.remove_suffix(view.size() - pos);

      return view.to_string(_allocator);
    }
  }

  return nullptr;
}

containers::string get_current_working_path(memory::allocator* _allocator) {
  containers::dynamic_array<char> buffer(_allocator);
  size_t size = static_cast<size_t>(PATH_MAX);

  for (;;) {
    buffer.resize(size);

    const char* result = ::getcwd(buffer.data(), buffer.size());

    if (result == NULL) {
      if (errno == ERANGE) {
        size *= 2;
      } else {
        return nullptr;
      }
    } else if (*result == '(' || *result != '/') {
      return nullptr;
    } else {
      return containers::string(_allocator, buffer.data());
    }
  }
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
