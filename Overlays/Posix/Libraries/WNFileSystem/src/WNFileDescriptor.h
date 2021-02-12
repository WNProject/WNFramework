// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_FILE_DESCRIPTOR_H__
#define __WN_FILE_SYSTEM_POSIX_FILE_DESCRIPTOR_H__

#include "core/inc/assert.h"

#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {

class file_descriptor final {
public:
  inline file_descriptor() : m_file_desciptor(-1) {}

  inline file_descriptor(const int _file_desciptor)
    : m_file_desciptor(_file_desciptor) {}

  inline file_descriptor(file_descriptor&& _other)
    : m_file_desciptor(_other.m_file_desciptor) {
    _other.m_file_desciptor = -1;
  }

  inline ~file_descriptor() {
    dispose();
  }

  inline file_descriptor& operator=(file_descriptor&& _other) {
    m_file_desciptor = _other.m_file_desciptor;
    _other.m_file_desciptor = -1;

    return *this;
  }

  inline int value() const {
    return m_file_desciptor;
  }

  inline bool is_valid() const {
    return (m_file_desciptor != -1);
  }

  inline void dispose() {
    if (m_file_desciptor != -1) {
      const int close_result = ::close(m_file_desciptor);

      m_file_desciptor = -1;

      WN_DEBUG_ASSERT(close_result == 0, "failed to close file descriptor");

#ifndef _WN_DEBUG
      (void)close_result;
#endif
    }
  }

private:
  int m_file_desciptor;
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_POSIX_FILE_DESCRIPTOR_H__
