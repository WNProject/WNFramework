////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_BOUNDS_2_INL__
#define __WN_MATH_BOUNDS_2_INL__

#ifndef __WN_MATH_BOUNDS_2_H__
    #error "WNBounds2.inl should never be included directly. Please include WNBounds2.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCircle.h"
#include "WNMath/inc/WNRectangle.h"
#include "WNMath/inc/WNVector2.h"

namespace WNMath {
    template <typename Type>
    WNBounds2<Type>::WNBounds2() :
        mMinimum(static_cast<Type>(0), static_cast<Type>(0)),
        mMaximum(static_cast<Type>(0), static_cast<Type>(0)) {
    }

    template <typename Type>
    WNBounds2<Type>::WNBounds2(const Type* _numbers) :
        mMinimum(_numbers[0], _numbers[1]),
        mMaximum(_numbers[2], _numbers[3]) {
    }

    template <typename Type>
    WNBounds2<Type>::WNBounds2(const WNVector2<Type>& _vector) :
        mMinimum(_vector),
        mMaximum(_vector) {
    }

    template <typename Type>
    WNBounds2<Type>::WNBounds2(const WNVector2<Type>& _min, const WNVector2<Type>& _max) :
        mMinimum(_min),
        mMaximum(_max) {
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::operator == (const WNBounds2<Type>& _bounds) const {
        return(mMinimum == _bounds.mMinimum && mMaximum == _bounds.mMaximum);
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::operator != (const WNBounds2<Type>& _bounds) const {
        return(mMinimum != _bounds.mMinimum || mMaximum != _bounds.mMaximum);
    }

    template <typename Type>
    WN_VOID WNBounds2<Type>::Zero() {
        mMinimum.Zero();
        mMaximum.Zero();
    }

    template <typename Type>
    Type WNBounds2<Type>::Length() const {
        const WNVector2<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX));
    }

    template <typename Type>
    Type WNBounds2<Type>::Width() const {
        const WNVector2<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mY));
    }

    template <typename Type>
    Type WNBounds2<Type>::Perimeter() const {
        const WNVector2<Type> size = mMaximum - mMinimum;

        return(WNAbs(static_cast<Type>(2) * size.mX) + WNAbs(static_cast<Type>(2) * size.mY));
    }

    template <typename Type>
    Type WNBounds2<Type>::Radius() const {
        Type total = static_cast<Type>(0);

        for (WN_UINT8 i = 0; i < 2; i++) {
            const Type minComponent = WNAbs(mMinimum[i]);
            const Type maxComponent = WNAbs(mMaximum[i]);

            if (minComponent > maxComponent) {
                total += minComponent * minComponent;
            } else {
                total += maxComponent * maxComponent;
            }
        }

        return(WNSqrt(total));
    }

    template <typename Type>
    Type WNBounds2<Type>::Area() const {
        const WNVector2<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX * size.mY));
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::IsZero() const {
        return(mMinimum.IsZero() && mMaximum.IsZero());
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::IsInsideOut() const {
        return(mMinimum.mX > mMaximum.mX || mMinimum.mY > mMaximum.mY);
    }

    template <typename Type>
    WN_VOID WNBounds2<Type>::Expand(Type _amount) {
        const WNVector2<Type> expand = WNVector2<Type>(_amount);

        mMaximum += expand;
        mMinimum -= expand;
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::Expand(const WNVector2<Type>& _vector) {
        WN_BOOL expanded = WN_FALSE;

        if (_vector.mX < mMinimum.mX) {
            mMinimum.mX = _vector.mX;

            expanded = WN_TRUE;
        }

        if (_vector.mX > mMaximum.mX) {
            mMaximum.mX = _vector.mX;

            expanded = WN_TRUE;
        }

        if (_vector.mY < mMinimum.mY) {
            mMinimum.mY = _vector.mY;

            expanded = WN_TRUE;
        }

        if (_vector.mY > mMaximum.mY) {
            mMaximum.mY = _vector.mY;

            expanded = WN_TRUE;
        }

        return(expanded);
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::Expand(const WNRectangle<Type>& _rectangle) {
        const WNVector2<Type> extents = _rectangle.mExtents.GetRotated(_rectangle.mRotation);
        const WNVector2<Type> point1 = _rectangle.mLocation + extents;
        const WNVector2<Type> point2 = _rectangle.mLocation + WNVector2<Type>(-extents.mX, extents.mY);
        const WNVector2<Type> point3 = _rectangle.mLocation + WNVector2<Type>(extents.mX, -extents.mY);
        const WNVector2<Type> point4 = _rectangle.mLocation - extents;

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4));
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::Expand(const WNCircle<Type>& _circle) {
        const WNVector2<Type> point1 = WNVector2<Type>(_circle.mLocation.mX + _circle.mRadius, _circle.mLocation.mY);
        const WNVector2<Type> point2 = WNVector2<Type>(_circle.mLocation.mX - _circle.mRadius, _circle.mLocation.mY);
        const WNVector2<Type> point3 = WNVector2<Type>(_circle.mLocation.mX, _circle.mLocation.mY + _circle.mRadius);
        const WNVector2<Type> point4 = WNVector2<Type>(_circle.mLocation.mX, _circle.mLocation.mY - _circle.mRadius);

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4));
    }

    template <typename Type>
    WN_BOOL WNBounds2<Type>::Expand(const WNBounds2<Type>& _bounds) {
        WN_BOOL expanded = WN_FALSE;

        if (_bounds.mMinimum.mX < mMinimum.mX) {
            mMinimum.mX = _bounds.mMinimum.mX;

            expanded = WN_TRUE;
        }

        if (_bounds.mMinimum.mY < mMinimum.mY) {
            mMinimum.mY = _bounds.mMinimum.mY;

            expanded = WN_TRUE;
        }

        if (_bounds.mMaximum.mX > mMaximum.mX) {
            mMaximum.mX = _bounds.mMaximum.mX;

            expanded = WN_TRUE;
        }

        if (_bounds.mMaximum.mY > mMaximum.mY) {
            mMaximum.mY = _bounds.mMaximum.mY;

            expanded = WN_TRUE;
        }

        return(expanded);
    }

    template <typename Type>
    WN_VOID WNBounds2<Type>::Translate(const WNVector2<Type>& _vector) {
        mMinimum += _vector;
        mMaximum += _vector;
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetExpanded(Type _amount) const {
        WNBounds2 bounds = *this;

        return(bounds.Expand(_amount), bounds);
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetExpanded(const WNVector2<Type>& _vector) const {
        WNBounds2 bounds = *this;

        return(bounds.Expand(_vector), bounds);
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetExpanded(const WNRectangle<Type>& _rectangle) const {
        WNBounds2<Type> bounds = *this;

        return(bounds.Expand(_rectangle), bounds);
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetExpanded(const WNCircle<Type>& _circle) const {
        WNBounds2<Type> bounds = *this;

        return(bounds.Expand(_circle), bounds);
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetExpanded(const WNBounds2<Type>& _bounds) const {
        WNBounds2<Type> bounds = *this;

        return(bounds.Expand(_bounds), bounds);
    }

    template <typename Type>
    WNBounds2<Type> WNBounds2<Type>::GetTranslated(const WNVector2<Type>& _vector) const {
        WNBounds2<Type> bounds = *this;

        return(bounds.Translate(_vector), bounds);
    }

    template <typename Type>
    WN_VOID WNBounds2<Type>::GetPoints(WNVector2<Type>* _vectors) const {
        _vectors[0] = mMaximum;
        _vectors[1] = (mMinimum * WNVector2<Type>(static_cast<Type>(0), static_cast<Type>(1)) +
                      mMaximum * WNVector2<Type>(static_cast<Type>(1), static_cast<Type>(0))).ToPoint2();
        _vectors[2] = (mMinimum * WNVector2<Type>(static_cast<Type>(1), static_cast<Type>(0)) +
                      mMaximum * WNVector2<Type>(static_cast<Type>(0), static_cast<Type>(1))).ToPoint2();
        _vectors[3] = mMinimum;
    }

    template <typename Type>
    WNRectangle<Type> WNBounds2<Type>::ToRectangle() const {
        const WNVector2<Type> location = WNMidPoint(mMinimum, mMaximum);
        const WNVector2<Type> extents = mMaximum - location;

        return(WNRectangle<Type>(location, extents, static_cast<Type>(0)));
    }
}

#endif // __WN_MATH_BOUNDS_2_INL__