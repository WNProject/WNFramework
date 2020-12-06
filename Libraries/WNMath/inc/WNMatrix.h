// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_MATRIX_H__
#define __WN_MATH_MATRIX_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"
#include "core/inc/type_traits.h"

namespace wn {
namespace math {

/*
 * Note that all matrices are stored as column-major by default. This means that
 * all column data is sotred in contiguous blocks of memory with 1 column after
 * the other
 *
 *  Mathmatical Representation
 * -----------------------------------------------------------------------------
 *
 *   | 1 | 4 | 7 | 10 |
 *   | 2 | 5 | 8 | 11 |
 *   | 3 | 6 | 9 | 12 |
 *
 *  Memory Layout
 * -----------------------------------------------------------------------------
 *
 *   | 1 2 3 | 4 5 6 | 7 8 9 | 10 11 12 |
 */

template <typename T, size_t Rows, size_t Columns, bool Precise = false>
class matrix final
  : public internal::matrix_base<matrix, T, Rows, Columns, Precise> {
  static_assert((Rows * Columns) > 0,
      "template integral parameters 'Rows' and 'Columns' must not be 0");
  static_assert(!core::is_const<T>::value,
      "template type parameter 'T' must not be const type");
  static_assert(!core::is_reference<T>::value,
      "template type parameter 'T' must not be a reference type");

private:
  using base =
      internal::matrix_base<wn::math::matrix, T, Rows, Columns, Precise>;

public:
  inline matrix() : base() {}

  inline matrix(matrix&& _matrix) : base(core::move(_matrix)) {}

  inline matrix(const matrix& _matrix) : base(_matrix) {}

  inline explicit matrix(const T& _value) : base(_value) {}

  inline explicit matrix(const T (&_values)[Rows * Columns]) : base(_values) {}

  inline explicit matrix(const T (&_values)[Columns][Rows]) : base(_values) {}

  template <typename Itr>
  inline matrix(Itr _begin, Itr _end) : base(_begin, _end) {}

  inline matrix(std::initializer_list<T> _initializer_list)
    : matrix(_initializer_list.begin(), _initializer_list.end()) {}

  inline matrix& operator=(matrix&& _matrix) {
    matrix(core::move(_matrix)).swap(*this);

    return *this;
  }

  inline matrix& operator=(const matrix& _matrix) {
    matrix(_matrix).swap(*this);

    return *this;
  }

  inline matrix& operator=(const T& _value) {
    matrix(_value).swap(*this);

    return *this;
  }

  inline matrix& operator=(const T (&_values)[Rows * Columns]) {
    matrix(_values).swap(*this);

    return *this;
  }

  inline matrix& operator=(const T (&_values)[Columns][Rows]) {
    matrix(_values).swap(*this);

    return *this;
  }

  inline matrix& operator=(std::initializer_list<T> _initializer_list) {
    matrix(_initializer_list).swap(*this);

    return *this;
  }

  template <size_t OtherColumns, bool OtherPrecise>
  inline matrix<T, Rows, OtherColumns, Precise> operator*(
      const matrix<T, Columns, OtherColumns, OtherPrecise>& _matrix) const {
    return base::multiply(_matrix);
  }
};

template <typename T, size_t Order, bool Precise>
inline matrix<T, Order, Order, Precise>& operator*=(
    matrix<T, Order, Order, Precise>& _lhs,
    const matrix<T, Order, Order, Precise>& _rhs) {
  _lhs = _lhs * _rhs;

  return _lhs;
}

template <typename T, size_t Rows, size_t Columns>
using matrix_precise = matrix<T, Rows, Columns, true>;

// short forms

template <typename T>
using matrix22 = matrix<T, 2, 2>;

template <typename T>
using matrix23 = matrix<T, 2, 3>;

template <typename T>
using matrix33 = matrix<T, 3, 3>;

template <typename T>
using matrix34 = matrix<T, 3, 4>;

template <typename T>
using matrix44 = matrix<T, 4, 4>;

template <typename T>
using matrix22_precise = matrix_precise<T, 2, 2>;

template <typename T>
using matrix23_precise = matrix_precise<T, 2, 3>;

template <typename T>
using matrix33_precise = matrix_precise<T, 3, 3>;

template <typename T>
using matrix34_precise = matrix_precise<T, 3, 4>;

template <typename T>
using matrix44_precise = matrix_precise<T, 4, 4>;

// short, short forms

template <typename T>
using mat22 = matrix22<T>;

template <typename T>
using mat23 = matrix23<T>;

template <typename T>
using mat33 = matrix33<T>;

template <typename T>
using mat34 = matrix34<T>;

template <typename T>
using mat44 = matrix44<T>;

template <typename T>
using mat22_p = matrix22_precise<T>;

template <typename T>
using mat23_p = matrix23_precise<T>;

template <typename T>
using mat33_p = matrix33_precise<T>;

template <typename T>
using mat34_p = matrix34_precise<T>;

template <typename T>
using mat44_p = matrix44_precise<T>;

// short, short, short forms

using mat22i8 = mat22<int8_t>;
using mat22i16 = mat22<int16_t>;
using mat22i32 = mat22<int32_t>;
using mat22i64 = mat22<int64_t>;
using mat22u8 = mat22<uint8_t>;
using mat22u16 = mat22<uint16_t>;
using mat22u32 = mat22<uint32_t>;
using mat22u64 = mat22<uint64_t>;
using mat22f = mat22<float>;
using mat22d = mat22<double>;

using mat23i8 = mat23<int8_t>;
using mat23i16 = mat23<int16_t>;
using mat23i32 = mat23<int32_t>;
using mat23i64 = mat23<int64_t>;
using mat23u8 = mat23<uint8_t>;
using mat23u16 = mat23<uint16_t>;
using mat23u32 = mat23<uint32_t>;
using mat23u64 = mat23<uint64_t>;
using mat23f = mat23<float>;
using mat23d = mat23<double>;

using mat33i8 = mat33<int8_t>;
using mat33i16 = mat33<int16_t>;
using mat33i32 = mat33<int32_t>;
using mat33i64 = mat33<int64_t>;
using mat33u8 = mat33<uint8_t>;
using mat33u16 = mat33<uint16_t>;
using mat33u32 = mat33<uint32_t>;
using mat33u64 = mat33<uint64_t>;
using mat33f = mat33<float>;
using mat33d = mat33<double>;

using mat34i8 = mat34<int8_t>;
using mat34i16 = mat34<int16_t>;
using mat34i32 = mat34<int32_t>;
using mat34i64 = mat34<int64_t>;
using mat34u8 = mat34<uint8_t>;
using mat34u16 = mat34<uint16_t>;
using mat34u32 = mat34<uint32_t>;
using mat34u64 = mat34<uint64_t>;
using mat34f = mat34<float>;
using mat34d = mat34<double>;

using mat44i8 = mat44<int8_t>;
using mat44i16 = mat44<int16_t>;
using mat44i32 = mat44<int32_t>;
using mat44i64 = mat44<int64_t>;
using mat44u8 = mat44<uint8_t>;
using mat44u16 = mat44<uint16_t>;
using mat44u32 = mat44<uint32_t>;
using mat44u64 = mat44<uint64_t>;
using mat44f = mat44<float>;
using mat44d = mat44<double>;

using mat22f_p = mat22_p<float>;
using mat22d_p = mat22_p<double>;

using mat23f_p = mat23_p<float>;
using mat23d_p = mat23_p<double>;

using mat33f_p = mat33_p<float>;
using mat33d_p = mat33_p<double>;

using mat34f_p = mat34_p<float>;
using mat34d_p = mat34_p<double>;

using mat44f_p = mat44_p<float>;
using mat44d_p = mat44_p<double>;

}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_MATRIX_H__
