// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOUNDS_2_H__
#define __WN_MATH_BOUNDS_2_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
template <typename type>
class WNRectangle;
template <typename type>
class circle;
template <typename type>
class vector2;

template <typename type>
class WNBounds2 {
public:
  WNBounds2();
  explicit WNBounds2(const type* _numbers);
  explicit WNBounds2(const vector2<type>& _vector);
  explicit WNBounds2(const vector2<type>& _min, const vector2<type>& _max);

  bool operator==(const WNBounds2& _bounds) const;
  bool operator!=(const WNBounds2& _bounds) const;

  void Zero();
  void Expand(type _amount);
  bool Expand(const vector2<type>& _vector);
  bool Expand(const WNRectangle<type>& _rectangle);
  bool Expand(const circle<type>& _circle);
  bool Expand(const WNBounds2& _bounds);
  void Translate(const vector2<type>& _vector);

  type Length() const;
  type Width() const;
  type Perimeter() const;
  type Radius() const;
  type Area() const;

  bool IsZero() const;
  bool IsInsideOut() const;

  void Set(const type* _numbers);
  void Set(const vector2<type>& _vector);
  void Set(const vector2<type>& _min, const vector2<type>& _max);

  WNBounds2 GetExpanded(type _amount) const;
  WNBounds2 GetExpanded(const vector2<type>& _vector) const;
  WNBounds2 GetExpanded(const WNRectangle<type>& _rectangle) const;
  WNBounds2 GetExpanded(const circle<type>& _circle) const;
  WNBounds2 GetExpanded(const WNBounds2& _bounds) const;
  WNBounds2 GetTranslated(const vector2<type>& _vector) const;

  void GetPoints(vector2<type>* _vectors) const;

  WNRectangle<type> ToRectangle() const;

public:
  vector2<type> mMinimum;
  vector2<type> mMaximum;
};
}

#include "WNMath/inc/Internal/WNBounds2.inl"

#endif  // __WN_MATH_BOUNDS_2_H__