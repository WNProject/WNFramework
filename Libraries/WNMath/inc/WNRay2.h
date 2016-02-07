// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RAY_2_H__
#define __WN_MATH_RAY_2_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/vector2.h"

namespace wn {
template <typename type>
class WNLine2;
template <typename type>
class WNLine3;

template <typename type>
class WNRay2 {
public:
  WNRay2();
  explicit WNRay2(const type* _numbers);
  explicit WNRay2(
      const vector2<type>& _location, const vector2<type>& _direction);

  WNRay2 operator+() const;
  WNRay2 operator-() const;

  bool operator==(const WNRay2& _ray) const;
  bool operator!=(const WNRay2& _ray) const;

  void Zero();

  bool IsZero() const;

  void Set(const type* _numbers);
  void Set(const vector2<type>& _location, const vector2<type>& _direction);

  WNLine2<type> ToLine2() const;
  WNLine3<type> ToLine3() const;

public:
  vector2<type> mLocation;
  vector2<type> mDirection;
};
}

#include "WNMath/inc/Internal/WNRay2.inl"

#endif  // __WN_MATH_RAY_2_H__