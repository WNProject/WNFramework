// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNFileWindows.h"

namespace wn {
namespace file_system {
namespace internal {

bool file_windows::resize(const size_type _size) {
  if (is_open()) {
    const size_type current_size = size();

    if (current_size != _size) {
      LARGE_INTEGER size;

      size.QuadPart = _size;

      if (current_size > _size) {
        unmap();

        const DWORD set_file_pointer_result = ::SetFilePointer(
            m_file_handle.value(), size.LowPart, &size.HighPart, FILE_BEGIN);

        if (set_file_pointer_result == INVALID_SET_FILE_POINTER) {
          const bool result = remap();

#ifndef _WN_DEBUG
          (void)result;
#endif

          WN_DEBUG_ASSERT(
              result, "failed to restore previous memory map for file");

          return false;
        }

        if (::SetEndOfFile(m_file_handle.value()) == FALSE) {
          const bool result = remap();

#ifndef _WN_DEBUG
          (void)result;
#endif

          WN_DEBUG_ASSERT(
              result, "failed to restore previous memory map for file");

          return false;
        }
      }

      if (size.QuadPart != 0) {
        utilities::internal::handle file_mapping_handle(
            ::CreateFileMappingW(m_file_handle.value(), nullptr, PAGE_READWRITE,
                size.HighPart, size.LowPart, nullptr));

        if (!file_mapping_handle.is_valid()) {
          return false;
        }

        LPVOID memory_mapped = ::MapViewOfFile(
            file_mapping_handle.value(), FILE_MAP_WRITE, 0, 0, 0);

        if (memory_mapped == nullptr) {
          return false;
        }

        unmap();

        m_file_mapping_handle = core::move(file_mapping_handle);
        m_mapped_memory = memory_mapped;
      } else {
        unmap();
      }
    }

    m_size.QuadPart = _size;

    return true;
  }

  return false;
}

void file_windows::unmap() {
  if (m_mapped_memory != nullptr) {
    const BOOL result = ::UnmapViewOfFile(m_mapped_memory);

    m_mapped_memory = nullptr;

#ifndef _WN_DEBUG
    (void)result;
#endif

    WN_DEBUG_ASSERT(result == TRUE, "failed to unmap memory mapped file view");

    m_file_mapping_handle.dispose();
  }
}

bool file_windows::remap() {
  if (m_size.QuadPart != 0) {
    utilities::internal::handle file_mapping_handle(
        ::CreateFileMappingW(m_file_handle.value(), nullptr, PAGE_READWRITE,
            m_size.HighPart, m_size.LowPart, nullptr));

    if (!file_mapping_handle.is_valid()) {
      return false;
    }

    LPVOID memory_mapped =
        ::MapViewOfFile(file_mapping_handle.value(), FILE_MAP_WRITE, 0, 0, 0);

    if (memory_mapped == nullptr) {
      return false;
    }

    m_file_mapping_handle = core::move(file_mapping_handle);
    m_mapped_memory = memory_mapped;
  }

  return true;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
