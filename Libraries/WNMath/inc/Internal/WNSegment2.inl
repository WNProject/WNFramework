// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_SEGMENT_2_INL__
#define __WN_MATH_SEGMENT_2_INL__

#ifndef __WN_MATH_SEGMENT_2_H__
    #error "WNSegment2.inl should never be included directly. Please include WNSegment2.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"
#include "WNMath/inc/WNRay2.h"
#include "WNMath/inc/WNRay3.h"
#include "WNMath/inc/WNVector2.h"

namespace wn {
    template <typename type>
    WNSegment2<type>::WNSegment2() :
        mStart(static_cast<type>(0), static_cast<type>(0)),
        mEnd(static_cast<type>(0), static_cast<type>(0)) {
    }

    template <typename type>
    WNSegment2<type>::WNSegment2(const type* _numbers) :
        mStart(_numbers[0], _numbers[1]),
        mEnd(_numbers[2], _numbers[3]) {
    }

    template <typename type>
    WNSegment2<type>::WNSegment2(const WNVector2<type>& _point1, const WNVector2<type>& _point2) :
        mStart(_point1),
        mEnd(_point2) {
    }

    template <typename type>
    WNSegment2<type> WNSegment2<type>::operator + () const {
        return(*this);
    }

    template <typename type>
    WNSegment2<type> WNSegment2<type>::operator - () const {
        return(WNSegment2<type>(-mStart, -mEnd));
    }

    template <typename type>
    wn_bool WNSegment2<type>::operator == (const WNSegment2<type>& _segment) const {
        return(mStart == _segment.start && mEnd == _segment.end);
    }

    template <typename type>
    wn_bool WNSegment2<type>::operator != (const WNSegment2<type>& _segment) const {
        return(mStart != _segment.start || mEnd != _segment.end);
    }

    template <typename type>
    wn_void WNSegment2<type>::Zero() {
        mStart.Zero();
        mEnd.Zero();
    }

    template <typename type>
    wn_void WNSegment2<type>::Set(const type* _numbers) {
        mStart.mX = _numbers[0];
        mStart.mY = _numbers[1];

        mEnd.mX = _numbers[2];
        mEnd.mY = _numbers[3];
    }

    template <typename type>
    wn_void WNSegment2<type>::Set(const WNVector2<type>& _point1, const WNVector2<type>& _point2) {
        mStart = _point1;
        mEnd = _point2;
    }

    template <typename type>
    type WNSegment2<type>::Length() const {
        return(WNDistance(mStart, mEnd));
    }

    template <typename type>
    type WNSegment2<type>::LengthSquared() const {
        return(WNDistanceSquared(mStart, mEnd));
    }

    template <typename type>
    WNVector2<type> WNSegment2<type>::Direction() const {
        return(mEnd - mStart);
    }

    template <typename type>
    WNLine2<type> WNSegment2<type>::ToLine2() const {
        return(WNLine2<type>(mStart, mEnd));
    }

    template <typename type>
    WNLine3<type> WNSegment2<type>::ToLine3() const {
        return(WNLine3<type>(mStart.ToPoint3(), mEnd.ToPoint3()));
    }

    template <typename type>
    WNRay2<type> WNSegment2<type>::ToRay2() const {
        return(WNRay2<type>(mStart, mEnd - mStart));
    }

    template <typename type>
    WNRay3<type> WNSegment2<type>::ToRay3() const {
        return(WNRay3<type>(mStart.ToPoint3(), (mEnd - mStart).ToVector3()));
    }
}

#endif // __WN_MATH_SEGMENT_2_INL__