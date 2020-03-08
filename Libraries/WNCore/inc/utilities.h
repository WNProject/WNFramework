// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_UTILITIES_H__
#define __WN_CORE_UTILITIES_H__

#include <utility>

#include "WNCore/inc/type_traits.h"
#include "WNCore/inc/types.h"

namespace wn {
namespace core {

using std::declval;
using std::forward;
using std::move;
using std::swap;

class non_copyable {
protected:
  non_copyable() = default;
  non_copyable(const non_copyable&) = delete;

  non_copyable& operator=(const non_copyable&) = delete;
};

class non_constructable {
protected:
  non_constructable() = delete;
  non_constructable(non_constructable&&) = delete;
  non_constructable(const non_constructable&) = delete;

  non_constructable& operator=(non_constructable&&) = delete;
  non_constructable& operator=(const non_constructable&) = delete;
};

template <typename T>
struct type_id final : non_constructable {
  static inline uintptr_t value() {
    static void* dummy = nullptr;

    return reinterpret_cast<uintptr_t>(&dummy);
  }
};

template <typename T>
inline decay_t<T> decay_copy(T&& value) {
  return forward<T>(value);
}

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_UTILITIES_H__
