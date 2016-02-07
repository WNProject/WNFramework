// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_PLANE_INL__
#define __WN_MATH_PLANE_INL__

#ifndef __WN_MATH_PLANE_H__
#error                                                                         \
    "WNPlane.inl should never be included directly. Please include WNPlane.h instead"
#endif

#include "WNMath/inc/WNVector3.h"

namespace wn {
template <typename type>
WNPlane<type>::WNPlane() {
  Zero();
}

template <typename type>
WNPlane<type>::WNPlane(const type* _numbers) {
  Set(_numbers);
}

template <typename type>
WNPlane<type>::WNPlane(const WNVector3<type>& _normal, type _distance) {
  Set(_normal, _distance);
}

template <typename type>
WNPlane<type>::WNPlane(type _a, type _b, type _c, type _d) {
  Set(_a, _b, _c, _d);
}

template <typename type>
WNPlane<type>::operator type*() {
  return (&this->mA);
}

template <typename type>
WNPlane<type>::operator const type*() const {
  return (&this->mA);
}

template <typename type>
WNPlane<type>& WNPlane<type>::operator+=(const WNPlane<type>& _plane) {
  this->mA += _plane.mA;
  this->mB += _plane.mB;
  this->mC += _plane.mC;
  this->mD += _plane.mD;

  return (*this);
}

template <typename type>
WNPlane<type>& WNPlane<type>::operator-=(const WNPlane<type>& _plane) {
  this->mA -= _plane.mA;
  this->mB -= _plane.mB;
  this->mC -= _plane.mC;
  this->mD -= _plane.mD;

  return (*this);
}

template <typename type>
WNPlane<type> WNPlane<type>::operator+() const {
  return (*this);
}

template <typename type>
WNPlane<type> WNPlane<type>::operator-() const {
  return (WNPlane<type>(-this->mA, -this->mB, -this->mC, -this->mD));
}

template <typename type>
WNPlane<type> WNPlane<type>::operator+(const WNPlane<type>& _plane) const {
  WNPlane<type> plane = *this;

  plane += _plane;

  return (plane);
}

template <typename type>
WNPlane<type> WNPlane<type>::operator-(const WNPlane<type>& _plane) const {
  WNPlane<type> plane = *this;

  plane -= _plane;

  return (plane);
}

template <typename type>
bool WNPlane<type>::operator==(const WNPlane<type>& _plane) const {
  return (this->mA == _plane.mA && this->mB == _plane.mB &&
          this->mC == _plane.mC && this->mD == _plane.mD);
}

template <typename type>
bool WNPlane<type>::operator!=(const WNPlane<type>& _plane) const {
  return (this->mA != _plane.mA || this->mB != _plane.mB ||
          this->mC != _plane.mC || this->mD != _plane.mD);
}

template <typename type>
void WNPlane<type>::Zero() {
  this->mA = static_cast<type>(0);
  this->mB = static_cast<type>(0);
  this->mC = static_cast<type>(0);
  this->mD = static_cast<type>(0);
}

template <typename type>
void WNPlane<type>::Set(const type* _numbers) {
  this->mA = _numbers[0];
  this->mB = _numbers[1];
  this->mC = _numbers[2];
  this->mD = _numbers[3];
}

template <typename type>
void WNPlane<type>::Set(type _a, type _b, type _c, type _d) {
  this->mA = _a;
  this->mB = _b;
  this->mC = _c;
  this->mD = _d;
}

template <typename type>
void WNPlane<type>::Define(const WNVector3<type>& _vector1,
    const WNVector3<type>& _vector2, const WNVector3<type>& _vector3) {
  const WNVector3<type> normal = _vector1.Cross(_vector2);

  this->mA = normal.mX;
  this->mB = normal.mY;
  this->mC = normal.mZ;

  this->mD = -(Normal().Dot(_vector3));
}

template <typename type>
void WNPlane<type>::Normalize() {
  const WNVector3<type> vector = Normal().GetNormalized();

  this->mA = vector.mX;
  this->mB = vector.mY;
  this->mC = vector.mZ;
}

template <typename type>
void WNPlane<type>::Translate(const WNVector3<type>& _translation) {
  this->mD -= _translation.Dot(Normal());
}

template <typename type>
WNVector3<type> WNPlane<type>::Normal() const {
  return (WNVector3<type>(this->mA, this->mB, this->mC));
}

template <typename type>
type WNPlane<type>::Distance() const {
  return (-this->mD);
}

template <typename type>
WNPlane<type> WNPlane<type>::GetNormalized() const {
  WNPlane<type> plane = *this;

  return (plane.Normalize(), plane);
}

template <typename type>
WNPlane<type> WNPlane<type>::GetTranslated(
    const WNVector3<type>& _translation) const {
  WNPlane<type> plane = *this;

  return (plane.Translate(_translation), plane);
}

#ifndef __WN_MATH_HAS_PLANE_CUSTOM_PREFETCH
template <typename type>
void WNPlane<type>::Prefetch() const {
  // Default
}
#endif

template <typename type>
template <typename NewType>
WNPlane<NewType> WNPlane<type>::ConvertTo() const {
  return (WNPlane<NewType>(static_cast<NewType>(this->mA),
      static_cast<NewType>(this->mB), static_cast<NewType>(this->mC),
      static_cast<NewType>(this->mD)));
}
}

#ifdef __WN_MATH_EXTENSIONS_ENABLED
#ifdef _WN_X86
//#include "WNMath/inc/Internal/Extensions/x86/WNPlane.inl"
#endif
#endif

#endif  // __WN_MATH_PLANE_INL__
