// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/Posix/WNSystemPaths.h"

#include <sys/stat.h>
#include <time.h>

namespace wn {
namespace file_system {
namespace internal {
namespace {

bool get_temp_path(containers::string& _path) {
  static const char* vars[4] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

  for (const char* var : vars) {
    const char* path = ::getenv(var);

    if (path) {
      struct stat dstat;

      if (::stat(path, &dstat) == 0) {
        if (S_ISDIR(dstat.st_mode)) {
          _path = path;

          if (_path.size() > 0) {
            if (_path.back() != '/') {
              _path += '/';
            }

            return true;
          }
        }
      }
    }
  }

  struct stat dstat;

  if (::stat("/tmp", &dstat) == 0) {
    if (S_ISDIR(dstat.st_mode)) {
      _path = "/tmp/";

      return true;
    }
  }

  return false;
}

}  // anonymous namespace

bool get_scratch_path(containers::string& _path) {
  containers::string temp_path;

  if (get_temp_path(temp_path)) {
    char name_template[7] = {0};

    for (;;) {
      memory::strncpy(name_template, "XXXXXX", 6);

      if (::mktemp(name_template)) {
        containers::string path(temp_path + "scratch." + name_template + "/");
        static const mode_t mode = S_IRWXU | S_IRWXG;

        if (::mkdir(path.c_str(), mode) == 0) {
          _path = std::move(path);

          return true;
        } else if (errno == EEXIST) {
          continue;
        }
      }

      break;
    }
  }

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn