// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_FILE_POSIX_H__
#define __WN_FILE_SYSTEM_POSIX_FILE_POSIX_H__

#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/src/WNFileDescriptor.h"

#include <sys/mman.h>

namespace wn {
namespace file_system {
namespace internal {

class file_posix : public file {
public:
  file_posix(memory::allocator* _allocator, file_descriptor&& _file_descriptor)
    : file(_allocator),
      m_file_descriptor(core::move(_file_descriptor)),
      m_mapped_memory(NULL),
      m_size(0) {}

  file_posix(memory::allocator* _allocator, file_descriptor&& _file_descriptor,
      void* _mapped_memory, size_t _size)
    : file(_allocator),
      m_file_descriptor(core::move(_file_descriptor)),
      m_mapped_memory(_mapped_memory),
      m_size(_size) {}

  virtual ~file_posix() override {
    close();
  }

  virtual pointer data() override {
    return reinterpret_cast<pointer>(m_mapped_memory);
  }

  virtual const_pointer data() const override {
    return reinterpret_cast<const_pointer>(m_mapped_memory);
  }

  virtual object_info info() const override {
    return object_info{
        object_type::file,              // type
        static_cast<size_type>(m_size)  // size
    };
  }

  virtual bool is_open() const override {
    return m_file_descriptor.is_valid();
  }

  virtual bool flush() override;

  virtual bool resize(const size_type _size) override;

  virtual void close() override {
    if (is_open()) {
      flush();

      if (m_mapped_memory) {
        const int unmap_result = ::munmap(m_mapped_memory, m_size);
        (void)unmap_result;
        WN_DEBUG_ASSERT(
            unmap_result == 0, "failed to unmap memory mapped file");
      }

      m_file_descriptor.dispose();

      m_mapped_memory = NULL;
      m_size = 0;
    }
  }

protected:
  file_descriptor m_file_descriptor;
  void* m_mapped_memory;
  size_t m_size;
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_POSIX_FILE_POSIX_H__
