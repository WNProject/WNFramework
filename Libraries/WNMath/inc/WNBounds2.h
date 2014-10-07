////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_BOUNDS_2_H__
#define __WN_MATH_BOUNDS_2_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNRectangle;
    template <typename Type> class WNCircle;
    template <typename Type> class WNVector2;

    template <typename Type>
    class WNBounds2 {
    public:
        WNBounds2();
        explicit WNBounds2(const Type* _numbers);
        explicit WNBounds2(const WNVector2<Type>& _vector);
        explicit WNBounds2(const WNVector2<Type>& _min, const WNVector2<Type>& _max);

        WN_BOOL operator == (const WNBounds2& _bounds) const;
        WN_BOOL operator != (const WNBounds2& _bounds) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector2<Type>& _vector);
        WN_BOOL Expand(const WNRectangle<Type>& _rectangle);
        WN_BOOL Expand(const WNCircle<Type>& _circle);
        WN_BOOL Expand(const WNBounds2& _bounds);
        WN_VOID Translate(const WNVector2<Type>& _vector);

        Type Length() const;
        Type Width() const;
        Type Perimeter() const;
        Type Radius() const;
        Type Area() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector2<Type>& _vector);
        WN_VOID Set(const WNVector2<Type>& _min, const WNVector2<Type>& _max);

        WNBounds2 GetExpanded(Type _amount) const;
        WNBounds2 GetExpanded(const WNVector2<Type>& _vector) const;
        WNBounds2 GetExpanded(const WNRectangle<Type>& _rectangle) const;
        WNBounds2 GetExpanded(const WNCircle<Type>& _circle) const;
        WNBounds2 GetExpanded(const WNBounds2& _bounds) const;
        WNBounds2 GetTranslated(const WNVector2<Type>& _vector) const;

        WN_VOID GetPoints(WNVector2<Type>* _vectors) const;

        WNRectangle<Type> ToRectangle() const;

    public:
        WNVector2<Type> mMinimum;
        WNVector2<Type> mMaximum;
    };
}

#include "WNMath/inc/Internal/WNBounds2.inl"

#endif // __WN_MATH_BOUNDS_2_H__