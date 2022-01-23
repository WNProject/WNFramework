// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/WNFilePosix.h"

#if defined _WN_HAS_FALLOCATE || defined _WN_HAS_POSIX_FALLOCATE
#include <fcntl.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace wn {
namespace file_system {
namespace internal {

bool file_posix::resize(const size_type _size) {
  if (is_open()) {
    const size_type current_size = size();

    if (current_size != _size) {
      void* mapped_memory = NULL;
      const bool make_bigger = _size > current_size;

      if (make_bigger) {
#ifdef _WN_HAS_FALLOCATE
        const int result = ::fallocate(m_file_descriptor.value(), 0, 0, _size);
#elif defined _WN_HAS_POSIX_FALLOCATE
        const int result =
            ::posix_fallocate(m_file_descriptor.value(), 0, _size);
#else
        const int result = ::ftruncate(m_file_descriptor.value(), _size);
#endif

        if (result != 0) {
          return false;
        }
      }

      if (_size == 0) {
        if (::munmap(m_mapped_memory, current_size) != 0) {
          return false;
        }
      } else if (current_size == 0) {
        mapped_memory = ::mmap(NULL, _size, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_NORESERVE, m_file_descriptor.value(), 0);

        if (mapped_memory == MAP_FAILED) {
          return false;
        }
      } else {
#ifdef _WN_HAS_MREMAP
        mapped_memory =
            ::mremap(m_mapped_memory, current_size, _size, MREMAP_MAYMOVE);
#else
        mapped_memory = ::mmap(NULL, _size, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_NORESERVE, m_file_descriptor.value(), 0);
#endif

        if (mapped_memory == MAP_FAILED) {
          return false;
        }

#ifndef _WN_HAS_MREMAP
        if (::munmap(m_mapped_memory, current_size) != 0) {
          WN_RELEASE_ASSERT(::munmap(mapped_memory, _size) == 0,
              "failed to cleanup temporrary memory map");

          return false;
        }
#endif
      }

      if (!make_bigger) {
        if (::ftruncate(m_file_descriptor.value(), _size) != 0) {
          return false;
        }
      }

      m_mapped_memory = mapped_memory;
      m_size = _size;
    }

    return true;
  }

  return false;
}

bool file_posix::flush() {
  if (::msync(m_mapped_memory, m_size, MS_SYNC | MS_INVALIDATE) != 0) {
    return false;
  }

  return (::fsync(m_file_descriptor.value()) == 0);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
