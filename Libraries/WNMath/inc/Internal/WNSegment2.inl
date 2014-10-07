////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

namespace WNMath {
    template <typename Type>
    WNSegment2<Type>::WNSegment2() :
        mStart(static_cast<Type>(0), static_cast<Type>(0)),
        mEnd(static_cast<Type>(0), static_cast<Type>(0)) {
    }

    template <typename Type>
    WNSegment2<Type>::WNSegment2(const Type* _numbers) :
        mStart(_numbers[0], _numbers[1]),
        mEnd(_numbers[2], _numbers[3]) {
    }

    template <typename Type>
    WNSegment2<Type>::WNSegment2(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2) :
        mStart(_point1),
        mEnd(_point2) {
    }

    template <typename Type>
    WNSegment2<Type> WNSegment2<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNSegment2<Type> WNSegment2<Type>::operator - () const {
        return(WNSegment2<Type>(-mStart, -mEnd));
    }

    template <typename Type>
    WN_BOOL WNSegment2<Type>::operator == (const WNSegment2<Type>& _segment) const {
        return(mStart == _segment.start && mEnd == _segment.end);
    }

    template <typename Type>
    WN_BOOL WNSegment2<Type>::operator != (const WNSegment2<Type>& _segment) const {
        return(mStart != _segment.start || mEnd != _segment.end);
    }

    template <typename Type>
    WN_VOID WNSegment2<Type>::Zero() {
        mStart.Zero();
        mEnd.Zero();
    }

    template <typename Type>
    WN_VOID WNSegment2<Type>::Set(const Type* _numbers) {
        mStart.mX = _numbers[0];
        mStart.mY = _numbers[1];

        mEnd.mX = _numbers[2];
        mEnd.mY = _numbers[3];
    }

    template <typename Type>
    WN_VOID WNSegment2<Type>::Set(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2) {
        mStart = _point1;
        mEnd = _point2;
    }

    template <typename Type>
    Type WNSegment2<Type>::Length() const {
        return(WNDistance(mStart, mEnd));
    }

    template <typename Type>
    Type WNSegment2<Type>::LengthSquared() const {
        return(WNDistanceSquared(mStart, mEnd));
    }

    template <typename Type>
    WNVector2<Type> WNSegment2<Type>::Direction() const {
        return(mEnd - mStart);
    }

    template <typename Type>
    WNLine2<Type> WNSegment2<Type>::ToLine2() const {
        return(WNLine2<Type>(mStart, mEnd));
    }

    template <typename Type>
    WNLine3<Type> WNSegment2<Type>::ToLine3() const {
        return(WNLine3<Type>(mStart.ToPoint3(), mEnd.ToPoint3()));
    }

    template <typename Type>
    WNRay2<Type> WNSegment2<Type>::ToRay2() const {
        return(WNRay2<Type>(mStart, mEnd - mStart));
    }

    template <typename Type>
    WNRay3<Type> WNSegment2<Type>::ToRay3() const {
        return(WNRay3<Type>(mStart.ToPoint3(), (mEnd - mStart).ToVector3()));
    }
}

#endif // __WN_MATH_SEGMENT_2_INL__