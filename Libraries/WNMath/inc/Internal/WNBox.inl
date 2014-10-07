////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNBox<Type>::WNBox() :
        mLocation(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mExtents(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mRotation(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)) {
    }

    template <typename Type>
    WNBox<Type>::WNBox(const Type* _numbers) :
        mLocation(_numbers[0], _numbers[1], _numbers[2]),
        mExtents(_numbers[3], _numbers[4], _numbers[5]),
        mRotation(_numbers[6], _numbers[7], _numbers[8], _numbers[9]) {
    }

    template <typename Type>
    WNBox<Type>::WNBox(const WNVector3<Type>& _location, const WNVector3<Type>& _extents, const WNQuaternion<Type>& _rotation) :
        mLocation(_location),
        mExtents(_extents),
        mRotation(_rotation) {
    }

    template <typename Type>
    WNBox<Type>::WNBox(const WNVector3<Type>& _vector) :
        mLocation(_vector),
        mExtents(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mRotation(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)) {
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::operator == (const WNBox& _box) const {
        return(mExtents == _box.mExtents && mLocation == _box.mLocation && mRotation == _box.mRotation);
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::operator != (const WNBox& _box) const {
        return(mExtents != _box.mExtents || mLocation != _box.mLocation || mRotation != _box.mRotation);
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Zero() {
        mLocation.Zero();
        mExtents.Zero();
        mRotation.Identity();
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Set(const Type* _numbers) {
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

    template <typename Type>
    WN_VOID WNBox<Type>::Set(const WNVector3<Type>& _location, WNVector3<Type>& _extents, const WNQuaternion<Type>& _rotation) {
        mLocation = _location;
        mExtents = _extents;
        mRotation = _rotation;
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Set(const WNVector3<Type>& _vector) {
        mLocation = _vector;
        mExtents.Zero();
        mRotation.Identity();
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Expand(Type _amount) {
        mExtents += _amount;
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor) {
        const WNVector3<Type> transformedPoint = ((_vector - mLocation).GetRotated(mRotation.GetInverted()).ToPoint3());

        if (!_anchor) {
            WNBounds3<Type> bounds = WNBounds3<Type>(-mExtents.ToPoint3(), mExtents.ToPoint3());

            if (bounds.Expand(transformedPoint)) {
                const WNBox<Type> newRectange = bounds.ToBox();

                mLocation += newRectange.mLocation.ToVector3();
                mExtents = newRectange.mExtents;

                return(WN_TRUE);
            }
        } else {
            const Type transformedAbsX = WNAbs(transformedPoint.mX);
            const Type transformedAbsY = WNAbs(transformedPoint.mY);
            const Type transformedAbsZ = WNAbs(transformedPoint.mZ);
            const Type extendsAbsX = WNAbs(mExtents.mX);
            const Type extendsAbsY = WNAbs(mExtents.mY);
            const Type extendsAbsZ = WNAbs(mExtents.mZ);

            WN_BOOL expanded = WN_FALSE;

            if (transformedAbsX > extendsAbsX) {
                mExtents.mX = transformedPoint.mX;

                expanded = WN_TRUE;
            }

            if (transformedAbsY > extendsAbsY) {
                mExtents.mY = transformedPoint.mY;

                expanded = WN_TRUE;
            }

            if (transformedAbsZ > extendsAbsY) {
                mExtents.mZ = transformedPoint.mZ;

                expanded = WN_TRUE;
            }

            return(expanded);
        }

        return(WN_FALSE);
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::Expand(const WNBounds3<Type>& _bounds, WN_BOOL _anchor) {
        WNVector3<Type> points[8];

        _bounds.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::Expand(const WNBox& _box, WN_BOOL _anchor) {
        WNVector3<Type> points[8];

        _box.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor) ||
               Expand(points[4], _anchor) || Expand(points[5], _anchor) ||
               Expand(points[6], _anchor) || Expand(points[7], _anchor));
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::Expand(const WNSphere<Type>& _sphere, WN_BOOL _anchor) {
        const WNVector3<Type> transformedLocation = ((_sphere.mLocation - mLocation).GetRotated(mRotation.GetInverted()).ToPoint3());
        const WNVector3<Type> componentX = WNVector3<Type>(_sphere.mRadius, static_cast<Type>(0), static_cast<Type>(0));
        const WNVector3<Type> componentY = WNVector3<Type>(static_cast<Type>(0), _sphere.mRadius, static_cast<Type>(0));
        const WNVector3<Type> componentZ = WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(0), _sphere.mRadius);
        const WNVector3<Type> point1 = transformedLocation + componentX;
        const WNVector3<Type> point2 = transformedLocation - componentX;
        const WNVector3<Type> point3 = transformedLocation + componentY;
        const WNVector3<Type> point4 = transformedLocation - componentY;
        const WNVector3<Type> point5 = transformedLocation + componentZ;
        const WNVector3<Type> point6 = transformedLocation - componentZ;

        return(Expand(point1, _anchor) || Expand(point2, _anchor) ||
               Expand(point3, _anchor) || Expand(point4, _anchor) ||
               Expand(point5, _anchor) || Expand(point6, _anchor));
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Translate(Type _x, Type _y, Type _z) {
        mLocation.mX += _x;
        mLocation.mY += _y;
        mLocation.mZ += _z;
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Translate(const WNVector3<Type>& _vector) {
        mLocation += _vector;
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Rotate(const WNQuaternion<Type>& _rotation) {
        mRotation.Rotate(_rotation);
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Scale(Type _scale) {
        mExtents.Scale(_scale);
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Scale(Type _x, Type _y, Type _z) {
        mExtents.Scale(_x, _y, _z);
    }

    template <typename Type>
    WN_VOID WNBox<Type>::Scale(const WNVector3<Type>& _scale) {
        mExtents.Scale(_scale);
    }

    template <typename Type>
    Type WNBox<Type>::Length() const {
        return(WNAbs(static_cast<Type>(2) * mExtents.mX));
    }

    template <typename Type>
    Type WNBox<Type>::Width() const {
        return(WNAbs(static_cast<Type>(2) * mExtents.mZ));
    }

    template <typename Type>
    Type WNBox<Type>::Height() const {
        return(WNAbs(static_cast<Type>(2) * mExtents.mY));
    }

    template <typename Type>
    Type WNBox<Type>::SurfaceArea() const {
        const Type mX = WNAbs(mExtents.mX * static_cast<Type>(2));
        const Type mY = WNAbs(mExtents.mY * static_cast<Type>(2));
        const Type mZ = WNAbs(mExtents.mZ * static_cast<Type>(2));
        const Type xy = mX * mY;
        const Type xz = mX * mZ;
        const Type yz = mY * mZ;

        return(xy + xz + yz);
    }

    template <typename Type>
    Type WNBox<Type>::Volume() const {
        const WNVector3<Type> dimensions = mExtents * static_cast<Type>(2);

        return(dimensions.mX * dimensions.mY * dimensions.mZ);
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::IsZero() const {
        return(mLocation.IsZero() && mExtents.IsZero() && mRotation.IsZero());
    }

    template <typename Type>
    WN_BOOL WNBox<Type>::IsInsideOut() const {
        return(mExtents.mX < static_cast<Type>(0) || mExtents.mY < static_cast<Type>(0) || mExtents.mZ < static_cast<Type>(0));
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetExpanded(Type _amount) const {
        WNBox<Type> box = *this;

        return(box.Expand(_amount), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor) const {
        WNBox<Type> box = *this;

        return(box.Expand(_vector, _anchor), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetExpanded(const WNSphere<Type>& _sphere, WN_BOOL _anchor) const {
        WNBox<Type> box = *this;

        return(box.Expand(_sphere, _anchor), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetExpanded(const WNBounds3<Type>& _bounds, WN_BOOL _anchor) const {
        WNBox<Type> box = *this;

        return(box.Expand(_bounds, _anchor), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetExpanded(const WNBox& _box, WN_BOOL _anchor) const {
        WNBox<Type> box = *this;

        return(box.Expand(_box, _anchor), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetTranslated(Type _x, Type _y, Type _z) {
        WNBox<Type> box = *this;

        return(box.Translate(_x, _y, _z), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetTranslated(const WNVector3<Type>& _vector) const {
        WNBox<Type> box = *this;

        return(box.Translate(_vector), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetRotated(const WNQuaternion<Type>& _rotation) const {
        WNBox<Type> box = *this;

        return(box.Rotate(_rotation), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetScaled(Type _scale) const {
        WNBox<Type> box = *this;

        return(box.Scale(_scale), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetScaled(Type _x, Type _y, Type _z) const {
        WNBox<Type> box = *this;

        return(box.Scale(_x, _y, _z), box);
    }

    template <typename Type>
    WNBox<Type> WNBox<Type>::GetScaled(const WNVector3<Type>& _scale) const {
        WNBox<Type> box = *this;

        return(box.Scale(_scale), box);
    }

    template <typename Type>
    WN_VOID WNBox<Type>::GetPoints(WNVector3<Type>* _vectors) const{
        const WNVector3<Type> transformedExtents = mExtents.GetRotated(mRotation);
        const WNVector3<Type> transformedXExtents = (mExtents * WNVector3<Type>(static_cast<Type>(-1), static_cast<Type>(1), static_cast<Type>(1))).GetRotated(mRotation);
        const WNVector3<Type> transformedYExtents = (mExtents * WNVector3<Type>(static_cast<Type>(1), -static_cast<Type>(-1), static_cast<Type>(1))).GetRotated(mRotation);
        const WNVector3<Type> transformedZExtents = (mExtents * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(1), -static_cast<Type>(-1))).GetRotated(mRotation);

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