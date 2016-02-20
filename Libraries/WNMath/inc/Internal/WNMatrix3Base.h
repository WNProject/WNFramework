// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_3_BASE_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace wn {
template <typename type>
class WNVector2;

template <typename type>
class WNVector3;

template <typename type>
class WNMatrix3;

namespace internal {
template <typename type, typename = std::enable_if<true>::type>
class __WNMatrix3Base : public __WNMatrixBase<WNMatrix3<type>, type, 3> {
protected:
  WN_FORCE_INLINE __WNMatrix3Base() {}
};

template <typename type>
class __WNMatrix3Base<type,
    typename std::enable_if<wn::is_real<type>::value>::type>
    : public __WNMatrixBase<WNMatrix3<type>, type, 3> {
public:
  WNVector2<type> TransformVector(const WNVector2<type>& _vector) const;
  WNVector2<type> TransformVector(
      const WNVector2<type>& _vector, type& _zComponent) const;
  WNVector3<type> TransformVector(const WNVector3<type>& _vector) const;
  WN_FORCE_INLINE WNVector2<type>& TransformVectorInPlace(
      WNVector2<type>& _vector) const {}
  WN_FORCE_INLINE WNVector2<type>& TransformVectorInPlace(
      WNVector2<type>& _vector, type& _zComponent) const {}
  WN_FORCE_INLINE WNVector3<type>& TransformVectorInPlace(
      WNVector3<type>& _vector) const {}

  void MakeRotation(const type& _angle);
  void MakeOrthographic(const type& _width, const type& _height,
      const type& _near, const type& _far);

public:
  static WNMatrix3<type> CreateRotation(const type& _angle);
  static WNMatrix3<type> CreateOrthographic(const type& _width,
      const type& _height, const type& _near, const type& _far);

protected:
  WN_FORCE_INLINE __WNMatrix3Base() {}
};
}
}

#include "WNMath/inc/Internal/WNMatrix3Base.inl"

#endif  // __WN_MATH_INTERNAL_MATRIX_3_BASE_H__