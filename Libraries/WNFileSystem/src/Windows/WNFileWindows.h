// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__
#define __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__

#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/src/Windows/WNHandle.h"

namespace wn {
namespace file_system {
namespace internal {

class file_windows final : public file {
public:
  WN_FORCE_INLINE file_windows(
      memory::allocator* _allocator, handle&& _file_handle)
    : file(_allocator),
      m_file_handle(std::move(_file_handle)),
      m_mapped_memory(NULL),
      m_size({0}) {}

  WN_FORCE_INLINE file_windows(memory::allocator* _allocator,
      handle&& _file_handle, handle&& _file_mapping_handle,
      LPVOID _mapped_memory, LARGE_INTEGER _size)
    : file(_allocator),
      m_file_handle(std::move(_file_handle)),
      m_file_mapping_handle(std::move(_file_mapping_handle)),
      m_mapped_memory(_mapped_memory),
      m_size(_size) {}

  WN_FORCE_INLINE virtual ~file_windows() override {
    close();
  }

  WN_FORCE_INLINE virtual pointer data() override {
    return static_cast<pointer>(m_mapped_memory);
  }

  WN_FORCE_INLINE virtual const_pointer data() const override {
    return static_cast<const_pointer>(m_mapped_memory);
  }

  WN_FORCE_INLINE virtual size_type size() const override {
    return static_cast<size_type>(m_size.QuadPart);
  }

  WN_FORCE_INLINE virtual bool is_open() const override {
    return m_file_handle.is_valid();
  }

  WN_FORCE_INLINE virtual bool flush() override {
    if (is_open()) {
      if (m_mapped_memory != NULL) {
        if (::FlushViewOfFile(m_mapped_memory, 0) != TRUE) {
          return false;
        }
      }

      return (::FlushFileBuffers(m_file_handle.value()) == TRUE);
    }

    return false;
  }

  virtual bool resize(const size_type _size) override;

  WN_FORCE_INLINE virtual void close() override {
    if (is_open()) {
      flush();
      unmap();

      m_file_handle.dispose();

      m_size.QuadPart = 0;
    }
  }

private:
  void unmap();
  bool remap();

  handle m_file_handle;
  handle m_file_mapping_handle;
  LPVOID m_mapped_memory;
  LARGE_INTEGER m_size;
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__
