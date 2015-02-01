// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_BOX_H__
#define __WN_MATH_BOX_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNVector3.h"
#include "WNMath/inc/WNQuaternion.h"

namespace WNMath {
    template <typename Type> class WNBounds3;
    template <typename Type> class WNSphere;

    template <typename Type>
    class WNBox {
    public:
        WNBox();
        explicit WNBox(const Type* _numbers);
        explicit WNBox(const WNVector3<Type>& _location, const WNVector3<Type>& _extents, const WNQuaternion<Type>& _rotation);
        explicit WNBox(const WNVector3<Type>& _vector);

        WN_BOOL operator == (const WNBox& _rectangle) const;
        WN_BOOL operator != (const WNBox& _rectangle) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNSphere<Type>& _sphere, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBounds3<Type>& _bounds, WN_BOOL _anchor = WN_FALSE);
        WN_BOOL Expand(const WNBox& _box, WN_BOOL _anchor = WN_FALSE);
        WN_VOID Translate(Type _x, Type _y, Type _z);
        WN_VOID Translate(const WNVector3<Type>& _vector);
        WN_VOID Rotate(const WNQuaternion<Type>& _rotation);
        WN_VOID Scale(Type _scale);
        WN_VOID Scale(Type _x, Type _y, Type _z);
        WN_VOID Scale(const WNVector3<Type>& _scale);
        
        Type Length() const;
        Type Width() const;
        Type Height() const;
        Type SurfaceArea() const;
        Type Volume() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector3<Type>& _location, WNVector3<Type>& _extents, const WNQuaternion<Type>& _rotation);
        WN_VOID Set(const WNVector3<Type>& _vector);

        WNBox GetExpanded(Type _amount) const;
        WNBox GetExpanded(const WNVector3<Type>& _vector, WN_BOOL _anchor = WN_FALSE) const;
        WNBox GetExpanded(const WNSphere<Type>& _sphere, WN_BOOL _anchor = WN_FALSE) const;
        WNBox GetExpanded(const WNBounds3<Type>& _bounds, WN_BOOL _anchor = WN_FALSE) const;
        WNBox GetExpanded(const WNBox& _box, WN_BOOL _anchor = WN_FALSE) const;
        WNBox GetTranslated(Type _x, Type _y, Type _z);
        WNBox GetTranslated(const WNVector3<Type>& _vector) const;
        WNBox GetRotated(const WNQuaternion<Type>& _rotation) const;
        WNBox GetScaled(Type _scale) const;
        WNBox GetScaled(Type _x, Type _y, Type _z) const;
        WNBox GetScaled(const WNVector3<Type>& _scale) const;

        WN_VOID GetPoints(WNVector3<Type>* _vectors) const;

    public:
        WNVector3<Type> mLocation;
        WNVector3<Type> mExtents;
        WNQuaternion<Type> mRotation;
    };
}

#include "WNMath/inc/Internal/WNBox.inl"

#endif // __WN_MATH_BOX_H__