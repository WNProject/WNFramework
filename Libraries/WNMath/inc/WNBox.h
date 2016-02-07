// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOX_H__
#define __WN_MATH_BOX_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNQuaternion.h"
#include "WNMath/inc/WNVector.h"

namespace wn {
template <typename type>
class WNBounds3;
template <typename type>
class sphere;

template <typename type>
class WNBox {
public:
  WNBox();
  explicit WNBox(const type* _numbers);
  explicit WNBox(const vector3<type>& _location, const vector3<type>& _extents,
      const WNQuaternion<type>& _rotation);
  explicit WNBox(const vector3<type>& _vector);

  bool operator==(const WNBox& _rectangle) const;
  bool operator!=(const WNBox& _rectangle) const;

  void Zero();
  void Expand(type _amount);
  bool Expand(const vector3<type>& _vector, bool _anchor = false);
  bool Expand(const sphere<type>& _sphere, bool _anchor = false);
  bool Expand(const WNBounds3<type>& _bounds, bool _anchor = false);
  bool Expand(const WNBox& _box, bool _anchor = false);
  void Translate(type _x, type _y, type _z);
  void Translate(const vector3<type>& _vector);
  void Rotate(const WNQuaternion<type>& _rotation);
  void Scale(type _scale);
  void Scale(type _x, type _y, type _z);
  void Scale(const vector3<type>& _scale);

  type Length() const;
  type Width() const;
  type Height() const;
  type SurfaceArea() const;
  type Volume() const;

  bool IsZero() const;
  bool IsInsideOut() const;

  void Set(const type* _numbers);
  void Set(const vector3<type>& _location, vector3<type>& _extents,
      const WNQuaternion<type>& _rotation);
  void Set(const vector3<type>& _vector);

  WNBox GetExpanded(type _amount) const;
  WNBox GetExpanded(const vector3<type>& _vector, bool _anchor = false) const;
  WNBox GetExpanded(const sphere<type>& _sphere, bool _anchor = false) const;
  WNBox GetExpanded(const WNBounds3<type>& _bounds, bool _anchor = false) const;
  WNBox GetExpanded(const WNBox& _box, bool _anchor = false) const;
  WNBox GetTranslated(type _x, type _y, type _z);
  WNBox GetTranslated(const vector3<type>& _vector) const;
  WNBox GetRotated(const WNQuaternion<type>& _rotation) const;
  WNBox GetScaled(type _scale) const;
  WNBox GetScaled(type _x, type _y, type _z) const;
  WNBox GetScaled(const vector3<type>& _scale) const;

  void GetPoints(vector3<type>* _vectors) const;

public:
  vector3<type> mLocation;
  vector3<type> mExtents;
  WNQuaternion<type> mRotation;
};
}

#include "WNMath/inc/Internal/WNBox.inl"

#endif  // __WN_MATH_BOX_H__