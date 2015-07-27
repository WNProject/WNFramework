// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_UTILITY_H__
#define __WN_CORE_UTILITY_H__

#include "WNCore/inc/WNTypeTraits.h"

#include <utility>

namespace wn {
namespace core {

template <typename T>
struct dependent_true : std::true_type {};

template <typename T>
struct dependent_false : std::false_type {};

class non_constructable {
protected:
  non_constructable() = delete;
};

class non_copyable {
protected:
  non_copyable() = default;
  non_copyable(const non_copyable &) = delete;

  non_copyable& operator = (const non_copyable&) = delete;
};

class non_constructable_non_copyable {
protected:
  non_constructable_non_copyable() = delete;
  non_constructable_non_copyable(const non_constructable_non_copyable&) =
    delete;

  non_constructable_non_copyable&
  operator = (const non_constructable_non_copyable&) = delete;
};

template <typename T>
WN_FORCE_INLINE decay_t<T> decay_copy(T&& value) {
  return(std::forward<T>(value));
}

} // namespace core
} // namespace wn

#endif // __WN_CORE_UTILITY_H__