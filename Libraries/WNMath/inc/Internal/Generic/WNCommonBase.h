// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__
#define __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMath/inc/Internal/WNStorageBase.h"

namespace wn {
namespace math {
namespace internal {

template <typename T, size_t Dimension>
class common_base_generic : public storage_base<T, Dimension> {
private:
  using base = storage_base<T, Dimension>;

public:
  WN_FORCE_INLINE common_base_generic() : base() {}

  WN_FORCE_INLINE explicit common_base_generic(const T& _value) : base() {
    for (size_t i = 0; i < Dimension; ++i) {
      at(i) = _value;
    }
  }

  WN_FORCE_INLINE explicit common_base_generic(const T (&_values)[Dimension])
    : base() {
    for (size_t i = 0; i < Dimension; ++i) {
      at(i) = _values[i];
    }
  }

  template <typename Itr>
  WN_FORCE_INLINE common_base_generic(Itr _begin, Itr _end) : base() {
    size_t count = 0;

    for (; _begin < _end && count < Dimension; ++_begin, ++count) {
      at(count) = *_begin;
    }

    for (; count < Dimension; ++count) {
      at(count) = T(0);
    }

    WN_DEBUG_ASSERT(_begin == _end, "not all values consumed");
  }

  WN_FORCE_INLINE T& operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const T& operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE T& at(const size_t _index) {
    return base::m_values[_index];
  }

  WN_FORCE_INLINE const T& at(const size_t _index) const {
    return base::m_values[_index];
  }

  WN_FORCE_INLINE void assign(const T& _value) {
    common_base_generic(_value).swap(*this);
  }

  WN_FORCE_INLINE void assign(const T (&_values)[Dimension]) {
    common_base_generic(_values).swap(*this);
  }

  WN_FORCE_INLINE bool operator==(const common_base_generic& _other) const {
    for (size_t i = 0; i < Dimension; ++i) {
      if (at(i) != _other.at(i)) {
        return false;
      }
    }

    return true;
  }

  WN_FORCE_INLINE bool operator!=(const common_base_generic& _other) const {
    for (size_t i = 0; i < Dimension; ++i) {
      if (at(i) == _other.at(i)) {
        return false;
      }
    }

    return true;
  }

  WN_FORCE_INLINE void swap(common_base_generic& _other) {
    for (size_t i = 0; i < Dimension; ++i) {
      core::swap(at(i), _other.at(i));
    }
  }
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__
