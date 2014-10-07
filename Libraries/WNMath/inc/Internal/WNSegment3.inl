////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_SEGMENT_3_INL__
#define __WN_MATH_SEGMENT_3_INL__

#ifndef __WN_MATH_SEGMENT_3_H__
    #error "WNSegment3.inl should never be included directly. Please include WNSegment3.h instead"
#endif

#include "WNMath/inc/WNLine2.h"
#include "WNMath/inc/WNLine3.h"
#include "WNMath/inc/WNRay2.h"
#include "WNMath/inc/WNRay3.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNSegment3<Type>::WNSegment3() :
        mStart(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mEnd(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)) {
    }

    template <typename Type>
    WNSegment3<Type>::WNSegment3(const Type* _numbers) :
        mStart(_numbers[0], _numbers[1], _numbers[2]),
        mEnd(_numbers[3], _numbers[4], _numbers[5]) {
    }

    template <typename Type>
    WNSegment3<Type>::WNSegment3(const WNVector3<Type>& _start, const WNVector3<Type>& _end) :
        mStart(_start),
        mEnd(_end) {
    }

    template <typename Type>
    WNSegment3<Type> WNSegment3<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNSegment3<Type> WNSegment3<Type>::operator - () const {
        return(WNSegment3<Type>(-mStart, -mEnd));
    }

    template <typename Type>
    WN_BOOL WNSegment3<Type>::operator == (const WNSegment3<Type>& _segment) const {
        return(mStart == _segment.start && mEnd == _segment.end);
    }

    template <typename Type>
    WN_BOOL WNSegment3<Type>::operator != (const WNSegment3<Type>& _segment) const {
        return(mStart != _segment.start || mEnd != _segment.end);
    }

    template <typename Type>
    WN_VOID WNSegment3<Type>::Zero() {
        mStart.Zero();
        mEnd.Zero();
    }

    template <typename Type>
    WN_VOID WNSegment3<Type>::Set(const Type* _numbers) {
        mStart.mX = _numbers[0];
        mStart.mY = _numbers[1];
        mStart.mZ = _numbers[2];

        mEnd.mX = _numbers[3];
        mEnd.mY = _numbers[4];
        mEnd.mZ = _numbers[5];
    }

    template <typename Type>
    WN_VOID WNSegment3<Type>::Set(const WNVector3<Type>& _start, const WNVector3<Type>& _end) {
        mStart = _start;
        mEnd = _end;
    }

    template <typename Type>
    Type WNSegment3<Type>::Length() const {
        return(WNDistance(mStart, mEnd));
    }

    template <typename Type>
    Type WNSegment3<Type>::LengthSquared() const {
        return(WNDistanceSquared(mStart, mEnd));
    }

    template <typename Type>
    WNVector3<Type> WNSegment3<Type>::Direction() const {
        return(mEnd - mStart);
    }

    template <typename Type>
    WNLine2<Type> WNSegment3<Type>::ToLine2() const {
        return(WNLine2<Type>(mStart.ToPoint2(), mEnd.ToPoint2()));
    }

    template <typename Type>
    WNLine3<Type> WNSegment3<Type>::ToLine3() const {
        return(WNLine3<Type>(mStart, mEnd));
    }

    template <typename Type>
    WNRay2<Type> WNSegment3<Type>::ToRay2() const {
        return(WNRay2<Type>(mStart.ToPoint2(), (mEnd - mStart).ToVector2()));
    }

    template <typename Type>
    WNRay3<Type> WNSegment3<Type>::ToRay3() const {
        return(WNRay3<Type>(mStart, mEnd - mStart));
    }
}

#endif // __WN_MATH_SEGMENT_3_INL__