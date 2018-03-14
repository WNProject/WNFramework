// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_ASSERT_H__
#define __WN_CORE_ASSERT_H__

#include "WNCore/inc/WNBase.h"

#include <cstdint>
#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

#define NDEBUG

namespace wn {
namespace core {
namespace internal {

inline void assert_helper(const bool _expression, const char* _file,
    const uint32_t _line, const char* _message) {
  if (!_expression) {
    std::cerr << "assertion failed!" << std::endl
              << std::endl
              << "file: " << _file << std::endl
              << "line: " << _line << std::endl
              << "message: " << _message << std::endl;

    WN_DEBUG_BREAK();

    assert(false);
  }
}

}  // namespace internal
}  // namespace core
}  // namespace wn

#ifndef _WN_RETAIL
#define WN_RELEASE_ASSERT(expression, description)                             \
  wn::core::internal::assert_helper(                                           \
      !!(expression), __FILE__, __LINE__, description)
#else
#define WN_RELEASE_ASSERT(expression, description)
#endif

#ifdef _WN_DEBUG
#define WN_DEBUG_ASSERT(expression, description)                               \
  wn::core::internal::assert_helper(                                           \
      !!(expression), __FILE__, __LINE__, description)
#else
#define WN_DEBUG_ASSERT(expression, description)
#endif

#endif  // __WN_CORE_ASSERT_H__
