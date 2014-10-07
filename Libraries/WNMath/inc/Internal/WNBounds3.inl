////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_BOUNDS_3_INL__
#define __WN_MATH_BOUNDS_3_INL__

#ifndef __WN_MATH_BOUNDS_3_H__
    #error "WNBounds3.inl should never be included directly. Please include WNBounds3.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBox.h"
#include "WNMath/inc/WNSphere.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNBounds3<Type>::WNBounds3() :
        mMinimum(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)),
        mMaximum(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0)) {
    }

    template <typename Type>
    WNBounds3<Type>::WNBounds3(const Type* _numbers) :
        mMinimum(_numbers[0], _numbers[1], _numbers[2]),
        mMaximum(_numbers[3], _numbers[4], _numbers[5]) {
    }

    template <typename Type>
    WNBounds3<Type>::WNBounds3(const WNVector3<Type>& _vector) :
        mMinimum(_vector),
        mMaximum(_vector) {
    }

    template <typename Type>
    WNBounds3<Type>::WNBounds3(const WNVector3<Type>& _min, const WNVector3<Type>& _max) :
        mMinimum(_min),
        mMaximum(_max) {
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::operator == (const WNBounds3<Type>& _bounds) const {
        return(mMinimum == _bounds.mMinimum && mMaximum == _bounds.mMaximum);
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::operator != (const WNBounds3<Type>& _bounds) const {
        return(mMinimum != _bounds.mMinimum || mMaximum != _bounds.mMaximum);
    }

    template <typename Type>
    WN_VOID WNBounds3<Type>::Zero() {
        mMinimum.Zero();
        mMaximum.Zero();
    }

    template <typename Type>
    Type WNBounds3<Type>::Length() const {
        const WNVector3<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX));
    }

    template <typename Type>
    Type WNBounds3<Type>::Width() const {
        const WNVector3<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mZ));
    }

    template <typename Type>
    Type WNBounds3<Type>::Height() const {
        const WNVector3<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mY));
    }

    template <typename Type>
    Type WNBounds3<Type>::Radius() const {
        Type total = static_cast<Type>(0);

        for (WN_UINT8 i = 0; i < 3; i++) {
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
    Type WNBounds3<Type>::SurfaceArea() const {
        const WNVector3<Type> size = mMaximum - mMinimum;
        const Type xy = WNAbs(size.mX * size.mY);
        const Type xz = WNAbs(size.mX * size.mZ);
        const Type yz = WNAbs(size.mY * size.mZ);

        return(xy + xz + yz);
    }

    template <typename Type>
    Type WNBounds3<Type>::Volume() const {
        const WNVector3<Type> size = mMaximum - mMinimum;

        return(WNAbs(size.mX * size.mY * size.mZ));
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::IsZero() const {
        return(mMinimum.IsZero() && mMaximum.IsZero());
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::IsInsideOut() const {
        return(mMinimum.mX > mMaximum.mX || mMinimum.mY > mMaximum.mY || mMinimum.mZ > mMaximum.mZ);
    }

    template <typename Type>
    WN_VOID WNBounds3<Type>::Expand(Type _amount) {
        const WNVector3<Type> expand = WNVector3<Type>(_amount);

        mMaximum += expand;
        mMinimum -= expand;
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::Expand(const WNVector3<Type>& _vector) {
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

        if (_vector.mZ < mMinimum.mZ) {
            mMinimum.mZ = _vector.mZ;

            expanded = WN_TRUE;
        }

        if (_vector.mZ > mMaximum.mZ) {
            mMaximum.mZ = _vector.mZ;

            expanded = WN_TRUE;
        }

        return(expanded);
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::Expand(const WNBox<Type>& _box) {
        const WNVector3<Type> extents = _box.mExtents.GetRotated(_box.mRotation);
        const WNVector3<Type> point1 = _box.mLocation + extents;
        const WNVector3<Type> point2 = _box.mLocation + WNVector3<Type>(-extents.mX, extents.mY, extents.mZ);
        const WNVector3<Type> point3 = _box.mLocation + WNVector3<Type>(extents.mX, -extents.mY, extents.mZ);
        const WNVector3<Type> point4 = _box.mLocation + WNVector3<Type>(extents.mX, extents.mY, -extents.mZ);
        const WNVector3<Type> point5 = _box.mLocation + WNVector3<Type>(-extents.mX, -extents.mY, extents.mZ);
        const WNVector3<Type> point6 = _box.mLocation + WNVector3<Type>(extents.mX, -extents.mY, -extents.mZ);
        const WNVector3<Type> point7 = _box.mLocation + WNVector3<Type>(-extents.mX, extents.mY, -extents.mZ);
        const WNVector3<Type> point8 = _box.mLocation - extents;

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4) ||
               Expand(point5) || Expand(point6) || Expand(point7) || Expand(point8));
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::Expand(const WNSphere<Type>& _sphere) {
        const WNVector3<Type> point1 = WNVector3<Type>(_sphere.mLocation.mX + _sphere.mRadius, _sphere.mLocation.mY, _sphere.mLocation.mZ);
        const WNVector3<Type> point2 = WNVector3<Type>(_sphere.mLocation.mX - _sphere.mRadius, _sphere.mLocation.mY, _sphere.mLocation.mZ);
        const WNVector3<Type> point3 = WNVector3<Type>(_sphere.mLocation.mX, _sphere.mLocation.mY + _sphere.mRadius, _sphere.mLocation.mZ);
        const WNVector3<Type> point4 = WNVector3<Type>(_sphere.mLocation.mX, _sphere.mLocation.mY - _sphere.mRadius, _sphere.mLocation.mZ);
        const WNVector3<Type> point5 = WNVector3<Type>(_sphere.mLocation.mX, _sphere.mLocation.mY, _sphere.mLocation.mZ + _sphere.mRadius);
        const WNVector3<Type> point6 = WNVector3<Type>(_sphere.mLocation.mX, _sphere.mLocation.mY, _sphere.mLocation.mZ - _sphere.mRadius);

        return(Expand(point1) || Expand(point2) || Expand(point3) || Expand(point4) || Expand(point5) || Expand(point6));
    }

    template <typename Type>
    WN_BOOL WNBounds3<Type>::Expand(const WNBounds3<Type>& _bounds) {
        WN_BOOL expanded = WN_FALSE;

        if (_bounds.mMinimum.mX < mMinimum.mX) {
            mMinimum.mX = _bounds.mMinimum.mX;

            expanded = WN_TRUE;
        }

        if (_bounds.mMinimum.mY < mMinimum.mY) {
            mMinimum.mY = _bounds.mMinimum.mY;

            expanded = WN_TRUE;
        }

        if (_bounds.mMinimum.mZ < mMinimum.mZ) {
            mMinimum.mZ = _bounds.mMinimum.mZ;

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

        if (_bounds.mMaximum.mZ > mMaximum.mZ) {
            mMaximum.mZ = _bounds.mMaximum.mZ;

            expanded = WN_TRUE;
        }

        return(expanded);
    }

    template <typename Type>
    WN_VOID WNBounds3<Type>::Translate(const WNVector3<Type>& _vector) {
        mMinimum += _vector;
        mMaximum += _vector;
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetExpanded(Type _amount) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Expand(_amount), bounds);
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetExpanded(const WNVector3<Type>& _vector) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Expand(_vector), bounds);
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetExpanded(const WNBox<Type>& _box) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Expand(_box), bounds);
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetExpanded(const WNSphere<Type>& _sphere) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Expand(_sphere), bounds);
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetExpanded(const WNBounds3<Type>& _bounds) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Expand(_bounds), bounds);
    }

    template <typename Type>
    WNBounds3<Type> WNBounds3<Type>::GetTranslated(const WNVector3<Type>& _vector) const {
        WNBounds3<Type> bounds = *this;

        return(bounds.Translate(_vector), bounds);
    }

    template <typename Type>
    WN_VOID WNBounds3<Type>::GetPoints(WNVector3<Type>* _vectors) const {
        _vectors[0] = mMaximum;
        _vectors[6] = mMinimum;
        _vectors[1] = (mMinimum * WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(1), static_cast<Type>(0)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(0), static_cast<Type>(1))).ToPoint3();
        _vectors[2] = (mMinimum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(1), static_cast<Type>(0)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1))).ToPoint3();
        _vectors[3] = (mMinimum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(0), static_cast<Type>(0)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(1), static_cast<Type>(1))).ToPoint3();
        _vectors[4] = (mMinimum * WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(1), static_cast<Type>(0))).ToPoint3();
        _vectors[5] = (mMinimum * WNVector3<Type>(static_cast<Type>(0), static_cast<Type>(1), static_cast<Type>(1)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(0), static_cast<Type>(0))).ToPoint3();
        _vectors[7] = (mMinimum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(0), static_cast<Type>(1)) +
                      mMaximum * WNVector3<Type>(static_cast<Type>(1), static_cast<Type>(1), static_cast<Type>(0))).ToPoint3();
    }

    template <typename Type>
    WNBox<Type> WNBounds3<Type>::ToBox() const {
        const WNVector3<Type> location = WNMidPoint(mMinimum, mMaximum);
        const WNVector3<Type> extents = mMaximum - location;

        return(WNBox<Type>(location, extents, WNQuaternion<Type>()));
    }
}

#endif // __WN_MATH_BOUNDS_3_INL__