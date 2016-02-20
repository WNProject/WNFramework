// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RECTANGLE_INL__
#define __WN_MATH_RECTANGLE_INL__

#ifndef __WN_MATH_RECTANGLE_H__
#error                                                                         \
    "WNRectangle.inl should never be included directly. Please include WNRectangle.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBounds2.h"
#include "WNMath/inc/WNCircle.h"
#include "WNMath/inc/WNVector3.h"

namespace wn {
template <typename type>
WNRectangle<type>::WNRectangle()
  : mLocation(static_cast<type>(0), static_cast<type>(0)),
    mExtents(static_cast<type>(0), static_cast<type>(0)),
    mRotation(static_cast<type>(0)) {}

template <typename type>
WNRectangle<type>::WNRectangle(const type* _numbers)
  : mLocation(_numbers[0], _numbers[1]),
    mExtents(_numbers[2], _numbers[3]),
    mRotation(_numbers[4]) {}

template <typename type>
WNRectangle<type>::WNRectangle(const WNVector3<type>& _location,
    const WNVector3<type>& _extents, type _rotation)
  : mLocation(_location), mExtents(_extents), mRotation(_rotation) {}

template <typename type>
WNRectangle<type>::WNRectangle(
    type _x1, type _y1, type _x2, type _y2, type _rotation)
  : mLocation(_x1, _y1), mExtents(_x2, _y2), mRotation(_rotation) {}

template <typename type>
bool WNRectangle<type>::operator==(const WNRectangle<type>& _rectangle) const {
  return (mExtents == _rectangle.mExtents &&
          mLocation == _rectangle.mLocation &&
          mRotation == _rectangle.mRotation);
}

template <typename type>
bool WNRectangle<type>::operator!=(const WNRectangle<type>& _rectangle) const {
  return (mExtents != _rectangle.mExtents ||
          mLocation != _rectangle.mLocation ||
          mRotation != _rectangle.mRotation);
}

template <typename type>
void WNRectangle<type>::Zero() {
  mLocation.Zero();
  mExtents.Zero();
  mRotation = static_cast<type>(0);
}

template <typename type>
void WNRectangle<type>::Expand(type _amount) {
  mExtents += _amount;
}

template <typename type>
bool WNRectangle<type>::Expand(const WNVector3<type>& _vector, bool _anchor) {
  const WNVector3<type> transformedPoint =
      ((_vector - mLocation).GetRotated(-mRotation).ToPoint2());

  if (!_anchor) {
    WNBounds2<type> bounds =
        WNBounds2<type>(-mExtents.ToPoint2(), mExtents.ToPoint2());

    if (bounds.Expand(transformedPoint)) {
      const WNRectangle<type> newRectange = bounds.ToRectangle();

      mLocation += newRectange.mLocation.ToVector2();
      mExtents = newRectange.mExtents;

      return (true);
    }
  } else {
    const type transformedAbsX = WNAbs(transformedPoint.mX);
    const type transformedAbsY = WNAbs(transformedPoint.mY);
    const type extendsAbsX = WNAbs(mExtents.mX);
    const type extendsAbsY = WNAbs(mExtents.mY);

    bool expanded = false;

    if (transformedAbsX > extendsAbsX) {
      mExtents.mX = transformedPoint.mX;

      expanded = true;
    }

    if (transformedAbsY > extendsAbsY) {
      mExtents.mY = transformedPoint.mY;

      expanded = true;
    }

    return (expanded);
  }

  return (false);
}

template <typename type>
bool WNRectangle<type>::Expand(const WNBounds2<type>& _bounds, bool _anchor) {
  WNVector3<type> points[4];

  _bounds.GetPoints(points);

  return (Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
          Expand(points[2], _anchor) || Expand(points[3], _anchor));
}

template <typename type>
bool WNRectangle<type>::Expand(
    const WNRectangle<type>& _rectangle, bool _anchor) {
  WNVector3<type> points[4];

  _rectangle.GetPoints(points);

  return (Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
          Expand(points[2], _anchor) || Expand(points[3], _anchor));
}

template <typename type>
bool WNRectangle<type>::Expand(const WNCircle<type>& _circle, bool _anchor) {
  const WNVector3<type> transformedLocation =
      ((_circle.mLocation - mLocation).GetRotated(-mRotation).ToPoint2());
  const WNVector3<type> componentX =
      WNVector3<type>(_circle.mRadius, static_cast<type>(0));
  const WNVector3<type> componentY =
      WNVector3<type>(static_cast<type>(0), _circle.mRadius);
  const WNVector3<type> point1 = transformedLocation + componentX;
  const WNVector3<type> point2 = transformedLocation - componentX;
  const WNVector3<type> point3 = transformedLocation + componentY;
  const WNVector3<type> point4 = transformedLocation - componentY;

  return (Expand(point1, _anchor) || Expand(point2, _anchor) ||
          Expand(point3, _anchor) || Expand(point4, _anchor));
}

template <typename type>
void WNRectangle<type>::Translate(type _x, type _y) {
  mLocation.mX += _x;
  mLocation.mY += _y;
}

template <typename type>
void WNRectangle<type>::Translate(const WNVector3<type>& _vector) {
  mLocation += _vector;
}

template <typename type>
void WNRectangle<type>::Rotate(type _angle) {
  mExtents.Scale(_angle);
}

template <typename type>
void WNRectangle<type>::Scale(type _scale) {
  mExtents.Scale(_scale);
}

template <typename type>
void WNRectangle<type>::Scale(type _x, type _y) {
  mExtents.Scale(_x, _y);
}

template <typename type>
void WNRectangle<type>::Scale(const WNVector3<type>& _scale) {
  mExtents.Scale(_scale);
}

template <typename type>
type WNRectangle<type>::Length() const {
  return (WNAbs(static_cast<type>(2) * mExtents.mX));
}

template <typename type>
type WNRectangle<type>::Width() const {
  return (WNAbs(static_cast<type>(2) * mExtents.mY));
}

template <typename type>
type WNRectangle<type>::Perimeter() const {
  return (
      static_cast<type>(4) * mExtents.mX + static_cast<type>(4) * mExtents.mY);
}

template <typename type>
type WNRectangle<type>::Area() const {
  const WNVector3<type> dimensions = mExtents * static_cast<type>(2);

  return (dimensions.mX * dimensions.mY);
}

template <typename type>
bool WNRectangle<type>::IsZero() const {
  return (mLocation.IsZero() && mExtents.IsZero() &&
          mRotation == static_cast<type>(0));
}

template <typename type>
bool WNRectangle<type>::IsInsideOut() const {
  return (
      mExtents.mX < static_cast<type>(0) || mExtents.mY < static_cast<type>(0));
}

template <typename type>
void WNRectangle<type>::Set(const type* _numbers) {
  mLocation.mX = _numbers[0];
  mLocation.mY = _numbers[1];
  mExtents.mX = _numbers[2];
  mExtents.mY = _numbers[3];
  mRotation = _numbers[4];
}

template <typename type>
void WNRectangle<type>::Set(const WNVector3<type>& _location,
    WNVector3<type>& _extents, type _rotation) {
  mLocation = _location;
  mExtents = _extents;
  mRotation = _rotation;
}

template <typename type>
void WNRectangle<type>::Set(
    type _x1, type _y1, type _x2, type _y2, type _rotation) {
  mLocation.mX = _x1;
  mLocation.mY = _y1;
  mExtents.mX = _x2;
  mExtents.mY = _y2;
  mRotation = _rotation;
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetExpanded(type _amount) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Expand(_amount), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetExpanded(
    const WNVector3<type>& _vector, bool _anchor) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Expand(_vector, _anchor), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetExpanded(
    const WNBounds2<type>& _bounds, bool _anchor) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Expand(_bounds, _anchor), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetExpanded(
    const WNRectangle<type>& _rectangle, bool _anchor) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Expand(_rectangle, _anchor), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetExpanded(
    const WNCircle<type>& _circle, bool _anchor) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Expand(_circle, _anchor), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetTranslated(type _x, type _y) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Translate(_x, _y), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetTranslated(
    const WNVector3<type>& _vector) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Translate(_vector), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetRotated(type _angle) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Rotate(_angle), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetScaled(type _scale) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Scale(_scale), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetScaled(type _x, type _y) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Scale(_x, _y), rectangle);
}

template <typename type>
WNRectangle<type> WNRectangle<type>::GetScaled(
    const WNVector3<type>& _scale) const {
  WNRectangle<type> rectangle = *this;

  return (rectangle.Scale(_scale), rectangle);
}

template <typename type>
void WNRectangle<type>::GetPoints(WNVector3<type>* _vectors) const {
  const WNVector3<type> transformedExtents = mExtents.GetRotated(mRotation);
  const WNVector3<type> transformedFlippedXExtents =
      (mExtents * WNVector3<type>(static_cast<type>(-1), static_cast<type>(1)))
          .GetRotated(mRotation);
  const WNVector3<type> transformedFlippedYExtents =
      (mExtents * WNVector3<type>(static_cast<type>(1), -static_cast<type>(-1)))
          .GetRotated(mRotation);

  _vectors[0] = mLocation + transformedExtents;
  _vectors[1] = mLocation + transformedFlippedXExtents;
  _vectors[2] = mLocation + transformedFlippedYExtents;
  _vectors[3] = mLocation - transformedExtents;
}
}

#endif  // __WN_MATH_RECTANGLE_INL__
