// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElementArray.h"

namespace wn {
namespace internal {
template <typename MatrixType, typename type, uint32_t Order>
class __WNMatrixBaseCommon {
public:
  type& operator[](const uint32_t _index);
  const type& operator[](const uint32_t _index) const;

  type& operator()(const uint32_t _index);
  const type& operator()(const uint32_t _index) const;

  type& operator()(const uint32_t _index1, const uint32_t _index2);
  const type& operator()(const uint32_t _index1, const uint32_t _index2) const;

  MatrixType operator+() const;
  MatrixType operator-() const;

  MatrixType& operator+=(const MatrixType& _matrix);
  MatrixType& operator-=(const MatrixType& _matrix);
  MatrixType& operator*=(const MatrixType& _matrix);
  MatrixType& operator+=(const type& _number);
  MatrixType& operator-=(const type& _number);
  MatrixType& operator*=(const type& _number);
  MatrixType& operator/=(const type& _number);

  MatrixType operator+(const MatrixType& _matrix) const;
  MatrixType operator-(const MatrixType& _matrix) const;
  MatrixType operator*(const MatrixType& _matrix) const;
  MatrixType operator+(const type& _number) const;
  MatrixType operator-(const type& _number) const;
  MatrixType operator*(const type& _number) const;
  MatrixType operator/(const type& _number) const;

  bool operator==(const MatrixType& _matrix) const;
  bool operator!=(const MatrixType& _matrix) const;

  void Zero();
  void Identity();
  void Transpose();

  type Trace() const;
  type Determinant() const;  // need to fix

  bool IsIdentity() const;
  bool IsSymmetric() const;
  bool IsDiagonal() const;

  void Set(const MatrixType& _matrix);
  void Set(const type& _number);
  void Set(const type* _numbers);

  MatrixType GetTransposed() const;

public:
  static WN_FORCE_INLINE MatrixType CreateZero();
  static WN_FORCE_INLINE MatrixType CreateIdentity();

protected:
  enum { dimension = (Order * Order) };

protected:
  WN_FORCE_INLINE __WNMatrixBaseCommon() {}

protected:
  element_array<type, dimension> mElements;
};

template <typename MatrixType, typename type, uint32_t Order,
    typename = std::enable_if<true>::type>
class __WNMatrixBase : public __WNMatrixBaseCommon<MatrixType, type, Order> {
protected:
  WN_FORCE_INLINE __WNMatrixBase() {}
};

template <typename MatrixType, typename type, uint32_t Order>
class __WNMatrixBase<MatrixType, type, Order,
    typename std::enable_if<wn::is_real<type>::value>::type>
    : public __WNMatrixBaseCommon<MatrixType, type, Order> {
public:
  WN_FORCE_INLINE bool Invert() {}

  MatrixType GetInverted() const;
  WN_FORCE_INLINE MatrixType GetAdjugate() const {}

protected:
  WN_FORCE_INLINE __WNMatrixBase() {}
};
}
}

#include "WNMath/inc/Internal/WNMatrixBase.inl"

#endif  // __WN_MATH_INTERNAL_MATRIX_BASE_H__