// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_ASSERT_H__
#define __WN_CORE_ASSERT_H__

#include "WNCore/inc/WNTypes.h"

#include <iostream>
#ifdef NDEBUG
#undef NDEBUG
#include <cassert>
#define NDEBUG
#else
#include <cassert>
#endif

namespace wn {
namespace core {
namespace internal {

inline void assert_helper(const bool expression, const char* file,
    const uint32_t line, const char* message) {
  if (!expression) {
    std::cerr << "assertion failed!" << std::endl
              << std::endl
              << "file: " << file << std::endl
              << "line: " << line << std::endl
              << "message: " << message << std::endl;

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
