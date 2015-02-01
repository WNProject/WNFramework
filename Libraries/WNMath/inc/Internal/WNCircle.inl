// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_CIRCLE_INL__
#define __WN_MATH_CIRCLE_INL__

#ifndef __WN_MATH_CIRCLE_H__
    #error "WNCircle.inl should never be included directly. Please include WNCircle.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBounds2.h"
#include "WNMath/inc/WNRectangle.h"
#include "WNMath/inc/WNVector2.h"

namespace WNMath {
    template <typename Type>
    WNCircle<Type>::WNCircle() :
        mLocation(static_cast<Type>(0), static_cast<Type>(0)),
        mRadius(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNCircle<Type>::WNCircle(const Type* _numbers) :
        mLocation(_numbers[0], _numbers[1]),
        mRadius(_numbers[2]) {
    }

    template <typename Type>
    WNCircle<Type>::WNCircle(const WNVector2<Type>& _location, Type _radius) :
        mLocation(_location),
        mRadius(_radius) {
    }

    template <typename Type>
    WNCircle<Type>::WNCircle(Type _x, Type _y, Type _radius) :
        mLocation(_x, _y),
        mRadius(_radius) {
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::operator == (const WNCircle<Type>& _circle) const {
        return(mLocation == _circle.mLocation && mRadius == _circle.mRadius);
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::operator != (const WNCircle<Type>& _circle) const {
        return(mLocation != _circle.mLocation || mRadius != _circle.mRadius);
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Zero() {
        mLocation.Zero();

        mRadius = static_cast<Type>(0);
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Set(const Type* _numbers) {
        mLocation.mX = _numbers[0];
        mLocation.mY = _numbers[1];
        mRadius = _numbers[2];
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Set(const WNVector2<Type>& _location, Type _radius) {
        mLocation = _location;
        mRadius = _radius;
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Set(Type _x, Type _y, Type _radius) {
        mLocation.mX = _x;
        mLocation.mY = _y;
        mRadius = _radius;
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::IsZero() const {
        return(mLocation.IsZero() && mRadius == static_cast<Type>(0));
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::IsInsideOut() const {
        return(mRadius < static_cast<Type>(0));
    }

    template <typename Type>
    Type WNCircle<Type>::Circumference() const {
        return(static_cast<Type>(WN_PI) * static_cast<Type>(2) * mRadius);
    }

    template <typename Type>
    Type WNCircle<Type>::Area() const {
        return(static_cast<Type>(WN_PI) * mRadius * mRadius);
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Expand(Type _amount) {
        mRadius += _amount;
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::Expand(const WNVector2<Type>& _vector, WN_BOOL _anchor) {
        if (mRadius < static_cast<Type>(0)) {
            if (!_anchor) {
                mLocation = _vector;
                mRadius = static_cast<Type>(0);
            } else {
                mRadius = WNDistance(mLocation, _vector);
            }

            return(WN_TRUE);
        } else {
            const Type distanceSquared = WNDistanceSquared(mLocation, _vector);

            if (distanceSquared > mRadius * mRadius) {
                const Type distance = WNSqrt(distanceSquared);

                if (!_anchor) {
                    mLocation += (_vector - mLocation) / static_cast<Type>(2) * (static_cast<Type>(1) - mRadius / distance);
                    mRadius += (distance - mRadius) / static_cast<Type>(2);
                } else {
                    mRadius = distance;
                }

                return(WN_TRUE);
            }
        }

        return(WN_FALSE);
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::Expand(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor) {
        WNVector2<Type> points[4];

        _rectangle.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor));
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::Expand(const WNBounds2<Type>& _bounds, WN_BOOL _anchor) {
        WNVector2<Type> points[4];

        _bounds.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor));
    }

    template <typename Type>
    WN_BOOL WNCircle<Type>::Expand(const WNCircle<Type>& _circle, WN_BOOL _anchor) {
        if (mRadius < 0.0f) {
            if (!_anchor) {
                mLocation = _circle.mLocation;
                mRadius = _circle.mRadius;
            } else {
                mRadius = WNDistance(mLocation, _circle.mLocation) + _circle.mRadius;
            }

            return(WN_TRUE);
        } else {
            const Type distanceSquared = WNDistanceSquared(mLocation, _circle.mLocation);

            if (distanceSquared > (mRadius + _circle.mRadius) * (mRadius + _circle.mRadius)) {
                const Type distance = WNSqrt(distanceSquared);

                if (!_anchor) {
                    mLocation += (_circle.mLocation - mLocation) / static_cast<Type>(2) *
                                 (static_cast<Type>(1) - mRadius / (distance + _circle.mRadius));
                    mRadius += ((distance + _circle.mRadius) - mRadius) / static_cast<Type>(2);
                } else {
                    mRadius = distance;
                }

                return(WN_TRUE);
            }
        }

        return(WN_FALSE);
    }

    template <typename Type>
    WN_VOID WNCircle<Type>::Translate(const WNVector2<Type>& _vector) {
        mLocation += _vector;
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetExpanded(Type _amount) const {
        WNCircle<Type> circle = *this;

        return(circle.Expand(_amount), circle);
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetExpanded(const WNVector2<Type>& _vector, WN_BOOL _anchor) const {
        WNCircle<Type> circle = *this;

        return(circle.Expand(_vector, _anchor), circle);
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetExpanded(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor) const {
        WNCircle<Type> circle = *this;

        return(circle.Expand(_rectangle, _anchor), circle);
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetExpanded(const WNBounds2<Type>& _bounds, WN_BOOL _anchor) const {
        WNCircle<Type> circle = *this;

        return(circle.Expand(_bounds, _anchor), circle);
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetExpanded(const WNCircle<Type>& _circle, WN_BOOL _anchor) const {
        WNCircle<Type> circle = *this;

        return(circle.Expand(_circle, _anchor), circle);
    }

    template <typename Type>
    WNCircle<Type> WNCircle<Type>::GetTranslated(const WNVector2<Type>& _vector) const {
        WNCircle<Type> circle = *this;

        return(circle.Translate(_vector), circle);
    }
}

#endif // __WN_MATH_CIRCLE_INL__