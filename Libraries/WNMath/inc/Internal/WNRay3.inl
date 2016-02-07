// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RAY_3_INL__
#define __WN_MATH_RAY_3_INL__

#ifndef __WN_MATH_RAY_3_H__
#error                                                                         \
    "WNRay3.inl should never be included directly. Please include WNRay2.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"

namespace wn {
template <typename type>
WNRay3<type>::WNRay3()
  : mLocation(static_cast<type>(0)), mDirection(static_cast<type>(0)) {}

template <typename type>
WNRay3<type>::WNRay3(const type* _numbers)
  : mLocation(&_numbers[0]), mDirection(&_numbers[3]) {}

template <typename type>
WNRay3<type>::WNRay3(
    const WNVector3<type>& _location, const WNVector3<type>& _direction)
  : mLocation(_location), mDirection(_direction) {}

template <typename type>
WNRay3<type> WNRay3<type>::operator+() const {
  return (*this);
}

template <typename type>
WNRay3<type> WNRay3<type>::operator-() const {
  return (WNRay3<type>(-mLocation, -mDirection));
}

template <typename type>
bool WNRay3<type>::operator==(const WNRay3<type>& _ray) const {
  return (mLocation == _ray.mLocation && mDirection == _ray.mDirection);
}

template <typename type>
bool WNRay3<type>::operator!=(const WNRay3<type>& _ray) const {
  return (mLocation != _ray.mLocation || mDirection != _ray.mDirection);
}

template <typename type>
void WNRay3<type>::Zero() {
  mLocation.Zero();
  mDirection.Zero();
}
template <typename type>
void WNRay3<type>::Set(const type* _numbers) {
  mLocation.Set(&_numbers[0]);
  mDirection.Set(&_numbers[3]);
}

template <typename type>
void WNRay3<type>::Set(
    const WNVector3<type>& _location, const WNVector3<type>& _direction) {
  mLocation = _location;
  mDirection = _direction;
}

template <typename type>
WNLine2<type> WNRay3<type>::ToLine2() const {
  return (
      WNLine2<type>(mLocation.ToPoint2(), (mLocation + mDirection).ToPoint2()));
}

template <typename type>
WNLine3<type> WNRay3<type>::ToLine3() const {
  return (WNLine3<type>(mLocation, mLocation + mDirection));
}
}

#endif  // __WN_MATH_RAY_3_INL__