// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__
#define __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__

#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/object_info.h"
#include "WNUtilities/inc/handle.h"

namespace wn {
namespace file_system {
namespace internal {

class file_windows final : public file {
public:
  file_windows(
      memory::allocator* _allocator, utilities::internal::handle&& _file_handle)
    : file(_allocator),
      m_file_handle(core::move(_file_handle)),
      m_mapped_memory(NULL),
      m_size({0}) {}

  file_windows(memory::allocator* _allocator,
      utilities::internal::handle&& _file_handle,
      utilities::internal::handle&& _file_mapping_handle, LPVOID _mapped_memory,
      LARGE_INTEGER _size)
    : file(_allocator),
      m_file_handle(core::move(_file_handle)),
      m_file_mapping_handle(core::move(_file_mapping_handle)),
      m_mapped_memory(_mapped_memory),
      m_size(_size) {}

  virtual ~file_windows() override {
    close();
  }

  virtual pointer data() override {
    return static_cast<pointer>(m_mapped_memory);
  }

  virtual const_pointer data() const override {
    return static_cast<const_pointer>(m_mapped_memory);
  }

  virtual object_info info() const override {
    return object_info {
        object_type::file,                       // type
        static_cast<size_type>(m_size.QuadPart)  // size
    };
  }

  virtual bool is_open() const override {
    return m_file_handle.is_valid();
  }

  virtual bool flush() override {
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

  virtual void close() override {
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

  utilities::internal::handle m_file_handle;
  utilities::internal::handle m_file_mapping_handle;
  LPVOID m_mapped_memory;
  LARGE_INTEGER m_size;
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_FILE_WINDOWS_H__
