// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_SEGMENT_3_H__
#define __WN_MATH_SEGMENT_3_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
template <typename type>
class WNLine2;
template <typename type>
class WNLine3;
template <typename type>
class WNRay2;
template <typename type>
class WNRay3;
template <typename type>
class vector3;

template <typename type>
class WNSegment3 {
public:
  WNSegment3();
  explicit WNSegment3(const type* _numbers);
  explicit WNSegment3(const vector3<type>& _start, const vector3<type>& _end);

  WNSegment3 operator+() const;
  WNSegment3 operator-() const;

  bool operator==(const WNSegment3& _segment) const;
  bool operator!=(const WNSegment3& _segment) const;

  void Zero();

  type Length() const;
  type LengthSquared() const;
  vector3<type> Direction() const;

  bool IsZero() const;

  void Set(const type* _numbers);
  void Set(const vector3<type>& _start, const vector3<type>& _end);

  WNLine2<type> ToLine2() const;
  WNLine3<type> ToLine3() const;
  WNRay2<type> ToRay2() const;
  WNRay3<type> ToRay3() const;

public:
  vector3<type> mStart;
  vector3<type> mEnd;
};
}

#include "WNMath/inc/Internal/WNSegment3.inl"

#endif  // __WN_MATH_SEGMENT_3_H__