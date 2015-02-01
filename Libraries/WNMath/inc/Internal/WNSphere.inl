// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_SPHERE_INL__
#define __WN_MATH_SPHERE_INL__

#ifndef __WN_MATH_SPHERE_H__
    #error "WNSphere.inl should never be included directly. Please include WNSphere.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBounds3.h"
#include "WNMath/inc/WNBox.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNSphere<Type>::WNSphere() :
        mLocation(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mRadius(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNSphere<Type>::WNSphere(const Type* _numbers) :
        mLocation(_numbers[0], _numbers[1], _numbers[2]),
        mRadius(_numbers[3]) {
    }

    template <typename Type>
    WNSphere<Type>::WNSphere(const WNVector3<Type>& _location, Type _radius) :
        mLocation(_location),
        mRadius(_radius) {
    }

    template <typename Type>
    WNSphere<Type>::WNSphere(Type _x, Type _y, Type _z, Type _radius) :
        mLocation(_x, _y, _z),
        mRadius(_radius) {
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::operator == (const WNSphere<Type>& _sphere) const {
        return(mLocation == _sphere.mLocation && mRadius == _sphere.mRadius);
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::operator != (const WNSphere<Type>& _sphere) const {
        return(mLocation != _sphere.mLocation || mRadius != _sphere.mRadius);
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Zero() {
        mLocation.Zero();

        mRadius = static_cast<Type>(0);
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Set(const Type* _numbers) {
        mLocation.mX = _numbers[0];
        mLocation.mY = _numbers[1];
        mLocation.mZ = _numbers[2];
        mRadius = _numbers[3];
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Set(const WNVector3<Type>& _location, Type _radius) {
        mLocation = _location;
        mRadius = _radius;
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Set(Type _x, Type _y, Type _z, Type _radius) {
        mLocation.mX = _x;
        mLocation.mY = _y;
        mLocation.mZ = _z;
        mRadius = _radius;
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::IsZero() const {
        return(mLocation.IsZero() && mRadius == static_cast<Type>(0));
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::IsInsideOut() const {
        return(mRadius < static_cast<Type>(0));
    }

    template <typename Type>
    Type WNSphere<Type>::Volume() const {
        return((static_cast<Type>(4) / static_cast<Type>(3)) * static_cast<Type>(WN_PI) * mRadius * mRadius * mRadius);
    }

    template <typename Type>
    Type WNSphere<Type>::SurfaceArea() const {
        return(static_cast<Type>(4) * static_cast<Type>(WN_PI) * mRadius * mRadius);
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Expand(Type _amount) {
        mRadius += _amount;
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor) {
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
    WN_BOOL WNSphere<Type>::Expand(const WNBox<Type>& _box, WN_BOOL _anchor) {
        WNVector3<Type> points[8];

        _box.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::Expand(const WNBounds3<Type>& _bounds, WN_BOOL _anchor) {
        WNVector3<Type> points[8];

        _bounds.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename Type>
    WN_BOOL WNSphere<Type>::Expand(const WNSphere<Type>& _sphere, WN_BOOL _anchor) {
        if (mRadius < static_cast<Type>(0)) {
            if (!_anchor) {
                mLocation = _sphere.mLocation;
                mRadius = _sphere.mRadius;
            } else {
                mRadius = WNDistance(mLocation, _sphere.mLocation) + _sphere.mRadius;
            }

            return(WN_TRUE);
        } else {
            const Type distanceSquared = WNDistanceSquared(mLocation, _sphere.mLocation);

            if (distanceSquared > (mRadius + _sphere.mRadius) * (mRadius + _sphere.mRadius)) {
                const Type distance = WNSqrt(distanceSquared);

                if (!_anchor) {
                    mLocation += (_sphere.mLocation - mLocation) / static_cast<Type>(2) *
                                 (static_cast<Type>(1) - mRadius / (distance + _sphere.mRadius));
                    mRadius += ((distance + _sphere.mRadius) - mRadius) / static_cast<Type>(2);
                } else {
                    mRadius = distance;
                }

                return(WN_TRUE);
            }
        }

        return(WN_FALSE);
    }

    template <typename Type>
    WN_VOID WNSphere<Type>::Translate(const WNVector3<Type>& _vector) {
        mLocation += _vector;
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetExpanded(Type _amount) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Expand(_amount), sphere);
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Expand(_vector, _anchor), sphere);
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetExpanded(const WNBox<Type>& _box, WN_BOOL _anchor) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Expand(_box, _anchor), sphere);
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetExpanded(const WNBounds3<Type>& _bounds, WN_BOOL _anchor) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Expand(_bounds, _anchor), sphere);
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetExpanded(const WNSphere<Type>& _sphere, WN_BOOL _anchor) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Expand(_sphere, _anchor), sphere);
    }

    template <typename Type>
    WNSphere<Type> WNSphere<Type>::GetTranslated(const WNVector3<Type>& _vector) const {
        WNSphere<Type> sphere = *this;

        return(sphere.Translate(_vector), sphere);
    }
}

#endif // __WN_MATH_SPHERE_INL__