// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_PAIR_H__
#define __WN_CORE_PAIR_H__

#include "WNCore/inc/WNUtility.h"

namespace wn {
namespace core {

template <typename T1, typename T2 = T1>
class pair final {
public:
  // types

  using first_type = T1;
  using second_type = T2;

  // constructors

  WN_FORCE_INLINE pair() : first(), second() {}

  WN_FORCE_INLINE pair(pair&& _other)
    : pair(move(_other.first), move(_other.second)) {}

  WN_FORCE_INLINE pair(const pair& _other)
    : pair(_other.first, _other.second) {}

  WN_FORCE_INLINE pair(const T1& _first, const T2& _second)
    : first(_first), second(_second) {}

  template <typename U, typename = enable_if_t<conjunction<is_same<T1, T2>,
                            is_constructible<T1, const U&>>::value>>
  WN_FORCE_INLINE pair(const U& _value) : pair(_value, _value) {}

  template <typename U1, typename U2,
      typename = enable_if_t<conjunction<is_constructible<T1, U1&&>,
          is_constructible<T2, U2&&>>::value>>
  WN_FORCE_INLINE pair(U1&& _first, U2&& _second)
    : first(forward<U1>(_first)), second(forward<U2>(_second)) {}

  template <typename U1, typename U2,
      typename = enable_if_t<conjunction<is_constructible<T1, U1&&>,
          is_constructible<T2, U2&&>>::value>>
  WN_FORCE_INLINE pair(pair<U1, U2>&& _other)
    : pair(forward<U1>(_other.first), forward<U2>(_other.second)) {}

  template <typename U1, typename U2,
      typename = enable_if_t<conjunction<is_constructible<T1, const U1&>,
          is_constructible<T2, const U2&>>::value>>
  WN_FORCE_INLINE pair(const pair<U1, U2>& _other)
    : pair(_other.first, _other.second) {}

  // assignments

  WN_FORCE_INLINE pair& operator=(pair&& _other) {
    pair(move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE pair& operator=(const pair& _other) {
    pair(_other).swap(*this);

    return *this;
  }

  template <typename U, typename = enable_if_t<conjunction<is_same<T1, T2>,
                            is_constructible<T1, const U&>>::value>>
  WN_FORCE_INLINE pair& operator=(const U& _value) {
    pair(_value).swap(*this);

    return *this;
  }

  template <typename U1, typename U2,
      typename = enable_if_t<conjunction<is_constructible<T1, U1&&>,
          is_constructible<T2, U2&&>>::value>>
  WN_FORCE_INLINE pair& operator=(pair<U1, U2>&& _other) {
    pair(move(_other)).swap(*this);

    return *this;
  }

  template <typename U1, typename U2,
      typename = enable_if_t<conjunction<is_constructible<T1, const U1&>,
          is_constructible<T2, const U2&>>::value>>
  WN_FORCE_INLINE pair& operator=(const pair<U1, U2>& _other) {
    pair(_other).swap(*this);

    return *this;
  }

  // modifiers

  WN_FORCE_INLINE void swap(pair& _other) {
    if ((&_other) != this) {
      core::swap(first, _other.first);
      core::swap(second, _other.second);
    }
  }

  // objects

  first_type first;
  second_type second;
};

namespace internal {

template <typename T>
struct arg_decay {
  using type = decay_t<T>;
};

template <typename T>
struct arg_decay<std::reference_wrapper<T>> {
  using type = T&;
};

template <typename T>
using arg_decay_t = typename arg_decay<T>::type;

}  // namespace internal

template <typename T1, typename T2>
WN_FORCE_INLINE pair<internal::arg_decay_t<T1>, internal::arg_decay_t<T2>>
make_pair(T1&& _first, T2&& _second) {
  return pair<internal::arg_decay_t<T1>, internal::arg_decay_t<T2>>(
      forward<T1>(_first), forward<T2>(_second));
}

template <typename T>
WN_FORCE_INLINE pair<internal::arg_decay_t<T>> make_pair(const T& _value) {
  return pair<internal::arg_decay_t<T>>(_value);
}

template <typename T1, typename T2>
WN_FORCE_INLINE pair<T1&&, T2&&> forward_as_pair(T1&& _first, T2&& _second) {
  return pair<T1&&, T2&&>(forward<T1>(_first), forward<T2>(_second));
}

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_PAIR_H__