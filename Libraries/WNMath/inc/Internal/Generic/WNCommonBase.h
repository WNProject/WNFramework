// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__
#define __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__

#include "WNMath/inc/Internal/WNStorageBase.h"
#include "core/inc/assert.h"
#include "core/inc/types.h"
#include "core/inc/utilities.h"

namespace wn {
namespace math {
namespace internal {

template <typename T, size_t Dimension>
class common_base_generic : public storage_base<T, Dimension> {
private:
  using base = storage_base<T, Dimension>;

public:
  inline common_base_generic() : base() {}

  inline explicit common_base_generic(const T& _value) : base() {
    for (size_t i = 0; i < Dimension; ++i) {
      at(i) = _value;
    }
  }

  inline explicit common_base_generic(const T (&_values)[Dimension]) : base() {
    for (size_t i = 0; i < Dimension; ++i) {
      at(i) = _values[i];
    }
  }

  template <typename Itr>
  inline common_base_generic(Itr _begin, Itr _end) : base() {
    size_t count = 0;

    for (; _begin < _end && count < Dimension; ++_begin, ++count) {
      at(count) = *_begin;
    }

    for (; count < Dimension; ++count) {
      at(count) = T(0);
    }

    WN_DEBUG_ASSERT(_begin == _end, "not all values consumed");
  }

  inline T& operator[](const size_t _index) {
    return at(_index);
  }

  inline const T& operator[](const size_t _index) const {
    return at(_index);
  }

  inline T& at(const size_t _index) {
    return base::m_values[_index];
  }

  inline const T& at(const size_t _index) const {
    return base::m_values[_index];
  }

  inline void assign(const T& _value) {
    common_base_generic(_value).swap(*this);
  }

  inline void assign(const T (&_values)[Dimension]) {
    common_base_generic(_values).swap(*this);
  }

  inline bool operator==(const common_base_generic& _other) const {
    for (size_t i = 0; i < Dimension; ++i) {
      if (at(i) != _other.at(i)) {
        return false;
      }
    }

    return true;
  }

  inline bool operator!=(const common_base_generic& _other) const {
    for (size_t i = 0; i < Dimension; ++i) {
      if (at(i) == _other.at(i)) {
        return false;
      }
    }

    return true;
  }

  inline void swap(common_base_generic& _other) {
    for (size_t i = 0; i < Dimension; ++i) {
      core::swap(at(i), _other.at(i));
    }
  }
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_COMMON_BASE_GENERIC_H__
