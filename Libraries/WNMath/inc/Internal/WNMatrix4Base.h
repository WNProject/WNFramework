// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_4_BASE_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace wn {
template <typename type>
class WNVector3;

template <typename type>
class WNVector4;

template <typename type>
class WNMatrix4;

template <typename type>
class WNQuaternion;

namespace internal {
template <typename type, typename = std::enable_if<true>::type>
class __WNMatrix4Base : public __WNMatrixBase<WNMatrix4<type>, type, 4> {
protected:
  WN_FORCE_INLINE __WNMatrix4Base() {}
};

template <typename type>
class __WNMatrix4Base<type,
    typename std::enable_if<wn::is_real<type>::value>::type>
    : public __WNMatrixBase<WNMatrix4<type>, type, 4> {
public:
  WNVector3<type> TransformVector(const WNVector3<type>& _vector) const;
  WNVector3<type> TransformVector(
      const WNVector3<type>& _vector, type& _wComponent) const;
  WNVector4<type> TransformVector(const WNVector4<type>& _vector) const;
  WNVector3<type>& TransformVectorInPlace(WNVector3<type>& _vector) const;
  WNVector3<type>& TransformVectorInPlace(
      WNVector3<type>& _vector, type& _wComponent) const;
  WNVector4<type>& TransformVectorInPlace(WNVector4<type>& _vector) const;

  void MakeRotation(const WNQuaternion<type>& _rotation);
  void MakeRotationX(const type& _angle);
  void MakeRotationY(const type& _angle);
  void MakeRotationZ(const type& _angle);
  void MakeRotationAxis(const type& _angle, const WNVector3<type>& _axis);
  void MakeRotationAxis(
      const type& _angle, const type& _x, const type& _y, const type& _z);

public:
  static WNMatrix4<type> CreateRotation(const WNQuaternion<type>& _rotation);
  static WNMatrix4<type> CreateRotationX(const type& _angle);
  static WNMatrix4<type> CreateRotationY(const type& _angle);
  static WNMatrix4<type> CreateRotationZ(const type& _angle);
  static WNMatrix4<type> CreateRotationAxis(
      const type& _angle, const WNVector3<type>& _axis);
  static WNMatrix4<type> CreateRotationAxis(
      const type& _angle, const type& _x, const type& _y, const type& _z);
  static WNMatrix4<type> CreatePerspective(const type& _fov,
      const type& _aspect, const type& _near, const type& _far);
  static WNMatrix4<type> CreateOrthographic(const type& _width,
      const type& _height, const type& _near, const type& _far);
  static WNMatrix4<type> CreateView(const WNVector3<type>& _position,
      const WNVector3<type>& _target, const WNVector3<type>& _up);

protected:
  WN_FORCE_INLINE __WNMatrix4Base() {}
};
}
}

#include "WNMath/inc/Internal/WNMatrix4Base.inl"

#endif  // __WN_MATH_INTERNAL_MATRIX_4_BASE_H__