// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__

#ifndef __WN_MATH_MATRIX_4_H__
#error                                                                         \
    "Internal/WNMatrix4Base.inl should never be included directly. Please include WNMatrix4.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNQuaternion.h"
#include "WNMath/inc/WNVector.h"
#include "WNMath/inc/WNVector4.h"

namespace wn {
namespace internal {
template <typename type>
WN_FORCE_INLINE WNVector<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector<type>& _vector) const {
  WNVector<type> vector = _vector;

  return (this->TransformVectorInPlace(vector));
}

template <typename type>
WN_FORCE_INLINE WNVector<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector<type>& _vector, type& _wComponent) const {
  WNVector4<type> vector = _vector.ToVector4();

  this->TransformVectorInPlace(vector);

  _wComponent = vector.GetW();

  return (vector.ToVector3());
}

template <typename type>
WN_FORCE_INLINE WNVector4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector4<type>& _vector) const {
  WNVector4<type> vector = _vector;

  return (this->TransformVectorInPlace(vector));
}

template <typename type>
WN_FORCE_INLINE WNVector<type>&
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVectorInPlace(WNVector<type>& _vector) const {
  const type x = _vector.GetX();
  const type y = _vector.GetY();
  const type z = _vector.GetZ();

  _vector[0] = x * this->mElements.mValues[0] + y * this->mElements.mValues[4] +
               z * this->mElements.mValues[8] + this->mElements.mValues[12];
  _vector[1] = x * this->mElements.mValues[1] + y * this->mElements.mValues[5] +
               z * this->mElements.mValues[9] + this->mElements.mValues[13];
  _vector[2] = x * this->mElements.mValues[2] + y * this->mElements.mValues[6] +
               z * this->mElements.mValues[10] + this->mElements.mValues[14];

  return (_vector);
}

template <typename type>
WN_FORCE_INLINE WNVector<type>&
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVectorInPlace(WNVector<type>& _vector, type& _wComponent) const {
  WNVector4<type> vector = _vector.ToVector4();

  this->TransformVectorInPlace(vector);

  _vector = vector.ToVector3();
  _wComponent = vector.GetW();

  return (_vector);
}

template <typename type>
WN_FORCE_INLINE WNVector4<type>&
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVectorInPlace(WNVector4<type>& _vector) const {
  const type x = _vector.GetX();
  const type y = _vector.GetY();
  const type z = _vector.GetZ();
  const type w = _vector.GetW();

  _vector[0] = x * this->mElements.mValues[0] + y * this->mElements.mValues[4] +
               z * this->mElements.mValues[8] + w * this->mElements.mValues[12];
  _vector[1] = x * this->mElements.mValues[1] + y * this->mElements.mValues[5] +
               z * this->mElements.mValues[9] + w * this->mElements.mValues[13];
  _vector[2] = x * this->mElements.mValues[2] + y * this->mElements.mValues[6] +
               z * this->mElements.mValues[10] +
               w * this->mElements.mValues[14];
  _vector[3] = x * this->mElements.mValues[3] + y * this->mElements.mValues[7] +
               z * this->mElements.mValues[11] +
               w * this->mElements.mValues[15];

  return (_vector);
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotation(const WNQuaternion<type>& _rotation) {
  *this *= _rotation.ToMatrix4();
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotationX(const type& _angle) {
  const type sinTheta = WNSin(_angle);
  const type cosTheta = WNCos(_angle);

  // First Column
  this->mElements.mValues[0] = static_cast<type>(1);
  this->mElements.mValues[1] = static_cast<type>(0);
  this->mElements.mValues[2] = static_cast<type>(0);
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = static_cast<type>(0);
  this->mElements.mValues[5] = cosTheta;
  this->mElements.mValues[6] = sinTheta;
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = static_cast<type>(0);
  this->mElements.mValues[9] = -sinTheta;
  this->mElements.mValues[10] = cosTheta;
  this->mElements.mValues[11] = static_cast<type>(0);

  // Fourth Column
  this->mElements.mValues[12] = static_cast<type>(0);
  this->mElements.mValues[13] = static_cast<type>(0);
  this->mElements.mValues[14] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotationY(const type& _angle) {
  const type sinTheta = WNSin(_angle);
  const type cosTheta = WNCos(_angle);

  // First Column
  this->mElements.mValues[0] = cosTheta;
  this->mElements.mValues[1] = static_cast<type>(0);
  this->mElements.mValues[2] = -sinTheta;
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = static_cast<type>(0);
  this->mElements.mValues[5] = static_cast<type>(0);
  this->mElements.mValues[6] = static_cast<type>(1);
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = sinTheta;
  this->mElements.mValues[9] = static_cast<type>(0);
  this->mElements.mValues[10] = cosTheta;
  this->mElements.mValues[11] = static_cast<type>(0);

  // Fourth Column
  this->mElements.mValues[12] = static_cast<type>(0);
  this->mElements.mValues[13] = static_cast<type>(0);
  this->mElements.mValues[14] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotationZ(const type& _angle) {
  const type sinTheta = WNSin(_angle);
  const type cosTheta = WNCos(_angle);

  // First Column
  this->mElements.mValues[0] = cosTheta;
  this->mElements.mValues[1] = sinTheta;
  this->mElements.mValues[2] = static_cast<type>(0);
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = -sinTheta;
  this->mElements.mValues[5] = cosTheta;
  this->mElements.mValues[6] = static_cast<type>(1);
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = static_cast<type>(0);
  this->mElements.mValues[9] = static_cast<type>(0);
  this->mElements.mValues[10] = static_cast<type>(1);
  this->mElements.mValues[11] = static_cast<type>(0);

  // Fourth Column
  this->mElements.mValues[12] = static_cast<type>(0);
  this->mElements.mValues[13] = static_cast<type>(0);
  this->mElements.mValues[14] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotationAxis(const type& _angle, const WNVector<type>& _axis) {
  this->MakeRotationAxis(_angle, _axis.GetX(), _axis.GetY(), _axis.GetZ());
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotationAxis(
        const type& _angle, const type& _x, const type& _y, const type& _z) {
  const type x2 = _x * _x;
  const type y2 = _y * _y;
  const type z2 = _z * _z;
  const type xy = _x * _y;
  const type xz = _x * _z;
  const type yz = _y * _z;
  const type sinTheta = WNSin(_angle);
  const type cosTheta = WNCos(_angle);
  const type oneMinusCosTheta = static_cast<type>(1) - cosTheta;
  const type xyOneMinusCosTheta = xy * oneMinusCosTheta;
  const type xzOneMinusCosTheta = xz * oneMinusCosTheta;
  const type yzOneMinusCosTheta = yz * oneMinusCosTheta;
  const type xSinTheta = _x * sinTheta;
  const type ySinTheta = _y * sinTheta;
  const type zSinTheta = _z * sinTheta;

  // First Column
  this->mElements.mValues[0] = cosTheta + x2 * oneMinusCosTheta;
  this->mElements.mValues[1] = xyOneMinusCosTheta + zSinTheta;
  this->mElements.mValues[2] = xzOneMinusCosTheta - ySinTheta;
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = xyOneMinusCosTheta - zSinTheta;
  this->mElements.mValues[5] = cosTheta + y2 * oneMinusCosTheta;
  this->mElements.mValues[6] = yzOneMinusCosTheta + xSinTheta;
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = xzOneMinusCosTheta + ySinTheta;
  this->mElements.mValues[9] = yzOneMinusCosTheta - xSinTheta;
  this->mElements.mValues[10] = cosTheta + z2 * oneMinusCosTheta;
  this->mElements.mValues[11] = static_cast<type>(0);

  // Fourth Column
  this->mElements.mValues[12] = static_cast<type>(0);
  this->mElements.mValues[13] = static_cast<type>(0);
  this->mElements.mValues[14] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotation(const WNQuaternion<type>& _rotation) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotation(_rotation), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotationX(const type& _angle) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotationX(_angle), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotationY(const type& _angle) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotationY(_angle), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotationZ(const type& _angle) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotationZ(_angle), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotationAxis(const type& _angle, const WNVector<type>& _axis) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotationAxis(_angle, _axis), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotationAxis(
        const type& _angle, const type& _x, const type& _y, const type& _z) {
  WNMatrix4<type> matrix;

  return (matrix.MakeRotationAxis(_angle, _x, _y, _z), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreatePerspective(const type& _fov, const type& _aspect, const type& _near,
        const type& _far) {
  const type yScale = static_cast<type>(1) / WNTan(_fov / static_cast<type>(2));
  const type xScale = yScale / _aspect;
  const type nearFarDiff = _near - _far;
  WNMatrix4<type> matrix;

  // First Column
  matrix.mElements.mValues[0] = xScale;
  matrix.mElements.mValues[1] = static_cast<type>(0);
  matrix.mElements.mValues[2] = static_cast<type>(0);
  matrix.mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  matrix.mElements.mValues[4] = static_cast<type>(0);
  matrix.mElements.mValues[5] = yScale;
  matrix.mElements.mValues[6] = static_cast<type>(0);
  matrix.mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  matrix.mElements.mValues[8] = static_cast<type>(0);
  matrix.mElements.mValues[9] = static_cast<type>(0);
  matrix.mElements.mValues[10] = _far / nearFarDiff;
  matrix.mElements.mValues[11] = static_cast<type>(-1);

  // Fourth Column
  matrix.mElements.mValues[12] = static_cast<type>(0);
  matrix.mElements.mValues[13] = static_cast<type>(0);
  matrix.mElements.mValues[14] = (_near * _far) / nearFarDiff;
  matrix.mElements.mValues[15] = static_cast<type>(1);

  return (matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateOrthographic(const type& _width, const type& _height,
        const type& _near, const type& _far) {
  const type farNearDiff = _far - _near;
  WNMatrix4<type> matrix;

  // First Column
  matrix.mElements.mValues[0] = static_cast<type>(2) / _width;
  matrix.mElements.mValues[1] = static_cast<type>(0);
  matrix.mElements.mValues[2] = static_cast<type>(0);
  matrix.mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  matrix.mElements.mValues[4] = static_cast<type>(0);
  matrix.mElements.mValues[5] = static_cast<type>(2) / _height;
  matrix.mElements.mValues[6] = static_cast<type>(0);
  matrix.mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  matrix.mElements.mValues[8] = static_cast<type>(0);
  matrix.mElements.mValues[9] = static_cast<type>(0);
  matrix.mElements.mValues[10] = -static_cast<type>(2) / farNearDiff;
  matrix.mElements.mValues[11] = static_cast<type>(0);

  // Fourth Column
  matrix.mElements.mValues[12] = static_cast<type>(0);
  matrix.mElements.mValues[13] = static_cast<type>(0);
  matrix.mElements.mValues[14] = -(_far + _near) / farNearDiff;
  matrix.mElements.mValues[15] = static_cast<type>(1);

  return (matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>
__WNMatrix4Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateView(const WNVector<type>& _position, const WNVector<type>& _target,
        const WNVector<type>& _up) {
  const WNVector<type> forward = (_position - _target).GetNormalized();
  const WNVector<type> right = (_up.Cross(forward)).GetNormalized();
  const WNVector<type> up = forward.Cross(right);
  WNMatrix4<type> matrix;

  // First Column
  matrix.mElements.mValues[0] = right.GetX();
  matrix.mElements.mValues[1] = right.GetY();
  matrix.mElements.mValues[2] = right.GetZ();
  matrix.mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  matrix.mElements.mValues[4] = up.GetX();
  matrix.mElements.mValues[5] = up.GetY();
  matrix.mElements.mValues[6] = up.GetZ();
  matrix.mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  matrix.mElements.mValues[8] = forward.GetX();
  matrix.mElements.mValues[9] = forward.GetY();
  matrix.mElements.mValues[10] = forward.GetZ();
  matrix.mElements.mValues[11] = static_cast<type>(0);

  // Forth Column
  matrix.mElements.mValues[12] = -right.Dot(_position);
  matrix.mElements.mValues[13] = -up.Dot(_position);
  matrix.mElements.mValues[14] = -forward.Dot(_position);
  matrix.mElements.mValues[15] = static_cast<type>(1);

  return (matrix);
}
}
}

#endif  // __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__