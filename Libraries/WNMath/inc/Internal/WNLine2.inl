// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_LINE_2_INL__
#define __WN_MATH_LINE_2_INL__

#ifndef __WN_MATH_LINE_2_H__
    #error "WNLine2.inl should never be included directly. Please include WNLine2.h instead"
#endif

#include "WNMath/inc/WNVector2.h"

namespace wn {
    template <typename type>
    WNLine2<type>::WNLine2() :
        mPoint1(static_cast<type>(0)),
        mPoint2(static_cast<type>(0)) {
    }

    template <typename type>
    WNLine2<type>::WNLine2(const WNLine2<type>& _line) {
        Set(_line);
    }

    template <typename type>
    WNLine2<type>::WNLine2(const type* _numbers) :
        mPoint1(&_numbers[0]),
        mPoint2(&_numbers[2]) {
    }

    template <typename type>
    WNLine2<type>::WNLine2(const WNVector2<type>& _point1, const WNVector2<type>& _point2) :
        mPoint1(_point1),
        mPoint2(_point2) {
    }

    template <typename type>
    WNLine2<type> WNLine2<type>::operator + () const {
        return(*this);
    }

    template <typename type>
    WNLine2<type> WNLine2<type>::operator - () const {
        return(WNLine2<type>(-mPoint1, -mPoint2));
    }

    template <typename type>
    bool WNLine2<type>::operator == (const WNLine2& _line) const {
        return(mPoint1 == _line.mPoint1 && mPoint2 == _line.mPoint2);
    }

    template <typename type>
    bool WNLine2<type>::operator != (const WNLine2& _line) const {
        return(mPoint1 != _line.mPoint1 || mPoint2 != _line.mPoint2);
    }

    template <typename type>
    void WNLine2<type>::Zero() {
        mPoint1.Zero();
        mPoint2.Zero();
    }

    template <typename type>
    void WNLine2<type>::Set(const WNLine2<type>& _line) {
        *this = _line;
    }

    template <typename type>
    void WNLine2<type>::Set(const type* _numbers) {
        mPoint1.mX = _numbers[0];
        mPoint1.mY = _numbers[1];

        mPoint2.mX = _numbers[2];
        mPoint2.mY = _numbers[3];
    }

    template <typename type>
    void WNLine2<type>::Set(const WNVector2<type>& _point1, const WNVector2<type>& _point2) {
        mPoint1 = _point1;
        mPoint2 = _point2;
    }

    template <typename type>
    WNVector2<type> WNLine2<type>::Direction() const {
        return(mPoint2 - mPoint1);
    }
}

#endif // __WN_MATH_LINE_2_INL__