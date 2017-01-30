// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__
#define __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
namespace math {
namespace internal {

template <typename T, const size_t Dimension>
class element_array_generic : core::non_copyable {
  static_assert(Dimension > 0, "must have a dimension of at least 1");

public:
  enum { dimension = Dimension };

  element_array_generic() = default;

  WN_FORCE_INLINE T& operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const T& operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE T& at(const size_t _index) {
    return m_values[_index];
  }

  WN_FORCE_INLINE const T& at(const size_t _index) const {
    return m_values[_index];
  }

  WN_FORCE_INLINE T* data() {
    return m_values;
  }

  WN_FORCE_INLINE const T* data() const {
    return m_values;
  }

private:
  T m_values[Dimension];
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__