////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_RECTANGLE_INL__
#define __WN_MATH_RECTANGLE_INL__

#ifndef __WN_MATH_RECTANGLE_H__
    #error "WNRectangle.inl should never be included directly. Please include WNRectangle.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCircle.h"
#include "WNMath/inc/WNBounds2.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNRectangle<Type>::WNRectangle() :
        mLocation(static_cast<Type>(0), static_cast<Type>(0)),
        mExtents(static_cast<Type>(0), static_cast<Type>(0)),
        mRotation(static_cast<Type>(0)) {
    }

    template <typename Type>
    WNRectangle<Type>::WNRectangle(const Type* _numbers) :
        mLocation(_numbers[0], _numbers[1]),
        mExtents(_numbers[2], _numbers[3]),
        mRotation(_numbers[4]) {
    }

    template <typename Type>
    WNRectangle<Type>::WNRectangle(const WNVector3<Type>& _location, const WNVector3<Type>& _extents, Type _rotation) :
        mLocation(_location),
        mExtents(_extents),
        mRotation(_rotation) {
    }

    template <typename Type>
    WNRectangle<Type>::WNRectangle(Type _x1, Type _y1, Type _x2, Type _y2, Type _rotation) :
        mLocation(_x1, _y1),
        mExtents(_x2, _y2),
        mRotation(_rotation) {
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::operator == (const WNRectangle<Type>& _rectangle) const {
        return(mExtents == _rectangle.mExtents && mLocation == _rectangle.mLocation && mRotation == _rectangle.mRotation);
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::operator != (const WNRectangle<Type>& _rectangle) const {
        return(mExtents != _rectangle.mExtents || mLocation != _rectangle.mLocation || mRotation != _rectangle.mRotation);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Zero() {
        mLocation.Zero();
        mExtents.Zero();
        mRotation = static_cast<Type>(0);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Expand(Type _amount) {
        mExtents += _amount;
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor) {
        const WNVector3<Type> transformedPoint = ((_vector - mLocation).GetRotated(-mRotation).ToPoint2());

        if (!_anchor) {
            WNBounds2<Type> bounds = WNBounds2<Type>(-mExtents.ToPoint2(), mExtents.ToPoint2());

            if (bounds.Expand(transformedPoint)) {
                const WNRectangle<Type> newRectange = bounds.ToRectangle();

                mLocation += newRectange.mLocation.ToVector2();
                mExtents = newRectange.mExtents;

                return(WN_TRUE);
            }
        } else {
            const Type transformedAbsX = WNAbs(transformedPoint.mX);
            const Type transformedAbsY = WNAbs(transformedPoint.mY);
            const Type extendsAbsX = WNAbs(mExtents.mX);
            const Type extendsAbsY = WNAbs(mExtents.mY);

            WN_BOOL expanded = WN_FALSE;

            if (transformedAbsX > extendsAbsX) {
                mExtents.mX = transformedPoint.mX;

                expanded = WN_TRUE;
            }

            if (transformedAbsY > extendsAbsY) {
                mExtents.mY = transformedPoint.mY;

                expanded = WN_TRUE;
            }

            return(expanded);
        }

        return(WN_FALSE);
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::Expand(const WNBounds2<Type>& _bounds, WN_BOOL _anchor) {
        WNVector3<Type> points[4];

        _bounds.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor));
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::Expand(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor) {
        WNVector3<Type> points[4];

        _rectangle.GetPoints(points);

        return(Expand(points[0], _anchor) || Expand(points[1], _anchor) ||
               Expand(points[2], _anchor) || Expand(points[3], _anchor));
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::Expand(const WNCircle<Type>& _circle, WN_BOOL _anchor) {
        const WNVector3<Type> transformedLocation = ((_circle.mLocation - mLocation).GetRotated(-mRotation).ToPoint2());
        const WNVector3<Type> componentX = WNVector3<Type>(_circle.mRadius, static_cast<Type>(0));
        const WNVector3<Type> componentY = WNVector3<Type>(static_cast<Type>(0), _circle.mRadius);
        const WNVector3<Type> point1 = transformedLocation + componentX;
        const WNVector3<Type> point2 = transformedLocation - componentX;
        const WNVector3<Type> point3 = transformedLocation + componentY;
        const WNVector3<Type> point4 = transformedLocation - componentY;

        return(Expand(point1, _anchor) || Expand(point2, _anchor) ||
               Expand(point3, _anchor) || Expand(point4, _anchor));
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Translate(Type _x, Type _y) {
        mLocation.mX += _x;
        mLocation.mY += _y;
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Translate(const WNVector3<Type>& _vector) {
        mLocation += _vector;
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Rotate(Type _angle) {
        mExtents.Scale(_angle);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Scale(Type _scale) {
        mExtents.Scale(_scale);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Scale(Type _x, Type _y) {
        mExtents.Scale(_x, _y);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Scale(const WNVector3<Type>& _scale) {
        mExtents.Scale(_scale);
    }

    template <typename Type>
    Type WNRectangle<Type>::Length() const {
        return(WNAbs(static_cast<Type>(2) * mExtents.mX));
    }

    template <typename Type>
    Type WNRectangle<Type>::Width() const {
        return(WNAbs(static_cast<Type>(2) * mExtents.mY));
    }

    template <typename Type>
    Type WNRectangle<Type>::Perimeter() const {
        return(static_cast<Type>(4) * mExtents.mX + static_cast<Type>(4) * mExtents.mY);
    }

    template <typename Type>
    Type WNRectangle<Type>::Area() const {
        const WNVector3<Type> dimensions = mExtents * static_cast<Type>(2);

        return(dimensions.mX * dimensions.mY);
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::IsZero() const {
        return(mLocation.IsZero() && mExtents.IsZero() && mRotation == static_cast<Type>(0));
    }

    template <typename Type>
    WN_BOOL WNRectangle<Type>::IsInsideOut() const {
        return(mExtents.mX < static_cast<Type>(0) || mExtents.mY < static_cast<Type>(0));
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Set(const Type* _numbers) {
        mLocation.mX = _numbers[0];
        mLocation.mY = _numbers[1];
        mExtents.mX = _numbers[2];
        mExtents.mY = _numbers[3];
        mRotation = _numbers[4];
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Set(const WNVector3<Type>& _location, WNVector3<Type>& _extents, Type _rotation) {
        mLocation = _location;
        mExtents = _extents;
        mRotation = _rotation;
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::Set(Type _x1, Type _y1, Type _x2, Type _y2, Type _rotation) {
        mLocation.mX = _x1;
        mLocation.mY = _y1;
        mExtents.mX = _x2;
        mExtents.mY = _y2;
        mRotation = _rotation;
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetExpanded(Type _amount) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Expand(_amount), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Expand(_vector, _anchor), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetExpanded(const WNBounds2<Type>& _bounds, WN_BOOL _anchor) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Expand(_bounds, _anchor), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetExpanded(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Expand(_rectangle, _anchor), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetExpanded(const WNCircle<Type>& _circle, WN_BOOL _anchor) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Expand(_circle, _anchor), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetTranslated(Type _x, Type _y) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Translate(_x, _y), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetTranslated(const WNVector3<Type>& _vector) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Translate(_vector), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetRotated(Type _angle) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Rotate(_angle), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetScaled(Type _scale) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Scale(_scale), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetScaled(Type _x, Type _y) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Scale(_x, _y), rectangle);
    }

    template <typename Type>
    WNRectangle<Type> WNRectangle<Type>::GetScaled(const WNVector3<Type>& _scale) const {
        WNRectangle<Type> rectangle = *this;

        return(rectangle.Scale(_scale), rectangle);
    }

    template <typename Type>
    WN_VOID WNRectangle<Type>::GetPoints(WNVector3<Type>* _vectors) const {
        const WNVector3<Type> transformedExtents = mExtents.GetRotated(mRotation);
        const WNVector3<Type> transformedFlippedXExtents = (mExtents * WNVector3<Type>(static_cast<Type>(-1), static_cast<Type>(1))).GetRotated(mRotation);
        const WNVector3<Type> transformedFlippedYExtents = (mExtents * WNVector3<Type>(static_cast<Type>(1), -static_cast<Type>(-1))).GetRotated(mRotation);

        _vectors[0] = mLocation + transformedExtents;
        _vectors[1] = mLocation + transformedFlippedXExtents;
        _vectors[2] = mLocation + transformedFlippedYExtents;
        _vectors[3] = mLocation - transformedExtents;
    }
}

#endif // __WN_MATH_RECTANGLE_INL__
