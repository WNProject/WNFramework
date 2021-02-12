// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_MATRIX_BASE_H__
#define __WN_MATH_INTERNAL_GENERIC_MATRIX_BASE_H__

#include "WNMath/inc/Internal/WNCommonBase.h"
#include "core/inc/type_traits.h"

namespace wn {
namespace math {
namespace internal {

template <template <typename, size_t, size_t, bool> class MatrixType,
    typename T, size_t Rows, size_t Columns, bool Precise>
class matrix_base_generic_common : public common_base<T, Rows * Columns> {
private:
  using base = common_base<T, Rows * Columns>;

  enum { dimension = Rows * Columns };

public:
  using base::at;

  inline matrix_base_generic_common() : base() {}

  inline matrix_base_generic_common(matrix_base_generic_common&& _matrix)
    : base() {
    for (size_t i = 0; i < dimension; ++i) {
      at(i) = core::move(_matrix[i]);
    }
  }

  inline matrix_base_generic_common(const matrix_base_generic_common& _matrix)
    : base(_matrix.m_values) {}

  inline explicit matrix_base_generic_common(const T& _value) : base(_value) {}

  inline explicit matrix_base_generic_common(const T (&_values)[Rows * Columns])
    : base(_values) {}

  inline explicit matrix_base_generic_common(const T (&_values)[Columns][Rows])
    : base() {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Columns; ++j) {
        at(i, j) = _values[j][i];
      }
    }
  }

  template <typename Itr>
  inline matrix_base_generic_common(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline T& at(const size_t _row, const size_t _column) {
    return at(_column * Rows + _row);
  }

  inline const T& at(const size_t _row, const size_t _column) const {
    return at(_column * Rows + _row);
  }

  inline MatrixType<T, Columns, Rows, Precise> transposed() const {
    MatrixType<T, Columns, Rows, Precise> new_matrix;

    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Columns; ++j) {
        new_matrix.at(j, i) = at(i, j);
      }
    }

    return new_matrix;
  }

protected:
  template <size_t OtherColumns, bool OtherPrecise>
  inline MatrixType<T, Rows, OtherColumns, Precise> multiply(
      const MatrixType<T, Columns, OtherColumns, OtherPrecise>& _matrix) const {
    MatrixType<T, Rows, OtherColumns, Precise> new_matrix;

    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < OtherColumns; ++j) {
        new_matrix.at(i, j) = 0;

        for (size_t k = 0; k < Columns; ++k) {
          new_matrix.at(i, j) += at(i, k) * _matrix.at(k, j);
        }
      }
    }

    return new_matrix;
  }
};

template <template <typename, size_t, size_t, bool> class MatrixType,
    typename T, size_t Order, bool Precise>
class matrix_base_generic_square_only
  : public matrix_base_generic_common<MatrixType, T, Order, Order, Precise> {
private:
  using base = matrix_base_generic_common<MatrixType, T, Order, Order, Precise>;

public:
  inline matrix_base_generic_square_only() : base() {}

  inline matrix_base_generic_square_only(
      matrix_base_generic_square_only&& _matrix)
    : base(core::move(_matrix)) {}

  inline matrix_base_generic_square_only(
      const matrix_base_generic_square_only& _matrix)
    : base(_matrix) {}

  inline matrix_base_generic_square_only(const T& _value) : base(_value) {}

  inline explicit matrix_base_generic_square_only(
      const T (&_values)[Order * Order])
    : base(_values) {}

  inline explicit matrix_base_generic_square_only(
      const T (&_values)[Order][Order])
    : base(_values) {}

  template <typename Itr>
  inline matrix_base_generic_square_only(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline T trace() const {
    T sum(0);

    for (size_t i = 0; i < Order; ++i) {
      sum += base::at(i, i);
    }

    return sum;
  }

  inline void transpose() {
    for (size_t i = 0; i < Order; ++i) {
      for (size_t j = 0; j < i; ++j) {
        core::swap(base::at(i, j), base::at(j, i));
      }
    }
  }
};

template <template <typename, size_t, size_t, bool> class MatrixType,
    typename T, size_t Rows, size_t Columns, bool Precise>
using matrix_base_generic = core::conditional_t<Rows == Columns,
    matrix_base_generic_square_only<MatrixType, T, Rows, Precise>,
    matrix_base_generic_common<MatrixType, T, Rows, Columns, Precise>>;

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_GENERIC_MATRIX_BASE_H__
