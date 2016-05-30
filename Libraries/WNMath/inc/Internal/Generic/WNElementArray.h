// Copyright (c) 2016, WNProject Authors. All rights reserved.
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

template <typename T, const size_t Columns, const size_t Rows = 1>
class element_array_generic : core::non_copyable {
  static_assert((Columns > 0) && (Rows > 0), "must have a count of at least 1");

public:
  enum { count = (Columns * Rows), columns = Columns, rows = Rows };

  element_array_generic() = default;

  WN_FORCE_INLINE T& operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const T& operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE T& at(const size_t _index) {
    const size_t column = _index % Columns;
    const size_t row = _index / Columns;

    return at(column, row);
  }

  WN_FORCE_INLINE const T& at(const size_t _index) const {
    const size_t column = _index % Columns;
    const size_t row = _index / Columns;

    return at(column, row);
  }

  WN_FORCE_INLINE T& at(const size_t _column, const size_t _row) {
    WN_DEBUG_ASSERT_DESC((_column < Columns) && (_row < Rows),
        "attempting to get element outside of bounds");

    return m_values[_column][_row];
  }

  WN_FORCE_INLINE const T& at(const size_t _column, const size_t _row) const {
    return m_values[_column][_row];
  }

  WN_FORCE_INLINE T* data() {
    return m_values;
  }

  WN_FORCE_INLINE const T* data() const {
    return m_values;
  }

private:
  T m_values[Rows][Columns];
};

template <typename T, const size_t Count>
class element_array_generic<T, Count, 1> : core::non_copyable {
  static_assert(Count > 0, "must have a count of at least 1");

public:
  enum { count = Count, columns = Count, rows = 1 };

  element_array_generic() = default;

  WN_FORCE_INLINE T& operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const T& operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE T& at(const size_t _index) {
    return at(_index, 0);
  }

  WN_FORCE_INLINE const T& at(const size_t _index) const {
    return at(_index, 0);
  }

  WN_FORCE_INLINE T& at(const size_t _column, const size_t _row) {
    WN_DEBUG_ASSERT_DESC((_column < Count) && (_row == 0),
        "attempting to get element outside of bounds");

#ifndef _WN_DEBUG
    WN_UNUSED_ARGUMENT(_row);
#endif

    return m_values[_column];
  }

  WN_FORCE_INLINE const T& at(const size_t _column, const size_t _row) const {
    WN_DEBUG_ASSERT_DESC((_column < Count) && (_row == 0),
        "attempting to get element outside of bounds");

#ifndef _WN_DEBUG
    WN_UNUSED_ARGUMENT(_row);
#endif

    return m_values[_column];
  }

  WN_FORCE_INLINE T* data() {
    return m_values;
  }

  WN_FORCE_INLINE const T* data() const {
    return m_values;
  }

private:
  T m_values[Count];
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__