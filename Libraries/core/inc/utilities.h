// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_UTILITIES_H__
#define __WN_CORE_UTILITIES_H__

#include "core/inc/type_traits.h"
#include "core/inc/types.h"

#include <utility>

namespace wn {
namespace core {

// standard ////////////////////////////////////////////////////////////////////

// general

using std::index_sequence;
using std::integer_sequence;
using std::make_index_sequence;
using std::make_integer_sequence;

// swap and type operations

using std::as_const;
using std::declval;
using std::exchange;
using std::forward;
using std::move;
using std::swap;

// custom //////////////////////////////////////////////////////////////////////

// general

template <bool... Values>
using bool_sequence = integer_sequence<bool, Values...>;

// swap and type operations

template <typename T>
inline decay_t<T> decay_copy(T&& value) {
  return forward<T>(value);
}

// object bases

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

// type information

template <typename T>
struct type_id final : non_constructable {
  static inline uintptr_t value() {
    static void* dummy = nullptr;

    return reinterpret_cast<uintptr_t>(&dummy);
  }
};

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_UTILITIES_H__
