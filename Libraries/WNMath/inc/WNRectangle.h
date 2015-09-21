// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_RECTANGLE_H__
#define __WN_MATH_RECTANGLE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/vector3.h"

namespace wn {
    template <typename type> class WNBounds2;
    template <typename type> class circle;

    template <typename type>
    class WNRectangle {
    public:
        WNRectangle();
        explicit WNRectangle(const type* _numbers);
        explicit WNRectangle(const vector3<type>& _location, const vector3<type>& _extents, type _rotation);
        explicit WNRectangle(type _x1, type _y1, type _x2, type _y2, type _rotation);

        wn_bool operator == (const WNRectangle& _rectangle) const;
        wn_bool operator != (const WNRectangle& _rectangle) const;

        wn_void Zero();
        wn_void Expand(type _amount);
        wn_bool Expand(const vector3<type>& _vector, wn_bool _anchor = wn_false);
        wn_bool Expand(const WNBounds2<type>& _bounds, wn_bool _anchor = wn_false);
        wn_bool Expand(const WNRectangle& _rectangle, wn_bool _anchor = wn_false);
        wn_bool Expand(const circle<type>& _circle, wn_bool _anchor = wn_false);
        wn_void Translate(type _x, type _y);
        wn_void Translate(const vector3<type>& _vector);
        wn_void Rotate(type _angle);
        wn_void Scale(type _scale);
        wn_void Scale(type _x, type _y);
        wn_void Scale(const vector3<type>& _scale);

        type Length() const;
        type Width() const;
        type Perimeter() const;
        type Area() const;

        wn_bool IsZero() const;
        wn_bool IsInsideOut() const;

        wn_void Set(const type* _f);
        wn_void Set(const vector3<type>& _location, vector3<type>& _extents, type _rotation);
        wn_void Set(type _x1, type _y1, type _x2, type _y2, type _rotation);

        WNRectangle GetExpanded(type _amount) const;
        WNRectangle GetExpanded(const vector3<type>& _vector, wn_bool _anchor = wn_false) const;
        WNRectangle GetExpanded(const WNBounds2<type>& _bounds, wn_bool _anchor = wn_false) const;
        WNRectangle GetExpanded(const circle<type>& _circle, wn_bool _anchor = wn_false) const;
        WNRectangle GetExpanded(const WNRectangle& _rectangle, wn_bool _anchor = wn_false) const;
        WNRectangle GetTranslated(type _x, type _y) const;
        WNRectangle GetTranslated(const vector3<type>& _vector) const;
        WNRectangle GetRotated(type _angle) const;
        WNRectangle GetScaled(type _scale) const;
        WNRectangle GetScaled(type _x, type _y) const;
        WNRectangle GetScaled(const vector3<type>& _scale) const;
        wn_void GetPoints(vector3<type>* _vectors) const;

    public:
        vector3<type> mLocation;
        vector3<type> mExtents;
        type mRotation;
    };
}

#include "WNMath/inc/Internal/WNRectangle.inl"

#endif // __WN_MATH_RECTANGLE_H__