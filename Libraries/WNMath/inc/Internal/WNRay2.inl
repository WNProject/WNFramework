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

namespace wn {
    template <typename type>
    WNRay2<type>::WNRay2() :
        mLocation(static_cast<type>(0)),
        mDirection(static_cast<type>(0)) {
    }

    template <typename type>
    WNRay2<type>::WNRay2(const type* _numbers) :
        mLocation(&_numbers[0]),
        mDirection(&_numbers[2]) {
    }

    template <typename type>
    WNRay2<type>::WNRay2(const WNVector2<type>& _location, const WNVector2<type>& _direction) :
        mLocation(_location),
        mDirection(_direction) {
    }

    template <typename type>
    WNRay2<type> WNRay2<type>::operator + () const {
        return(*this);
    }

    template <typename type>
    WNRay2<type> WNRay2<type>::operator - () const {
        return(WNRay2<type>(-mLocation, -mDirection));
    }

    template <typename type>
    wn_bool WNRay2<type>::operator == (const WNRay2<type>& _ray) const {
        return(mLocation == _ray.mLocation && mDirection == _ray.mDirection);
    }

    template <typename type>
    wn_bool WNRay2<type>::operator != (const WNRay2<type>& _ray) const {
        return(mLocation != _ray.mLocation || mDirection != _ray.mDirection);
    }

    template <typename type>
    wn_void WNRay2<type>::Zero() {
        mLocation.Zero();
        mDirection.Zero();
    }

    template <typename type>
    wn_void WNRay2<type>::Set(const type* _numbers) {
        mLocation.Set(&_numbers[0]);
        mDirection.Set(&_numbers[2]);
    }

    template <typename type>
    wn_void WNRay2<type>::Set(const WNVector2<type>& _location, const WNVector2<type>& _direction) {
        mLocation = _location;
        mDirection = _direction;
    }

    template <typename type>
    WNLine2<type> WNRay2<type>::ToLine2() const {
        return(WNLine2<type>(mLocation, mLocation + mDirection));
    }

    template <typename type>
    WNLine3<type> WNRay2<type>::ToLine3() const {
        return(WNLine3<type>(mLocation.ToPoint3(), (mLocation + mDirection).ToPoint3()));
    }
}

#endif // __WN_MATH_RAY_2_INL__