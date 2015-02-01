// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_SPHERE_H__
#define __WN_MATH_SPHERE_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNBounds3;
    template <typename Type> class WNBox;
    template <typename Type> class WNVector3;

    template <typename Type>
    class WNSphere {
    public:
        WNSphere();
        explicit WNSphere(const Type* _numbers);
        explicit WNSphere(const WNVector3<Type>& _location, Type _radius);
        explicit WNSphere(Type _x, Type _y, Type _z, Type _radius);

        WN_BOOL operator == (const WNSphere& _sphere) const;
        WN_BOOL operator != (const WNSphere& _sphere) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBox<Type>& _box, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBounds3<Type>& _bounds, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNSphere& _sphere, WN_BOOL _anchor = WN_FALSE);
        WN_VOID Translate(const WNVector3<Type>& _vector);

        Type Volume() const;
        Type SurfaceArea() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector3<Type>& _location, Type _radius);
        WN_VOID Set(Type _x, Type _y, Type _z, Type _radius);

        WNSphere GetExpanded(Type _amount) const;
        WNSphere GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE) const;
        WNSphere GetExpanded(const WNBox<Type>& _box, WN_BOOL _anchor = WN_FALSE) const;
        WNSphere GetExpanded(const WNBounds3<Type>& _bounds, WN_BOOL _anchor = WN_FALSE) const;
        WNSphere GetExpanded(const WNSphere& _circle, WN_BOOL _anchor = WN_FALSE) const;
        WNSphere GetTranslated(const WNVector3<Type>& _vector) const;

    public:
        WNVector3<Type> mLocation;
        Type mRadius;
    };
}

#include "WNMath/inc/Internal/WNSphere.inl"

#endif // __WN_MATH_SPHERE_H__