// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_PLANE_H__
#define __WN_MATH_PLANE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNConfig.h"

namespace wn {
namespace internal {
template <typename type>
struct __WNPlaneBase {
  type mA;
  type mB;
  type mC;
  type mD;
};
}

template <typename type>
class vector3;

template <typename type>
class WNPlane : public internal::__WNPlaneBase<type> {
public:
  WNPlane();
  explicit WNPlane(const type* _numbers);
  explicit WNPlane(const vector3<type>& _normal, type _distance);
  explicit WNPlane(type _a, type _b, type _c, type _d);

  operator type*();
  operator const type*() const;

  WNPlane& operator+=(const WNPlane& _plane);
  WNPlane& operator-=(const WNPlane& _plane);

  WNPlane operator+() const;
  WNPlane operator-() const;

  WNPlane operator+(const WNPlane& _plane) const;
  WNPlane operator-(const WNPlane& _plane) const;

  bool operator==(const WNPlane& _plane) const;
  bool operator!=(const WNPlane& _plane) const;

  void Zero();
  void Normalize();
  void Translate(const vector3<type>& _translation);

  vector3<type> Normal() const;
  type Distance() const;

  bool IsZero() const;

  void Set(const type* _numbers);
  void Set(type _a, type _b, type _c, type _d);

  void Define(const vector3<type>& _vector1, const vector3<type>& _vector2,
      const vector3<type>& _vector3);

  WNPlane GetNormalized() const;
  WNPlane GetTranslated(const vector3<type>& _translation) const;

  type* RetrieveRaw(type* _numbers);

  void Prefetch() const;

  template <typename NewType>
  WNPlane<NewType> ConvertTo() const;
};
}

#include "WNMath/inc/Internal/WNPlane.inl"

#endif  // __WN_MATH_PLANE_H__