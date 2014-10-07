////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_RECTANGLE_H__
#define __WN_MATH_RECTANGLE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type> class WNBounds2;
    template <typename Type> class WNCircle;

    template <typename Type>
    class WNRectangle {
    public:
        WNRectangle();
        explicit WNRectangle(const Type* _numbers);
        explicit WNRectangle(const WNVector3<Type>& _location, const WNVector3<Type>& _extents, Type _rotation);
        explicit WNRectangle(Type _x1, Type _y1, Type _x2, Type _y2, Type _rotation);

        WN_BOOL operator == (const WNRectangle& _rectangle) const;
        WN_BOOL operator != (const WNRectangle& _rectangle) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBounds2<Type>& _bounds, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNRectangle& _rectangle, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNCircle<Type>& _circle, WN_BOOL _anchor = WN_FALSE);
        WN_VOID Translate(Type _x, Type _y);
        WN_VOID Translate(const WNVector3<Type>& _vector);
        WN_VOID Rotate(Type _angle);
        WN_VOID Scale(Type _scale);
        WN_VOID Scale(Type _x, Type _y);
        WN_VOID Scale(const WNVector3<Type>& _scale);

        Type Length() const;
        Type Width() const;
        Type Perimeter() const;
        Type Area() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WN_VOID Set(const Type* _f);
        WN_VOID Set(const WNVector3<Type>& _location, WNVector3<Type>& _extents, Type _rotation);
        WN_VOID Set(Type _x1, Type _y1, Type _x2, Type _y2, Type _rotation);

        WNRectangle GetExpanded(Type _amount) const;
        WNRectangle GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE) const;
        WNRectangle GetExpanded(const WNBounds2<Type>& _bounds, WN_BOOL _anchor = WN_FALSE) const;
        WNRectangle GetExpanded(const WNCircle<Type>& _circle, WN_BOOL _anchor = WN_FALSE) const;
        WNRectangle GetExpanded(const WNRectangle& _rectangle, WN_BOOL _anchor = WN_FALSE) const;
        WNRectangle GetTranslated(Type _x, Type _y) const;
        WNRectangle GetTranslated(const WNVector3<Type>& _vector) const;
        WNRectangle GetRotated(Type _angle) const;
        WNRectangle GetScaled(Type _scale) const;
        WNRectangle GetScaled(Type _x, Type _y) const;
        WNRectangle GetScaled(const WNVector3<Type>& _scale) const;
        WN_VOID GetPoints(WNVector3<Type>* _vectors) const;

    public:
        WNVector3<Type> mLocation;
        WNVector3<Type> mExtents;
        Type mRotation;
    };
}

#include "WNMath/inc/Internal/WNRectangle.inl"

#endif // __WN_MATH_RECTANGLE_H__