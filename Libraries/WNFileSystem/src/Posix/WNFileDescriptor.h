// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_FILE_DESCRIPTOR_H__
#define __WN_FILE_SYSTEM_POSIX_FILE_DESCRIPTOR_H__

#include "WNCore/inc/WNAssert.h"

#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {

class file_descriptor final {
public:
  WN_FORCE_INLINE file_descriptor() : m_file_desciptor(-1) {}

  WN_FORCE_INLINE file_descriptor(const int _file_desciptor)
    : m_file_desciptor(_file_desciptor) {}

  WN_FORCE_INLINE file_descriptor(file_descriptor&& _other)
    : m_file_desciptor(_other.m_file_desciptor) {
    _other.m_file_desciptor = -1;
  }

  WN_FORCE_INLINE ~file_descriptor() {
    dispose();
  }

  WN_FORCE_INLINE file_descriptor& operator=(file_descriptor&& _other) {
    m_file_desciptor = _other.m_file_desciptor;
    _other.m_file_desciptor = -1;

    return *this;
  }

  WN_FORCE_INLINE int value() const {
    return m_file_desciptor;
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_file_desciptor != -1);
  }

  WN_FORCE_INLINE void dispose() {
    if (m_file_desciptor != -1) {
      const int close_result = ::close(m_file_desciptor);

      m_file_desciptor = -1;

      WN_DEBUG_ASSERT_DESC(
          close_result == 0, "failed to close file descriptor");

#ifndef _WN_DEBUG
      WN_UNUSED_ARGUMENT(close_result);
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