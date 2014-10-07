////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_RAY_3_INL__
#define __WN_MATH_RAY_3_INL__

#ifndef __WN_MATH_RAY_3_H__
    #error "WNRay3.inl should never be included directly. Please include WNRay2.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"

namespace WNMath {
    template <typename Type>
    WNRay3<Type>::WNRay3() :
        mLocation(static_cast<Type>(0)),
        mDirection(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNRay3<Type>::WNRay3(const Type* _numbers) :
        mLocation(&_numbers[0]),
        mDirection(&_numbers[3]) {
    }

    template <typename Type>
    WNRay3<Type>::WNRay3(const WNVector3<Type>& _location, const WNVector3<Type>& _direction) :
        mLocation(_location),
        mDirection(_direction) {
    }

    template <typename Type>
    WNRay3<Type> WNRay3<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNRay3<Type> WNRay3<Type>::operator - () const {
        return(WNRay3<Type>(-mLocation, -mDirection));
    }

    template <typename Type>
    WN_BOOL WNRay3<Type>::operator == (const WNRay3<Type>& _ray) const {
        return(mLocation == _ray.mLocation && mDirection == _ray.mDirection);
    }

    template <typename Type>
    WN_BOOL WNRay3<Type>::operator != (const WNRay3<Type>& _ray) const {
        return(mLocation != _ray.mLocation || mDirection != _ray.mDirection);
    }

    template <typename Type>
    WN_VOID WNRay3<Type>::Zero() {
        mLocation.Zero();
        mDirection.Zero();
    }
    template <typename Type>
    WN_VOID WNRay3<Type>::Set(const Type* _numbers) {
        mLocation.Set(&_numbers[0]);
        mDirection.Set(&_numbers[3]);
    }

    template <typename Type>
    WN_VOID WNRay3<Type>::Set(const WNVector3<Type>& _location, const WNVector3<Type>& _direction) {
        mLocation = _location;
        mDirection = _direction;
    }

    template <typename Type>
    WNLine2<Type> WNRay3<Type>::ToLine2() const {
        return(WNLine2<Type>(mLocation.ToPoint2(), (mLocation + mDirection).ToPoint2()));
    }

    template <typename Type>
    WNLine3<Type> WNRay3<Type>::ToLine3() const {
        return(WNLine3<Type>(mLocation, mLocation + mDirection));
    }
}

#endif // __WN_MATH_RAY_3_INL__