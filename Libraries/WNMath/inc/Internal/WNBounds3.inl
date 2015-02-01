// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_BOUNDS_3_INL__
#define __WN_MATH_BOUNDS_3_INL__

#ifndef __WN_MATH_BOUNDS_3_H__
    #error "WNBounds3.inl should never be included directly. Please include WNBounds3.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBox.h"
#include "WNMath/inc/WNSphere.h"
#include "WNMath/inc/WNVector.h"

namespace wn {
    template <typename type>
    WNBounds3<type>::WNBounds3() :
        mMinimum(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)),
        mMaximum(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)) {
    }

    template <typename type>
    WNBounds3<type>::WNBounds3(const type* _numbers) :
        mMinimum(_numbers[0], _numbers[1], _numbers[2]),
        mMaximum(_numbers[3], _numbers[4], _numbers[5]) {
    }

    template <typename type>
    WNBounds3<type>::WNBounds3(const WNVector<type>& _vector) :
        mMinimum(_vector),
        mMaximum(_vector) {
    }

    template <typename type>
    WNBounds3<type>::WNBounds3(const WNVector<type>& _min, const WNVector<type>& _max) :
        mMinimum(_min),
        mMaximum(_max) {
    }

    template <typename type>
    wn_bool WNBounds3<type>::operator == (const WNBounds3<type>& _bounds) const {
        return(mMinimum == _bounds.mMinimum && mMaximum == _bounds.mMaximum);
    }

    template <typename type>
    wn_bool WNBounds3<type>::operator != (const WNBounds3<type>& _bounds) const {
        return(mMinimum != _bounds.mMinimum || mMaximum != _bounds.mMaximum);
    }

    template <typename type>
    wn_void WNBounds3<type>::Zero() {
        mMinimum.Zero();
        mMaximum.Zero();
    }

    template <typename type>
    type WNBounds3<type>::Length() const {
        const WNVector<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX));
    }

    template <typename type>
    type WNBounds3<type>::Width() const {
        const WNVector<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mZ));
    }

    template <typename type>
    type WNBounds3<type>::Height() const {
        const WNVector<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mY));
    }

    template <typename type>
    type WNBounds3<type>::Radius() const {
        type total = static_cast<type>(0);

        for (wn_uint8 i = 0; i < 3; i++) {
            const type minComponent = WNAbs(mMinimum[i]);
            const type maxComponent = WNAbs(mMaximum[i]);

            if (minComponent > maxComponent) {
                total += minComponent * minComponent;
            } else {
                total += maxComponent * maxComponent;
            }
        }

        return(wn::sqrt(total));
    }

    template <typename type>
    type WNBounds3<type>::SurfaceArea() const {
        const WNVector<type> size = mMaximum - mMinimum;
        const type xy = WNAbs(size.mX * size.mY);
        const type xz = WNAbs(size.mX * size.mZ);
        const type yz = WNAbs(size.mY * size.mZ);

        return(xy + xz + yz);
    }

    template <typename type>
    type WNBounds3<type>::Volume() const {
        const WNVector<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX * size.mY * size.mZ));
    }

    template <typename type>
    wn_bool WNBounds3<type>::IsZero() const {
        return(mMinimum.IsZero() && mMaximum.IsZero());
    }

    template <typename type>
    wn_bool WNBounds3<type>::IsInsideOut() const {
        return(mMinimum.mX > mMaximum.mX || mMinimum.mY > mMaximum.mY || mMinimum.mZ > mMaximum.mZ);
    }

    template <typename type>
    wn_void WNBounds3<type>::Expand(type _amount) {
        const WNVector<type> expand = WNVector<type>(_amount);

        mMaximum += expand;
        mMinimum -= expand;
    }

    template <typename type>
    wn_bool WNBounds3<type>::Expand(const WNVector<type>& _vector) {
        wn_bool expanded = wn_false;

        if (_vector.mX < mMinimum.mX) {
            mMinimum.mX = _vector.mX;

            expanded = wn_true;
        }

        if (_vector.mX > mMaximum.mX) {
            mMaximum.mX = _vector.mX;

            expanded = wn_true;
        }

        if (_vector.mY < mMinimum.mY) {
            mMinimum.mY = _vector.mY;

            expanded = wn_true;
        }

        if (_vector.mY > mMaximum.mY) {
            mMaximum.mY = _vector.mY;

            expanded = wn_true;
        }

        if (_vector.mZ < mMinimum.mZ) {
            mMinimum.mZ = _vector.mZ;

            expanded = wn_true;
        }

        if (_vector.mZ > mMaximum.mZ) {
            mMaximum.mZ = _vector.mZ;

            expanded = wn_true;
        }

        return(expanded);
    }

    template <typename type>
    wn_bool WNBounds3<type>::Expand(const WNBox<type>& _box) {
        const WNVector<type> extents = _box.mExtents.GetRotated(_box.mRotation);
        const WNVector<type> point1 = _box.mLocation + extents;
        const WNVector<type> point2 = _box.mLocation + WNVector<type>(-extents.mX, extents.mY, extents.mZ);
        const WNVector<type> point3 = _box.mLocation + WNVector<type>(extents.mX, -extents.mY, extents.mZ);
        const WNVector<type> point4 = _box.mLocation + WNVector<type>(extents.mX, extents.mY, -extents.mZ);
        const WNVector<type> point5 = _box.mLocation + WNVector<type>(-extents.mX, -extents.mY, extents.mZ);
        const WNVector<type> point6 = _box.mLocation + WNVector<type>(extents.mX, -extents.mY, -extents.mZ);
        const WNVector<type> point7 = _box.mLocation + WNVector<type>(-extents.mX, extents.mY, -extents.mZ);
        const WNVector<type> point8 = _box.mLocation - extents;

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4) ||
               Expand(point5) || Expand(point6) || Expand(point7) || Expand(point8));
    }

    template <typename type>
    wn_bool WNBounds3<type>::Expand(const WNSphere<type>& _sphere) {
        const WNVector<type> point1 = WNVector<type>(_sphere.mLocation.mX + _sphere.mRadius, _sphere.mLocation.mY, _sphere.mLocation.mZ);
        const WNVector<type> point2 = WNVector<type>(_sphere.mLocation.mX - _sphere.mRadius, _sphere.mLocation.mY, _sphere.mLocation.mZ);
        const WNVector<type> point3 = WNVector<type>(_sphere.mLocation.mX, _sphere.mLocation.mY + _sphere.mRadius, _sphere.mLocation.mZ);
        const WNVector<type> point4 = WNVector<type>(_sphere.mLocation.mX, _sphere.mLocation.mY - _sphere.mRadius, _sphere.mLocation.mZ);
        const WNVector<type> point5 = WNVector<type>(_sphere.mLocation.mX, _sphere.mLocation.mY, _sphere.mLocation.mZ + _sphere.mRadius);
        const WNVector<type> point6 = WNVector<type>(_sphere.mLocation.mX, _sphere.mLocation.mY, _sphere.mLocation.mZ - _sphere.mRadius);

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4) || Expand(point5) || Expand(point6));
    }

    template <typename type>
    wn_bool WNBounds3<type>::Expand(const WNBounds3<type>& _bounds) {
        wn_bool expanded = wn_false;

        if (_bounds.mMinimum.mX < mMinimum.mX) {
            mMinimum.mX = _bounds.mMinimum.mX;

            expanded = wn_true;
        }

        if (_bounds.mMinimum.mY < mMinimum.mY) {
            mMinimum.mY = _bounds.mMinimum.mY;

            expanded = wn_true;
        }

        if (_bounds.mMinimum.mZ < mMinimum.mZ) {
            mMinimum.mZ = _bounds.mMinimum.mZ;

            expanded = wn_true;
        }

        if (_bounds.mMaximum.mX > mMaximum.mX) {
            mMaximum.mX = _bounds.mMaximum.mX;

            expanded = wn_true;
        }

        if (_bounds.mMaximum.mY > mMaximum.mY) {
            mMaximum.mY = _bounds.mMaximum.mY;

            expanded = wn_true;
        }

        if (_bounds.mMaximum.mZ > mMaximum.mZ) {
            mMaximum.mZ = _bounds.mMaximum.mZ;

            expanded = wn_true;
        }

        return(expanded);
    }

    template <typename type>
    wn_void WNBounds3<type>::Translate(const WNVector<type>& _vector) {
        mMinimum += _vector;
        mMaximum += _vector;
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetExpanded(type _amount) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Expand(_amount), bounds);
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetExpanded(const WNVector<type>& _vector) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Expand(_vector), bounds);
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetExpanded(const WNBox<type>& _box) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Expand(_box), bounds);
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetExpanded(const WNSphere<type>& _sphere) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Expand(_sphere), bounds);
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetExpanded(const WNBounds3<type>& _bounds) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Expand(_bounds), bounds);
    }

    template <typename type>
    WNBounds3<type> WNBounds3<type>::GetTranslated(const WNVector<type>& _vector) const {
        WNBounds3<type> bounds = *this;

        return(bounds.Translate(_vector), bounds);
    }

    template <typename type>
    wn_void WNBounds3<type>::GetPoints(WNVector<type>* _vectors) const {
        _vectors[0] = mMaximum;
        _vectors[6] = mMinimum;
        _vectors[1] = (mMinimum * WNVector<type>(static_cast<type>(0), static_cast<type>(1), static_cast<type>(0)) +
                      mMaximum * WNVector<type>(static_cast<type>(1), static_cast<type>(0), static_cast<type>(1))).ToPoint3();
        _vectors[2] = (mMinimum * WNVector<type>(static_cast<type>(1), static_cast<type>(1), static_cast<type>(0)) +
                      mMaximum * WNVector<type>(static_cast<type>(0), static_cast<type>(0), static_cast<type>(1))).ToPoint3();
        _vectors[3] = (mMinimum * WNVector<type>(static_cast<type>(1), static_cast<type>(0), static_cast<type>(0)) +
                      mMaximum * WNVector<type>(static_cast<type>(0), static_cast<type>(1), static_cast<type>(1))).ToPoint3();
        _vectors[4] = (mMinimum * WNVector<type>(static_cast<type>(0), static_cast<type>(0), static_cast<type>(1)) +
                      mMaximum * WNVector<type>(static_cast<type>(1), static_cast<type>(1), static_cast<type>(0))).ToPoint3();
        _vectors[5] = (mMinimum * WNVector<type>(static_cast<type>(0), static_cast<type>(1), static_cast<type>(1)) +
                      mMaximum * WNVector<type>(static_cast<type>(1), static_cast<type>(0), static_cast<type>(0))).ToPoint3();
        _vectors[7] = (mMinimum * WNVector<type>(static_cast<type>(1), static_cast<type>(0), static_cast<type>(1)) +
                      mMaximum * WNVector<type>(static_cast<type>(1), static_cast<type>(1), static_cast<type>(0))).ToPoint3();
    }

    template <typename type>
    WNBox<type> WNBounds3<type>::ToBox() const {
        const WNVector<type> location = WNMidPoint(mMinimum, mMaximum);
        const WNVector<type> extents = mMaximum - location;

        return(WNBox<type>(location, extents, WNQuaternion<type>()));
    }
}

#endif // __WN_MATH_BOUNDS_3_INL__