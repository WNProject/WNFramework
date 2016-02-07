// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_QUATERNION_BASE_H__
#define __WN_MATH_INTERNAL_QUATERNION_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElementArray.h"

namespace wn {
namespace internal {
template <typename QuaternionType, typename type>
class __WNQuaternionBaseCommon {
public:
  type& operator[](const uint32_t _index);
  const type& operator[](const uint32_t _index) const;

  type& operator()(const uint32_t _index);
  const type& operator()(const uint32_t _index) const;

  QuaternionType operator+() const;
  QuaternionType operator-() const;

  QuaternionType& operator+=(const QuaternionType& _quaternion);
  QuaternionType& operator-=(const QuaternionType& _quaternion);
  QuaternionType& operator*=(const QuaternionType& _quaternion);
  QuaternionType& operator*=(const type& _number);
  QuaternionType& operator/=(const type& _number);

  QuaternionType operator+(const QuaternionType& _quaternion) const;
  QuaternionType operator-(const QuaternionType& _quaternion) const;
  QuaternionType operator*(const QuaternionType& _quaternion) const;
  QuaternionType operator*(const type& _number) const;
  QuaternionType operator/(const type& _number) const;

  bool operator==(const QuaternionType& _quaternion) const;
  bool operator!=(const QuaternionType& _quaternion) const;

  void Zero();
  void Identity();

  type Length() const;
  type LengthSquared() const;

  type Dot(const QuaternionType& _quaternion) const;

  bool IsZero() const;
  bool IsIdentity() const;

  void Set(const QuaternionType& _quaternion);
  void Set(const type& _number);
  void Set(const type* _number);
  void Set(const type& _x, const type& _y, const type& _z, const type& _w);

public:
  static QuaternionType CreateZero();
  static QuaternionType CreateIdentity();

protected:
  __WNQuaternionBaseCommon();

protected:
  element_array<type, 4> mElements;
};

template <typename QuaternionType, typename type,
    typename = std::enable_if<true>::type>
class __WNQuaternionBase
    : public __WNQuaternionBaseCommon<QuaternionType, type> {
protected:
  WN_FORCE_INLINE __WNQuaternionBase() {}
};

template <typename QuaternionType, typename type>
class __WNQuaternionBase<QuaternionType, type,
    typename std::enable_if<wn::is_real<type>::value>::type>
    : public __WNQuaternionBaseCommon<QuaternionType, type> {
public:
  void Normalize();
  void Invert();
  void Logarithm();
  void Exponent();
  void Rotate(const QuaternionType& _rotation);

  QuaternionType GetNormalized() const;
  QuaternionType GetInverted() const;
  QuaternionType GetConjugate() const;
  QuaternionType GetLogarithm() const;
  QuaternionType GetExponent() const;
  QuaternionType GetRotated(const QuaternionType& _rotation) const;

protected:
  WN_FORCE_INLINE __WNQuaternionBase() {}
};
}
}

#include "WNMath/inc/Internal/WNQuaternionBase.inl"

#endif  // __WN_MATH_INTERNAL_QUATERNION_BASE_H__