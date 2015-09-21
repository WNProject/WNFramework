// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOX_INL__
#define __WN_MATH_BOX_INL__

#ifndef __WN_MATH_BOX_H__
    #error "WNBox.inl should never be included directly. Please include WNBox.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBounds3.h"
#include "WNMath/inc/WNMatrix3.h"
#include "WNMath/inc/WNSphere.h"
#include "WNMath/inc/WNVector.h"

namespace wn {
    template <typename type>
    WNBox<type>::WNBox() :
        mLocation(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)),
        mExtents(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)),
        mRotation(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0), static_cast<type>(1)) {
    }

    template <typename type>
    WNBox<type>::WNBox(const type* _numbers) :
        mLocation(_numbers[0], _numbers[1], _numbers[2]),
        mExtents(_numbers[3], _numbers[4], _numbers[5]),
        mRotation(_numbers[6], _numbers[7], _numbers[8], _numbers[9]) {
    }

    template <typename type>
    WNBox<type>::WNBox(const WNVector<type>& _location, const WNVector<type>& _extents, const WNQuaternion<type>& _rotation) :
        mLocation(_location),
        mExtents(_extents),
        mRotation(_rotation) {
    }

    template <typename type>
    WNBox<type>::WNBox(const WNVector<type>& _vector) :
        mLocation(_vector),
        mExtents(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0)),
        mRotation(static_cast<type>(0), static_cast<type>(0), static_cast<type>(0), static_cast<type>(1)) {
    }

    template <typename type>
    wn_bool WNBox<type>::operator == (const WNBox& _box) const {
        return(mExtents == _box.mExtents && mLocation == _box.mLocation && mRotation == _box.mRotation);
    }

    template <typename type>
    wn_bool WNBox<type>::operator != (const WNBox& _box) const {
        return(mExtents != _box.mExtents || mLocation != _box.mLocation || mRotation != _box.mRotation);
    }

    template <typename type>
    wn_void WNBox<type>::Zero() {
        mLocation.Zero();
        mExtents.Zero();
        mRotation.Identity();
    }

    template <typename type>
    wn_void WNBox<type>::Set(const type* _numbers) {
        mLocation.mX = _numbers[0];
        mLocation.mY = _numbers[1];
        mLocation.mZ = _numbers[2];

        mExtents.mX = _numbers[3];
        mExtents.mY = _numbers[4];
        mExtents.mZ = _numbers[5];

        mRotation.mX = _numbers[6];
        mRotation.mY = _numbers[7];
        mRotation.mZ = _numbers[8];
        mRotation.mW = _numbers[9];
    }

    template <typename type>
    wn_void WNBox<type>::Set(const WNVector<type>& _location, WNVector<type>& _extents, const WNQuaternion<type>& _rotation) {
        mLocation = _location;
        mExtents = _extents;
        mRotation = _rotation;
    }

    template <typename type>
    wn_void WNBox<type>::Set(const WNVector<type>& _vector) {
        mLocation = _vector;
        mExtents.Zero();
        mRotation.Identity();
    }

    template <typename type>
    wn_void WNBox<type>::Expand(type _amount) {
        mExtents += _amount;
    }

    template <typename type>
    wn_bool WNBox<type>::Expand(const WNVector<type>& _vector, wn_bool _anchor) {
        const WNVector<type> transformedPoint = ((_vector - mLocation).GetRotated(mRotation.GetInverted()).ToPoint3());

        if (!_anchor) {
            WNBounds3<type> bounds = WNBounds3<type>(-mExtents.ToPoint3(), mExtents.ToPoint3());

            if (bounds.Expand(transformedPoint)) {
                const WNBox<type> newRectange = bounds.ToBox();

                mLocation += newRectange.mLocation.ToVector3();
                mExtents = newRectange.mExtents;

                return(wn_true);
            }
        } else {
            const type transformedAbsX = WNAbs(transformedPoint.mX);
            const type transformedAbsY = WNAbs(transformedPoint.mY);
            const type transformedAbsZ = WNAbs(transformedPoint.mZ);
            const type extendsAbsX = WNAbs(mExtents.mX);
            const type extendsAbsY = WNAbs(mExtents.mY);
            const type extendsAbsZ = WNAbs(mExtents.mZ);

            wn_bool expanded = wn_false;

            if (transformedAbsX > extendsAbsX) {
                mExtents.mX = transformedPoint.mX;

                expanded = wn_true;
            }

            if (transformedAbsY > extendsAbsY) {
                mExtents.mY = transformedPoint.mY;

                expanded = wn_true;
            }

            if (transformedAbsZ > extendsAbsY) {
                mExtents.mZ = transformedPoint.mZ;

                expanded = wn_true;
            }

            return(expanded);
        }

        return(wn_false);
    }

    template <typename type>
    wn_bool WNBox<type>::Expand(const WNBounds3<type>& _bounds, wn_bool _anchor) {
        WNVector<type> points[8];

        _bounds.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename type>
    wn_bool WNBox<type>::Expand(const WNBox& _box, wn_bool _anchor) {
        WNVector<type> points[8];

        _box.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename type>
    wn_bool WNBox<type>::Expand(const WNSphere<type>& _sphere, wn_bool _anchor) {
        const WNVector<type> transformedLocation = ((_sphere.mLocation - mLocation).GetRotated(mRotation.GetInverted()).ToPoint3());
        const WNVector<type> componentX = WNVector<type>(_sphere.mRadius, static_cast<type>(0), static_cast<type>(0));
        const WNVector<type> componentY = WNVector<type>(static_cast<type>(0), _sphere.mRadius, static_cast<type>(0));
        const WNVector<type> componentZ = WNVector<type>(static_cast<type>(0), static_cast<type>(0), _sphere.mRadius);
        const WNVector<type> point1 = transformedLocation + componentX;
        const WNVector<type> point2 = transformedLocation - componentX;
        const WNVector<type> point3 = transformedLocation + componentY;
        const WNVector<type> point4 = transformedLocation - componentY;
        const WNVector<type> point5 = transformedLocation + componentZ;
        const WNVector<type> point6 = transformedLocation - componentZ;

        return(Expand(point1, _anchor) || Expand(point2, _anchor) ||
               Expand(point3, _anchor) || Expand(point4, _anchor) ||
               Expand(point5, _anchor) || Expand(point6, _anchor));
    }

    template <typename type>
    wn_void WNBox<type>::Translate(type _x, type _y, type _z) {
        mLocation.mX += _x;
        mLocation.mY += _y;
        mLocation.mZ += _z;
    }

    template <typename type>
    wn_void WNBox<type>::Translate(const WNVector<type>& _vector) {
        mLocation += _vector;
    }

    template <typename type>
    wn_void WNBox<type>::Rotate(const WNQuaternion<type>& _rotation) {
        mRotation.Rotate(_rotation);
    }

    template <typename type>
    wn_void WNBox<type>::Scale(type _scale) {
        mExtents.Scale(_scale);
    }

    template <typename type>
    wn_void WNBox<type>::Scale(type _x, type _y, type _z) {
        mExtents.Scale(_x, _y, _z);
    }

    template <typename type>
    wn_void WNBox<type>::Scale(const WNVector<type>& _scale) {
        mExtents.Scale(_scale);
    }

    template <typename type>
    type WNBox<type>::Length() const {
        return(WNAbs(static_cast<type>(2) * mExtents.mX));
    }

    template <typename type>
    type WNBox<type>::Width() const {
        return(WNAbs(static_cast<type>(2) * mExtents.mZ));
    }

    template <typename type>
    type WNBox<type>::Height() const {
        return(WNAbs(static_cast<type>(2) * mExtents.mY));
    }

    template <typename type>
    type WNBox<type>::SurfaceArea() const {
        const type mX = WNAbs(mExtents.mX * static_cast<type>(2));
        const type mY = WNAbs(mExtents.mY * static_cast<type>(2));
        const type mZ = WNAbs(mExtents.mZ * static_cast<type>(2));
        const type xy = mX * mY;
        const type xz = mX * mZ;
        const type yz = mY * mZ;

        return(xy + xz + yz);
    }

    template <typename type>
    type WNBox<type>::Volume() const {
        const WNVector<type> dimensions = mExtents * static_cast<type>(2);

        return(dimensions.mX * dimensions.mY * dimensions.mZ);
    }

    template <typename type>
    wn_bool WNBox<type>::IsZero() const {
        return(mLocation.IsZero() && mExtents.IsZero() && mRotation.IsZero());
    }

    template <typename type>
    wn_bool WNBox<type>::IsInsideOut() const {
        return(mExtents.mX < static_cast<type>(0) || mExtents.mY < static_cast<type>(0) || mExtents.mZ < static_cast<type>(0));
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetExpanded(type _amount) const {
        WNBox<type> box = *this;

        return(box.Expand(_amount), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetExpanded(const WNVector<type>& _vector, wn_bool _anchor) const {
        WNBox<type> box = *this;

        return(box.Expand(_vector, _anchor), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetExpanded(const WNSphere<type>& _sphere, wn_bool _anchor) const {
        WNBox<type> box = *this;

        return(box.Expand(_sphere, _anchor), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetExpanded(const WNBounds3<type>& _bounds, wn_bool _anchor) const {
        WNBox<type> box = *this;

        return(box.Expand(_bounds, _anchor), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetExpanded(const WNBox& _box, wn_bool _anchor) const {
        WNBox<type> box = *this;

        return(box.Expand(_box, _anchor), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetTranslated(type _x, type _y, type _z) {
        WNBox<type> box = *this;

        return(box.Translate(_x, _y, _z), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetTranslated(const WNVector<type>& _vector) const {
        WNBox<type> box = *this;

        return(box.Translate(_vector), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetRotated(const WNQuaternion<type>& _rotation) const {
        WNBox<type> box = *this;

        return(box.Rotate(_rotation), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetScaled(type _scale) const {
        WNBox<type> box = *this;

        return(box.Scale(_scale), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetScaled(type _x, type _y, type _z) const {
        WNBox<type> box = *this;

        return(box.Scale(_x, _y, _z), box);
    }

    template <typename type>
    WNBox<type> WNBox<type>::GetScaled(const WNVector<type>& _scale) const {
        WNBox<type> box = *this;

        return(box.Scale(_scale), box);
    }

    template <typename type>
    wn_void WNBox<type>::GetPoints(WNVector<type>* _vectors) const{
        const WNVector<type> transformedExtents = mExtents.GetRotated(mRotation);
        const WNVector<type> transformedXExtents = (mExtents * WNVector<type>(static_cast<type>(-1), static_cast<type>(1), static_cast<type>(1))).GetRotated(mRotation);
        const WNVector<type> transformedYExtents = (mExtents * WNVector<type>(static_cast<type>(1), -static_cast<type>(-1), static_cast<type>(1))).GetRotated(mRotation);
        const WNVector<type> transformedZExtents = (mExtents * WNVector<type>(static_cast<type>(1), static_cast<type>(1), -static_cast<type>(-1))).GetRotated(mRotation);

        _vectors[0] = mLocation - transformedExtents;
        _vectors[6] = mLocation + transformedExtents;
        _vectors[3] = mLocation - transformedXExtents;
        _vectors[5] = mLocation + transformedXExtents;
        _vectors[1] = mLocation - transformedYExtents;
        _vectors[7] = mLocation + transformedYExtents;
        _vectors[4] = mLocation - transformedZExtents;
        _vectors[2] = mLocation + transformedZExtents;
    }
}

#endif // __WN_MATH_BOX_INL__