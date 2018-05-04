// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_UTILITIES_WINDOWS_INTERNAL_HANDLE_H__
#define __WN_UTILITIES_WINDOWS_INTERNAL_HANDLE_H__

#include "WNCore/inc/assert.h"
#include "WNCore/inc/utilities.h"

namespace wn {
namespace utilities {
namespace internal {

class handle final {
public:
  handle() : m_handle(NULL) {}

  handle(const nullptr_t) : handle() {}

  handle(const HANDLE _handle) : m_handle(_handle) {}

  handle(handle&& _other) : m_handle(core::move(_other.m_handle)) {
    _other.m_handle = NULL;
  }

  handle(const handle& _other) : handle() {
    const HANDLE process_handle = ::GetCurrentProcess();
    const BOOL duplicate_result =
        ::DuplicateHandle(process_handle, _other.m_handle, process_handle,
            &m_handle, 0, FALSE, DUPLICATE_SAME_ACCESS);

    WN_DEBUG_ASSERT(
        duplicate_result == TRUE, "failed to duplicate handle");

    (void)duplicate_result;
  }

  ~handle() {
    dispose();
  }

  handle& operator=(const nullptr_t) {
    handle(nullptr).swap(*this);

    return *this;
  }

  handle& operator=(const HANDLE _handle) {
    handle(_handle).swap(*this);

    return *this;
  }

  handle& operator=(handle&& _other) {
    handle(core::move(_other)).swap(*this);

    return *this;
  }

  handle& operator=(const handle& _other) {
    handle(_other).swap(*this);

    return *this;
  }

  bool is_null() const {
    return (m_handle == NULL);
  }

  bool is_valid() const {
    return (!is_null() && m_handle != INVALID_HANDLE_VALUE);
  }

  HANDLE value() const {
    return m_handle;
  }

  void dispose() {
    if (is_valid()) {
      const BOOL close_result = ::CloseHandle(m_handle);

      m_handle = NULL;

      WN_DEBUG_ASSERT(close_result == TRUE, "failed to close handle");

      (void)close_result;
    }
  }

  void swap(handle& _other) {
    core::swap(m_handle, _other.m_handle);
  }

private:
  HANDLE m_handle;
};

inline bool operator==(const handle& _handle, const nullptr_t) {
  return _handle.is_null();
}

inline bool operator==(const nullptr_t, const handle& _handle) {
  return (_handle == nullptr);
}

inline bool operator!=(const handle& _handle, const nullptr_t) {
  return !(_handle == nullptr);
}

inline bool operator!=(const nullptr_t, const handle& _handle) {
  return (_handle != nullptr);
}

}  // namespace internal
}  // namespace utilities
}  // namespace wn

#endif  // __WN_UTILITIES_WINDOWS_INTERNAL_HANDLE_H__
