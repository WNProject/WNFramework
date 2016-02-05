// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_UTILITY_H__
#define __WN_CORE_UTILITY_H__

#include "WNCore/inc/WNTypeTraits.h"

#include <utility>

namespace wn {
namespace core {

using std::move;
using std::forward;
using std::swap;

class non_constructable {
protected:
  non_constructable() = delete;
};

class non_copyable {
protected:
  non_copyable() = default;
  non_copyable(const non_copyable&) = delete;

  non_copyable& operator=(const non_copyable&) = delete;
};

class non_constructable_non_copyable {
protected:
  non_constructable_non_copyable() = delete;
  non_constructable_non_copyable(
      const non_constructable_non_copyable&) = delete;

  non_constructable_non_copyable& operator=(
      const non_constructable_non_copyable&) = delete;
};

template <typename T>
WN_FORCE_INLINE decay_t<T> decay_copy(T&& value) {
  return forward<T>(value);
}

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_UTILITY_H__