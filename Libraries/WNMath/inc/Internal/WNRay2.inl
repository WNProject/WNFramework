// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_RAY_2_INL__
#define __WN_MATH_RAY_2_INL__

#ifndef __WN_MATH_RAY_2_H__
    #error "WNRay2.inl should never be included directly. Please include WNRay2.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"

namespace WNMath {
    template <typename Type>
    WNRay2<Type>::WNRay2() :
        mLocation(static_cast<Type>(0)),
        mDirection(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNRay2<Type>::WNRay2(const Type* _numbers) :
        mLocation(&_numbers[0]),
        mDirection(&_numbers[2]) {
    }

    template <typename Type>
    WNRay2<Type>::WNRay2(const WNVector2<Type>& _location, const WNVector2<Type>& _direction) :
        mLocation(_location),
        mDirection(_direction) {
    }

    template <typename Type>
    WNRay2<Type> WNRay2<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNRay2<Type> WNRay2<Type>::operator - () const {
        return(WNRay2<Type>(-mLocation, -mDirection));
    }

    template <typename Type>
    WN_BOOL WNRay2<Type>::operator == (const WNRay2<Type>& _ray) const {
        return(mLocation == _ray.mLocation && mDirection == _ray.mDirection);
    }

    template <typename Type>
    WN_BOOL WNRay2<Type>::operator != (const WNRay2<Type>& _ray) const {
        return(mLocation != _ray.mLocation || mDirection != _ray.mDirection);
    }

    template <typename Type>
    WN_VOID WNRay2<Type>::Zero() {
        mLocation.Zero();
        mDirection.Zero();
    }

    template <typename Type>
    WN_VOID WNRay2<Type>::Set(const Type* _numbers) {
        mLocation.Set(&_numbers[0]);
        mDirection.Set(&_numbers[2]);
    }

    template <typename Type>
    WN_VOID WNRay2<Type>::Set(const WNVector2<Type>& _location, const WNVector2<Type>& _direction) {
        mLocation = _location;
        mDirection = _direction;
    }

    template <typename Type>
    WNLine2<Type> WNRay2<Type>::ToLine2() const {
        return(WNLine2<Type>(mLocation, mLocation + mDirection));
    }

    template <typename Type>
    WNLine3<Type> WNRay2<Type>::ToLine3() const {
        return(WNLine3<Type>(mLocation.ToPoint3(), (mLocation + mDirection).ToPoint3()));
    }
}

#endif // __WN_MATH_RAY_2_INL__