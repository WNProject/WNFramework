// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_SEGMENT_3_INL__
#define __WN_MATH_SEGMENT_3_INL__

#ifndef __WN_MATH_SEGMENT_3_H__
#error                                                                         \
    "WNSegment3.inl should never be included directly. Please include WNSegment3.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"
#include "WNMath/inc/WNRay2.h"
#include "WNMath/inc/WNRay3.h"
#include "WNMath/inc/WNVector3.h"

namespace wn {
template <typename type>
WNSegment3<type>::WNSegment3()
  : mStart(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)),
    mEnd(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)) {}

template <typename type>
WNSegment3<type>::WNSegment3(const type* _numbers)
  : mStart(_numbers[0], _numbers[1], _numbers[2]),
    mEnd(_numbers[3], _numbers[4], _numbers[5]) {}

template <typename type>
WNSegment3<type>::WNSegment3(
    const WNVector3<type>& _start, const WNVector3<type>& _end)
  : mStart(_start), mEnd(_end) {}

template <typename type>
WNSegment3<type> WNSegment3<type>::operator+() const {
  return (*this);
}

template <typename type>
WNSegment3<type> WNSegment3<type>::operator-() const {
  return (WNSegment3<type>(-mStart, -mEnd));
}

template <typename type>
bool WNSegment3<type>::operator==(const WNSegment3<type>& _segment) const {
  return (mStart == _segment.start && mEnd == _segment.end);
}

template <typename type>
bool WNSegment3<type>::operator!=(const WNSegment3<type>& _segment) const {
  return (mStart != _segment.start || mEnd != _segment.end);
}

template <typename type>
void WNSegment3<type>::Zero() {
  mStart.Zero();
  mEnd.Zero();
}

template <typename type>
void WNSegment3<type>::Set(const type* _numbers) {
  mStart.mX = _numbers[0];
  mStart.mY = _numbers[1];
  mStart.mZ = _numbers[2];

  mEnd.mX = _numbers[3];
  mEnd.mY = _numbers[4];
  mEnd.mZ = _numbers[5];
}

template <typename type>
void WNSegment3<type>::Set(
    const WNVector3<type>& _start, const WNVector3<type>& _end) {
  mStart = _start;
  mEnd = _end;
}

template <typename type>
type WNSegment3<type>::Length() const {
  return (WNDistance(mStart, mEnd));
}

template <typename type>
type WNSegment3<type>::LengthSquared() const {
  return (WNDistanceSquared(mStart, mEnd));
}

template <typename type>
WNVector3<type> WNSegment3<type>::Direction() const {
  return (mEnd - mStart);
}

template <typename type>
WNLine2<type> WNSegment3<type>::ToLine2() const {
  return (WNLine2<type>(mStart.ToPoint2(), mEnd.ToPoint2()));
}

template <typename type>
WNLine3<type> WNSegment3<type>::ToLine3() const {
  return (WNLine3<type>(mStart, mEnd));
}

template <typename type>
WNRay2<type> WNSegment3<type>::ToRay2() const {
  return (WNRay2<type>(mStart.ToPoint2(), (mEnd - mStart).ToVector2()));
}

template <typename type>
WNRay3<type> WNSegment3<type>::ToRay3() const {
  return (WNRay3<type>(mStart, mEnd - mStart));
}
}

#endif  // __WN_MATH_SEGMENT_3_INL__