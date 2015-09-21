// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_LINE_3_INL__
#define __WN_MATH_LINE_3_INL__

#ifndef __WN_MATH_LINE_3_H__
    #error "WNLine3.inl should never be included directly. Please include WNLine3.h instead"
#endif

#include "WNMath/inc/WNVector3.h"

namespace wn {
    template <typename type>
    WNLine3<type>::WNLine3() :
        mPoint1(static_cast<type>(0)),
        mPoint2(static_cast<type>(0)) {
    }

    template <typename type>
    WNLine3<type>::WNLine3(const WNLine3<type>& _line) {
        Set(_line);
    }

    template <typename type>
    WNLine3<type>::WNLine3(const type* _numbers) :
        mPoint1(&_numbers[0]),
        mPoint2(&_numbers[3]) {
    }

    template <typename type>
    WNLine3<type>::WNLine3(const WNVector3<type>& _point1, const WNVector3<type>& _point2) :
        mPoint1(_point1),
        mPoint2(_point2) {
    }

    template <typename type>
    WNLine3<type> WNLine3<type>::operator + () const {
        return(*this);
    }

    template <typename type>
    WNLine3<type> WNLine3<type>::operator - () const {
        return(WNLine3<type>(-mPoint1, -mPoint2));
    }

    template <typename type>
    wn_bool WNLine3<type>::operator == (const WNLine3<type>& _line) const {
        return(mPoint1 == _line.mPoint1 && mPoint2 == _line.mPoint2);
    }

    template <typename type>
    wn_bool WNLine3<type>::operator != (const WNLine3<type>& _line) const {
        return(mPoint1 != _line.mPoint1 || mPoint2 != _line.mPoint2);
    }

    template <typename type>
    wn_void WNLine3<type>::Zero() {
        mPoint1.Zero();
        mPoint2.Zero();
    }

    template <typename type>
    wn_void WNLine3<type>::Set(const WNLine3<type>& _line) {
        mPoint1.Set(_line.mPoint1);
        mPoint2.Set(_line.mPoint1);
    }

    template <typename type>
    wn_void WNLine3<type>::Set(const type* _numbers) {
        mPoint1.Set(&_numbers[0]);
        mPoint2.Set(&_numbers[3]);
    }

    template <typename type>
    wn_void WNLine3<type>::Set(const WNVector3<type>& _point1, const WNVector3<type>& _point2) {
        mPoint1 = _point1;
        mPoint2 = _point2;
    }

    template <typename type>
    WNVector3<type> WNLine3<type>::Direction() const {
        return(mPoint2 - mPoint1);
    }
}

#endif // __WN_MATH_LINE_3_INL__