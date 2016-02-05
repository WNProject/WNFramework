// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOUNDS_2_INL__
#define __WN_MATH_BOUNDS_2_INL__

#ifndef __WN_MATH_BOUNDS_2_H__
    //#error "WNBounds2.inl should never be included directly. Please include WNBounds2.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCircle.h"
#include "WNMath/inc/WNRectangle.h"
#include "WNMath/inc/WNVector2.h"

namespace wn {
    template <typename type>
    WNBounds2<type>::WNBounds2() :
        mMinimum(static_cast<type>(0), static_cast<type>(0)),
        mMaximum(static_cast<type>(0), static_cast<type>(0)) {
    }

    template <typename type>
    WNBounds2<type>::WNBounds2(const type* _numbers) :
        mMinimum(_numbers[0], _numbers[1]),
        mMaximum(_numbers[2], _numbers[3]) {
    }

    template <typename type>
    WNBounds2<type>::WNBounds2(const WNVector2<type>& _vector) :
        mMinimum(_vector),
        mMaximum(_vector) {
    }

    template <typename type>
    WNBounds2<type>::WNBounds2(const WNVector2<type>& _min, const WNVector2<type>& _max) :
        mMinimum(_min),
        mMaximum(_max) {
    }

    template <typename type>
    bool WNBounds2<type>::operator == (const WNBounds2<type>& _bounds) const {
        return(mMinimum == _bounds.mMinimum && mMaximum == _bounds.mMaximum);
    }

    template <typename type>
    bool WNBounds2<type>::operator != (const WNBounds2<type>& _bounds) const {
        return(mMinimum != _bounds.mMinimum || mMaximum != _bounds.mMaximum);
    }

    template <typename type>
    void WNBounds2<type>::Zero() {
        mMinimum.Zero();
        mMaximum.Zero();
    }

    template <typename type>
    type WNBounds2<type>::Length() const {
        const WNVector2<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX));
    }

    template <typename type>
    type WNBounds2<type>::Width() const {
        const WNVector2<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mY));
    }

    template <typename type>
    type WNBounds2<type>::Perimeter() const {
        const WNVector2<type> size = mMaximum - mMinimum;

        return(WNAbs(static_cast<type>(2) * size.mX) + WNAbs(static_cast<type>(2) * size.mY));
    }

    template <typename type>
    type WNBounds2<type>::Radius() const {
        type total = static_cast<type>(0);

        for (uint8_t i = 0; i < 2; i++) {
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
    type WNBounds2<type>::Area() const {
        const WNVector2<type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX * size.mY));
    }

    template <typename type>
    bool WNBounds2<type>::IsZero() const {
        return(mMinimum.IsZero() && mMaximum.IsZero());
    }

    template <typename type>
    bool WNBounds2<type>::IsInsideOut() const {
        return(mMinimum.mX > mMaximum.mX || mMinimum.mY > mMaximum.mY);
    }

    template <typename type>
    void WNBounds2<type>::Expand(type _amount) {
        const WNVector2<type> expand = WNVector2<type>(_amount);

        mMaximum += expand;
        mMinimum -= expand;
    }

    template <typename type>
    bool WNBounds2<type>::Expand(const WNVector2<type>& _vector) {
        bool expanded = false;

        if (_vector.mX < mMinimum.mX) {
            mMinimum.mX = _vector.mX;

            expanded = true;
        }

        if (_vector.mX > mMaximum.mX) {
            mMaximum.mX = _vector.mX;

            expanded = true;
        }

        if (_vector.mY < mMinimum.mY) {
            mMinimum.mY = _vector.mY;

            expanded = true;
        }

        if (_vector.mY > mMaximum.mY) {
            mMaximum.mY = _vector.mY;

            expanded = true;
        }

        return(expanded);
    }

    template <typename type>
    bool WNBounds2<type>::Expand(const WNRectangle<type>& _rectangle) {
        const WNVector2<type> extents = _rectangle.mExtents.GetRotated(_rectangle.mRotation);
        const WNVector2<type> point1 = _rectangle.mLocation + extents;
        const WNVector2<type> point2 = _rectangle.mLocation + WNVector2<type>(-extents.mX, extents.mY);
        const WNVector2<type> point3 = _rectangle.mLocation + WNVector2<type>(extents.mX, -extents.mY);
        const WNVector2<type> point4 = _rectangle.mLocation - extents;

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4));
    }

    template <typename type>
    bool WNBounds2<type>::Expand(const WNCircle<type>& _circle) {
        const WNVector2<type> point1 = WNVector2<type>(_circle.mLocation.mX + _circle.mRadius, _circle.mLocation.mY);
        const WNVector2<type> point2 = WNVector2<type>(_circle.mLocation.mX - _circle.mRadius, _circle.mLocation.mY);
        const WNVector2<type> point3 = WNVector2<type>(_circle.mLocation.mX, _circle.mLocation.mY + _circle.mRadius);
        const WNVector2<type> point4 = WNVector2<type>(_circle.mLocation.mX, _circle.mLocation.mY - _circle.mRadius);

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4));
    }

    template <typename type>
    bool WNBounds2<type>::Expand(const WNBounds2<type>& _bounds) {
        bool expanded = false;

        if (_bounds.mMinimum.mX < mMinimum.mX) {
            mMinimum.mX = _bounds.mMinimum.mX;

            expanded = true;
        }

        if (_bounds.mMinimum.mY < mMinimum.mY) {
            mMinimum.mY = _bounds.mMinimum.mY;

            expanded = true;
        }

        if (_bounds.mMaximum.mX > mMaximum.mX) {
            mMaximum.mX = _bounds.mMaximum.mX;

            expanded = true;
        }

        if (_bounds.mMaximum.mY > mMaximum.mY) {
            mMaximum.mY = _bounds.mMaximum.mY;

            expanded = true;
        }

        return(expanded);
    }

    template <typename type>
    void WNBounds2<type>::Translate(const WNVector2<type>& _vector) {
        mMinimum += _vector;
        mMaximum += _vector;
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetExpanded(type _amount) const {
        WNBounds2 bounds = *this;

        return(bounds.Expand(_amount), bounds);
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetExpanded(const WNVector2<type>& _vector) const {
        WNBounds2 bounds = *this;

        return(bounds.Expand(_vector), bounds);
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetExpanded(const WNRectangle<type>& _rectangle) const {
        WNBounds2<type> bounds = *this;

        return(bounds.Expand(_rectangle), bounds);
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetExpanded(const WNCircle<type>& _circle) const {
        WNBounds2<type> bounds = *this;

        return(bounds.Expand(_circle), bounds);
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetExpanded(const WNBounds2<type>& _bounds) const {
        WNBounds2<type> bounds = *this;

        return(bounds.Expand(_bounds), bounds);
    }

    template <typename type>
    WNBounds2<type> WNBounds2<type>::GetTranslated(const WNVector2<type>& _vector) const {
        WNBounds2<type> bounds = *this;

        return(bounds.Translate(_vector), bounds);
    }

    template <typename type>
    void WNBounds2<type>::GetPoints(WNVector2<type>* _vectors) const {
        _vectors[0] = mMaximum;
        _vectors[1] = (mMinimum * WNVector2<type>(static_cast<type>(0), static_cast<type>(1)) +
                      mMaximum * WNVector2<type>(static_cast<type>(1), static_cast<type>(0))).ToPoint2();
        _vectors[2] = (mMinimum * WNVector2<type>(static_cast<type>(1), static_cast<type>(0)) +
                      mMaximum * WNVector2<type>(static_cast<type>(0), static_cast<type>(1))).ToPoint2();
        _vectors[3] = mMinimum;
    }

    template <typename type>
    WNRectangle<type> WNBounds2<type>::ToRectangle() const {
        const WNVector2<type> location = WNMidPoint(mMinimum, mMaximum);
        const WNVector2<type> extents = mMaximum - location;

        return(WNRectangle<type>(location, extents, static_cast<type>(0)));
    }
}

#endif // __WN_MATH_BOUNDS_2_INL__