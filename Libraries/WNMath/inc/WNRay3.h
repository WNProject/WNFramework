// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RAY_3_H__
#define __WN_MATH_RAY_3_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/vector3.h"

namespace wn {
template <typename type>
class WNLine2;
template <typename type>
class WNLine3;

template <typename type>
class WNRay3 {
public:
  WNRay3();
  explicit WNRay3(const type* _numbers);
  explicit WNRay3(
      const vector3<type>& _location, const vector3<type>& _direction);

  WNRay3 operator+() const;
  WNRay3 operator-() const;

  bool operator==(const WNRay3& _ray) const;
  bool operator!=(const WNRay3& _ray) const;

  void Zero();

  bool IsZero() const;

  void Set(const type* _numbers);
  void Set(const vector3<type>& _location, const vector3<type>& _direction);

  WNLine2<type> ToLine2() const;
  WNLine3<type> ToLine3() const;

public:
  vector3<type> mLocation;
  vector3<type> mDirection;
};
}

#include "WNMath/inc/Internal/WNRay3.inl"

#endif  // __WN_MATH_RAY_3_H__