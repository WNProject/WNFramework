// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RECTANGLE_H__
#define __WN_MATH_RECTANGLE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/vector3.h"

namespace wn {
template <typename type>
class WNBounds2;
template <typename type>
class circle;

template <typename type>
class WNRectangle {
public:
  WNRectangle();
  explicit WNRectangle(const type* _numbers);
  explicit WNRectangle(const vector3<type>& _location,
      const vector3<type>& _extents, type _rotation);
  explicit WNRectangle(type _x1, type _y1, type _x2, type _y2, type _rotation);

  bool operator==(const WNRectangle& _rectangle) const;
  bool operator!=(const WNRectangle& _rectangle) const;

  void Zero();
  void Expand(type _amount);
  bool Expand(const vector3<type>& _vector, bool _anchor = false);
  bool Expand(const WNBounds2<type>& _bounds, bool _anchor = false);
  bool Expand(const WNRectangle& _rectangle, bool _anchor = false);
  bool Expand(const circle<type>& _circle, bool _anchor = false);
  void Translate(type _x, type _y);
  void Translate(const vector3<type>& _vector);
  void Rotate(type _angle);
  void Scale(type _scale);
  void Scale(type _x, type _y);
  void Scale(const vector3<type>& _scale);

  type Length() const;
  type Width() const;
  type Perimeter() const;
  type Area() const;

  bool IsZero() const;
  bool IsInsideOut() const;

  void Set(const type* _f);
  void Set(
      const vector3<type>& _location, vector3<type>& _extents, type _rotation);
  void Set(type _x1, type _y1, type _x2, type _y2, type _rotation);

  WNRectangle GetExpanded(type _amount) const;
  WNRectangle GetExpanded(
      const vector3<type>& _vector, bool _anchor = false) const;
  WNRectangle GetExpanded(
      const WNBounds2<type>& _bounds, bool _anchor = false) const;
  WNRectangle GetExpanded(
      const circle<type>& _circle, bool _anchor = false) const;
  WNRectangle GetExpanded(
      const WNRectangle& _rectangle, bool _anchor = false) const;
  WNRectangle GetTranslated(type _x, type _y) const;
  WNRectangle GetTranslated(const vector3<type>& _vector) const;
  WNRectangle GetRotated(type _angle) const;
  WNRectangle GetScaled(type _scale) const;
  WNRectangle GetScaled(type _x, type _y) const;
  WNRectangle GetScaled(const vector3<type>& _scale) const;
  void GetPoints(vector3<type>* _vectors) const;

public:
  vector3<type> mLocation;
  vector3<type> mExtents;
  type mRotation;
};
}

#include "WNMath/inc/Internal/WNRectangle.inl"

#endif  // __WN_MATH_RECTANGLE_H__