// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_LINE_3_INL__
#define __WN_MATH_LINE_3_INL__

#ifndef __WN_MATH_LINE_3_H__
    #error "WNLine3.inl should never be included directly. Please include WNLine3.h instead"
#endif

#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNLine3<Type>::WNLine3() :
        mPoint1(static_cast<Type>(0)),
        mPoint2(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNLine3<Type>::WNLine3(const WNLine3<Type>& _line) {
        Set(_line);
    }

    template <typename Type>
    WNLine3<Type>::WNLine3(const Type* _numbers) :
        mPoint1(&_numbers[0]),
        mPoint2(&_numbers[3]) {
    }

    template <typename Type>
    WNLine3<Type>::WNLine3(const WNVector3<Type>& _point1, const WNVector3<Type>& _point2) :
        mPoint1(_point1),
        mPoint2(_point2) {
    }

    template <typename Type>
    WNLine3<Type> WNLine3<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNLine3<Type> WNLine3<Type>::operator - () const {
        return(WNLine3<Type>(-mPoint1, -mPoint2));
    }

    template <typename Type>
    WN_BOOL WNLine3<Type>::operator == (const WNLine3<Type>& _line) const {
        return(mPoint1 == _line.mPoint1 && mPoint2 == _line.mPoint2);
    }

    template <typename Type>
    WN_BOOL WNLine3<Type>::operator != (const WNLine3<Type>& _line) const {
        return(mPoint1 != _line.mPoint1 || mPoint2 != _line.mPoint2);
    }

    template <typename Type>
    WN_VOID WNLine3<Type>::Zero() {
        mPoint1.Zero();
        mPoint2.Zero();
    }

    template <typename Type>
    WN_VOID WNLine3<Type>::Set(const WNLine3<Type>& _line) {
        mPoint1.Set(_line.mPoint1);
        mPoint2.Set(_line.mPoint1);
    }

    template <typename Type>
    WN_VOID WNLine3<Type>::Set(const Type* _numbers) {
        mPoint1.Set(&_numbers[0]);
        mPoint2.Set(&_numbers[3]);
    }

    template <typename Type>
    WN_VOID WNLine3<Type>::Set(const WNVector3<Type>& _point1, const WNVector3<Type>& _point2) {
        mPoint1 = _point1;
        mPoint2 = _point2;
    }

    template <typename Type>
    WNVector3<Type> WNLine3<Type>::Direction() const {
        return(mPoint2 - mPoint1);
    }
}

#endif // __WN_MATH_LINE_3_INL__