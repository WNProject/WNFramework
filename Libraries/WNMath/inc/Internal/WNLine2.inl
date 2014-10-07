////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_LINE_2_INL__
#define __WN_MATH_LINE_2_INL__

#ifndef __WN_MATH_LINE_2_H__
    #error "WNLine2.inl should never be included directly. Please include WNLine2.h instead"
#endif

#include "WNMath/inc/WNVector2.h"

namespace WNMath {
    template <typename Type>
    WNLine2<Type>::WNLine2() :
        mPoint1(static_cast<Type>(0)),
        mPoint2(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNLine2<Type>::WNLine2(const WNLine2<Type>& _line) {
        Set(_line);
    }

    template <typename Type>
    WNLine2<Type>::WNLine2(const Type* _numbers) :
        mPoint1(&_numbers[0]),
        mPoint2(&_numbers[2]) {
    }

    template <typename Type>
    WNLine2<Type>::WNLine2(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2) :
        mPoint1(_point1),
        mPoint2(_point2) {
    }

    template <typename Type>
    WNLine2<Type> WNLine2<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNLine2<Type> WNLine2<Type>::operator - () const {
        return(WNLine2<Type>(-mPoint1, -mPoint2));
    }

    template <typename Type>
    WN_BOOL WNLine2<Type>::operator == (const WNLine2& _line) const {
        return(mPoint1 == _line.mPoint1 && mPoint2 == _line.mPoint2);
    }

    template <typename Type>
    WN_BOOL WNLine2<Type>::operator != (const WNLine2& _line) const {
        return(mPoint1 != _line.mPoint1 || mPoint2 != _line.mPoint2);
    }

    template <typename Type>
    WN_VOID WNLine2<Type>::Zero() {
        mPoint1.Zero();
        mPoint2.Zero();
    }

    template <typename Type>
    WN_VOID WNLine2<Type>::Set(const WNLine2<Type>& _line) {
        *this = _line;
    }

    template <typename Type>
    WN_VOID WNLine2<Type>::Set(const Type* _numbers) {
        mPoint1.mX = _numbers[0];
        mPoint1.mY = _numbers[1];

        mPoint2.mX = _numbers[2];
        mPoint2.mY = _numbers[3];
    }

    template <typename Type>
    WN_VOID WNLine2<Type>::Set(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2) {
        mPoint1 = _point1;
        mPoint2 = _point2;
    }

    template <typename Type>
    WNVector2<Type> WNLine2<Type>::Direction() const {
        return(mPoint2 - mPoint1);
    }
}

#endif // __WN_MATH_LINE_2_INL__