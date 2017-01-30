// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TYPES_H__
#define __WN_CORE_TYPES_H__

#include "WNCore/inc/WNBase.h"

#include <cfloat>
#include <cstddef>
#include <cstdint>

#ifdef _WN_64_BIT
typedef int64_t signed_t;
#else
typedef int32_t signed_t;
#endif

#ifndef _WN_HAS_CPP11_NULLPTR
namespace wn {
namespace core {
namespace internal {

const class {
public:
  template <typename T>
  WN_FORCE_INLINE operator T*() const {
    return 0;
  }

  template <typename T, typename U>
  WN_FORCE_INLINE operator U T::*() const {
    return 0;
  }

  void operator&() const = delete;
} null_pointer = {};

}  // namespace internal
}  // namespace core
}  // namespace wn

#define nullptr wn::core::internal::null_pointer
#endif

#ifndef _WN_HAS_CPP11_NULLPTR_T
typedef decltype(nullptr) nullptr_t;
#endif

#endif  // __WN_CORE_TYPES_H__
