// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__

#ifndef __WN_MATH_MATRIX_3_H__
#error                                                                         \
    "Internal/WNMatrix3Base.inl should never be included directly. Please include WNMatrix3.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNVector.h"
#include "WNMath/inc/WNVector2.h"

namespace wn {
namespace internal {
template <typename type>
WN_FORCE_INLINE WNVector2<type>
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector2<type>& _vector) const {
  WNVector2<type> vector = _vector;

  return (this->TransformVectorInPlace(vector));
}

template <typename type>
WN_FORCE_INLINE WNVector2<type>
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector2<type>& _vector, type& _zComponent) const {
  WNVector<type> vector = _vector.ToVector3();

  this->TransformVectorInPlace(vector);

  _zComponent = vector.GetZ();

  return (vector.ToVector2());
}

template <typename type>
WN_FORCE_INLINE WNVector<type>
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    TransformVector(const WNVector<type>& _vector) const {
  WNVector<type> vector = _vector;

  return (this->TransformVectorInPlace(vector));
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeRotation(const type& _angle) {
  const type sinTheta = WNSin(_angle);
  const type cosTheta = WNCos(_angle);

  // First Column
  this->mElements.mValues[0] = cosTheta;
  this->mElements.mValues[1] = sinTheta;
  this->mElements.mValues[2] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[3] = -sinTheta;
  this->mElements.mValues[4] = cosTheta;
  this->mElements.mValues[5] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[6] = static_cast<type>(0);
  this->mElements.mValues[7] = static_cast<type>(0);
  this->mElements.mValues[8] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE void
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    MakeOrthographic(const type& _width, const type& _height, const type& _near,
        const type& _far) {
  // First Column
  this->mElements.mValues[0] = static_cast<type>(2) / _width;
  this->mElements.mValues[1] = static_cast<type>(0);
  this->mElements.mValues[2] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[3] = static_cast<type>(0);
  this->mElements.mValues[4] = static_cast<type>(-2) / _height;
  this->mElements.mValues[5] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[6] = static_cast<type>(0);
  this->mElements.mValues[7] = static_cast<type>(0);
  this->mElements.mValues[8] = static_cast<type>(1) / (_near - _far);
}

template <typename type>
WN_FORCE_INLINE WNMatrix3<type>
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateRotation(const type& _angle) {
  WNMatrix3<type> matrix;

  return (matrix.MakeRotation(_angle), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix3<type>
__WNMatrix3Base<type, typename std::enable_if<wn::is_real<type>::value>::type>::
    CreateOrthographic(const type& _width, const type& _height,
        const type& _near, const type& _far) {
  WNMatrix3<type> matrix;

  return (matrix.MakeOrthographic(_width, _height, _near, _far), matrix);
}
}
}

#endif  // __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__