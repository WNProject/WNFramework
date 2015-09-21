// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOX_H__
#define __WN_MATH_BOX_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNVector.h"
#include "WNMath/inc/WNQuaternion.h"

namespace wn {
    template <typename type> class WNBounds3;
    template <typename type> class sphere;

    template <typename type>
    class WNBox {
    public:
        WNBox();
        explicit WNBox(const type* _numbers);
        explicit WNBox(const vector3<type>& _location, const vector3<type>& _extents, const WNQuaternion<type>& _rotation);
        explicit WNBox(const vector3<type>& _vector);

        wn_bool operator == (const WNBox& _rectangle) const;
        wn_bool operator != (const WNBox& _rectangle) const;

        wn_void Zero();
        wn_void Expand(type _amount);
        wn_bool Expand(const vector3<type>& _vector, wn_bool _anchor = wn_false);
        wn_bool Expand(const sphere<type>& _sphere, wn_bool _anchor = wn_false);
        wn_bool Expand(const WNBounds3<type>& _bounds, wn_bool _anchor = wn_false);
        wn_bool Expand(const WNBox& _box, wn_bool _anchor = wn_false);
        wn_void Translate(type _x, type _y, type _z);
        wn_void Translate(const vector3<type>& _vector);
        wn_void Rotate(const WNQuaternion<type>& _rotation);
        wn_void Scale(type _scale);
        wn_void Scale(type _x, type _y, type _z);
        wn_void Scale(const vector3<type>& _scale);

        type Length() const;
        type Width() const;
        type Height() const;
        type SurfaceArea() const;
        type Volume() const;

        wn_bool IsZero() const;
        wn_bool IsInsideOut() const;

        wn_void Set(const type* _numbers);
        wn_void Set(const vector3<type>& _location, vector3<type>& _extents, const WNQuaternion<type>& _rotation);
        wn_void Set(const vector3<type>& _vector);

        WNBox GetExpanded(type _amount) const;
        WNBox GetExpanded(const vector3<type>& _vector, wn_bool _anchor = wn_false) const;
        WNBox GetExpanded(const sphere<type>& _sphere, wn_bool _anchor = wn_false) const;
        WNBox GetExpanded(const WNBounds3<type>& _bounds, wn_bool _anchor = wn_false) const;
        WNBox GetExpanded(const WNBox& _box, wn_bool _anchor = wn_false) const;
        WNBox GetTranslated(type _x, type _y, type _z);
        WNBox GetTranslated(const vector3<type>& _vector) const;
        WNBox GetRotated(const WNQuaternion<type>& _rotation) const;
        WNBox GetScaled(type _scale) const;
        WNBox GetScaled(type _x, type _y, type _z) const;
        WNBox GetScaled(const vector3<type>& _scale) const;

        wn_void GetPoints(vector3<type>* _vectors) const;

    public:
        vector3<type> mLocation;
        vector3<type> mExtents;
        WNQuaternion<type> mRotation;
    };
}

#include "WNMath/inc/Internal/WNBox.inl"

#endif // __WN_MATH_BOX_H__