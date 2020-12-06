// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/WNMappingPosix.h"

#include "WNFileSystem/src/WNFilePosix.h"
#include "WNFileSystem/src/WNSystemUtilities.h"
#include "core/inc/assert.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _WN_HAS_NFTW
#include <ftw.h>
#elif defined _WN_HAS_READDIR
#include <dirent.h>
#endif

namespace wn {
namespace file_system {
namespace internal {

mapping_posix::~mapping_posix() {
  if (m_cleanup) {
    const bool result = recursive_remove_directory(m_root_path);

    WN_RELEASE_ASSERT(result, "unable to cleanup mapping root directory");
  }
}

bool mapping_posix::exists_file(const containers::string_view _path) const {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return false;
  }

  return internal::exists_file(path);
}

bool mapping_posix::exists_directory(
    const containers::string_view _path) const {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return false;
  }

  return internal::exists_directory(path);
}

file_ptr mapping_posix::create_file(
    const containers::string_view _path, result& _result) {
  containers::string path(m_allocator);

  if (sanitize_and_validate_path(_path, path)) {
    static const int flags = O_RDWR | O_CREAT | O_EXCL;
    static const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    file_descriptor fdescriptor(::open(path.c_str(), flags, mode));

    if (fdescriptor.is_valid()) {
      _result = result::ok;

      return memory::make_intrusive<file_posix>(
          m_allocator, m_allocator, core::move(fdescriptor));
    }
  }

  _result = result::fail;

  return nullptr;
}

result mapping_posix::create_directory(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (sanitize_and_validate_path(_path, path)) {
    static const mode_t mode = S_IRWXU | S_IRWXG;

    if (::mkdir(path.c_str(), mode) == 0) {
      return result::ok;
    }
  }

  return result::fail;
}

file_ptr mapping_posix::open_file(
    const containers::string_view _path, result& _result) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    _result = result::fail;

    return nullptr;
  }

  file_descriptor fdescriptor(::open(path.c_str(), O_RDWR));

  if (!fdescriptor.is_valid()) {
    _result = result::fail;

    return nullptr;
  }

  struct stat fstates;

  if (::fstat(fdescriptor.value(), &fstates) == -1) {
    _result = result::fail;

    return nullptr;
  }

  if (!S_ISREG(fstates.st_mode)) {
    _result = result::fail;

    return nullptr;
  }

  const size_t file_size = static_cast<size_t>(fstates.st_size);

  if (file_size > 0) {
    void* mapped_memory = ::mmap(NULL, file_size, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_NORESERVE, fdescriptor.value(), 0);

    if (mapped_memory == MAP_FAILED) {
      _result = result::fail;

      return nullptr;
    }

    _result = result::ok;

    return memory::make_intrusive<file_posix>(m_allocator, m_allocator,
        core::move(fdescriptor), mapped_memory, file_size);
  } else {
    _result = result::ok;

    return memory::make_intrusive<file_posix>(
        m_allocator, m_allocator, core::move(fdescriptor));
  }
}

result mapping_posix::delete_file(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (sanitize_and_validate_path(_path, path)) {
    if (::unlink(path.c_str()) == 0) {
      return result::ok;
    }
  }

  return result::fail;
}

result mapping_posix::delete_directory(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (sanitize_and_validate_path(_path, path)) {
    if (recursive_remove_directory(path)) {
      return result::ok;
    }
  }

  return result::fail;
}

bool mapping_posix::recursive_remove_directory(
    const containers::string& _path) {
#ifdef _WN_HAS_NFTW
  static auto predicate = [](const char* _path, const struct stat* _s,
                              const int _flag, struct FTW* _f) -> int {
    (void)_s;
    (void)_f;

    int status;

    switch (_flag) {
      case FTW_DP:
        status = ::rmdir(_path);

        break;
      default:
        status = ::unlink(_path);
    }

    return status;
  };

  return (::nftw(_path.c_str(), predicate, FOPEN_MAX, FTW_DEPTH) == 0);
#else
  DIR* dir;
  struct dirent* entry;

  if (!(dir = ::opendir(_path.c_str()))) {
    return false;
  }

  if (!(entry = ::readdir(dir))) {
    if (::closedir(dir) != 0) {
      WN_DEBUG_ASSERT(false, "failed to close handle to directory");
    }

    return false;
  }

  do {
    const char* entry_name = entry->d_name;

    if (memory::strncmp(entry_name, ".", 1) != 0 &&
        memory::strncmp(entry_name, "..", 2) != 0) {
      containers::string sub_path(_path.data(), _path.size(), m_allocator);

      sub_path += entry_name;

      if (entry->d_type == DT_DIR) {
        if (!recursive_remove_directory(sub_path)) {
          break;
        }
      } else if (::unlink(sub_path.c_str()) != 0) {
        break;
      }
    }
  } while (entry = ::readdir(dir));

  if (::closedir(dir) != 0) {
    WN_DEBUG_ASSERT(false, "failed to close handle to directory");

    return false;
  }

  return (::rmdir(_path.c_str()) == 0);
#endif
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
