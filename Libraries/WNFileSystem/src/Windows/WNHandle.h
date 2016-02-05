// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_HANDLE_H__
#define __WN_FILE_SYSTEM_WINDOWS_HANDLE_H__

#include "WNCore/inc/WNAssert.h"

#include <Windows.h>

namespace wn {
namespace file_system {
namespace internal {

class handle final : core::non_copyable {
public:
  WN_FORCE_INLINE handle() : m_handle(NULL) {}

  WN_FORCE_INLINE handle(const nullptr_t) : handle() {}

  WN_FORCE_INLINE handle(HANDLE _handle) : m_handle(_handle) {}

  WN_FORCE_INLINE handle(handle&& _handle)
    : m_handle(std::move(_handle.m_handle)) {
    _handle.m_handle = NULL;
  }

  WN_FORCE_INLINE ~handle() {
    dispose();
  }

  WN_FORCE_INLINE handle& operator=(handle&& _handle) {
    m_handle = _handle.m_handle;
    _handle.m_handle = NULL;

    return *this;
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_handle != NULL && m_handle != INVALID_HANDLE_VALUE);
  }

  WN_FORCE_INLINE HANDLE value() const {
    return m_handle;
  }

  WN_FORCE_INLINE void dispose() {
    if (m_handle != NULL && m_handle != INVALID_HANDLE_VALUE) {
      const BOOL close_result = ::CloseHandle(m_handle);

      m_handle = NULL;

      WN_DEBUG_ASSERT_DESC(close_result == TRUE, "failed to close handle");

#ifndef _WN_DEBUG
      WN_UNUSED_ARGUMENT(close_result);
#endif
    }
  }

private:
  HANDLE m_handle;
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_HANDLE_H__
