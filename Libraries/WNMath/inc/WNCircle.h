// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_CIRCLE_H__
#define __WN_MATH_CIRCLE_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNBounds2;
    template <typename Type> class WNRectangle;
    template <typename Type> class WNVector2;

    template <typename Type>
    class WNCircle {
    public:
        WNCircle();
        explicit WNCircle(const Type* _numbers);
        explicit WNCircle(const WNVector2<Type>& _location, Type _radius);
        explicit WNCircle(Type _x, Type _y, Type _radius);

        WN_BOOL operator == (const WNCircle& _circle) const;
        WN_BOOL operator != (const WNCircle& _circle) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector2<Type>& _vector, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBounds2<Type>& _bounds, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNCircle& _circle, WN_BOOL _anchor = WN_FALSE);
        WN_VOID Translate(const WNVector2<Type>& _vector);

        Type Circumference() const;
        Type Area() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector2<Type>& _location, Type _radius);
        WN_VOID Set(Type _x, Type _y, Type _radius);

        WNCircle GetExpanded(Type _amount) const;
        WNCircle GetExpanded(const WNVector2<Type>& _vector, WN_BOOL _anchor = WN_FALSE) const;
        WNCircle GetExpanded(const WNRectangle<Type>& _rectangle, WN_BOOL _anchor = WN_FALSE) const;
        WNCircle GetExpanded(const WNBounds2<Type>& _bounds, WN_BOOL _anchor = WN_FALSE) const;
        WNCircle GetExpanded(const WNCircle& _circle, WN_BOOL _anchor = WN_FALSE) const;
        WNCircle GetTranslated(const WNVector2<Type>& _vector) const;

    public:
        WNVector2<Type> mLocation;
        Type mRadius;
    };
}

#include "WNMath/inc/Internal/WNCircle.inl"

#endif // __WN_MATH_CIRCLE_H__