// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_BOUNDS_2_H__
#define __WN_MATH_BOUNDS_2_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    template <typename type> class WNRectangle;
    template <typename type> class circle;
    template <typename type> class vector2;

    template <typename type>
    class WNBounds2 {
    public:
        WNBounds2();
        explicit WNBounds2(const type* _numbers);
        explicit WNBounds2(const vector2<type>& _vector);
        explicit WNBounds2(const vector2<type>& _min, const vector2<type>& _max);

        wn_bool operator == (const WNBounds2& _bounds) const;
        wn_bool operator != (const WNBounds2& _bounds) const;

        wn_void Zero();
        wn_void Expand(type _amount);
        wn_bool Expand(const vector2<type>& _vector);
        wn_bool Expand(const WNRectangle<type>& _rectangle);
        wn_bool Expand(const circle<type>& _circle);
        wn_bool Expand(const WNBounds2& _bounds);
        wn_void Translate(const vector2<type>& _vector);

        type Length() const;
        type Width() const;
        type Perimeter() const;
        type Radius() const;
        type Area() const;

        wn_bool IsZero() const;
        wn_bool IsInsideOut() const;

        wn_void Set(const type* _numbers);
        wn_void Set(const vector2<type>& _vector);
        wn_void Set(const vector2<type>& _min, const vector2<type>& _max);

        WNBounds2 GetExpanded(type _amount) const;
        WNBounds2 GetExpanded(const vector2<type>& _vector) const;
        WNBounds2 GetExpanded(const WNRectangle<type>& _rectangle) const;
        WNBounds2 GetExpanded(const circle<type>& _circle) const;
        WNBounds2 GetExpanded(const WNBounds2& _bounds) const;
        WNBounds2 GetTranslated(const vector2<type>& _vector) const;

        wn_void GetPoints(vector2<type>* _vectors) const;

        WNRectangle<type> ToRectangle() const;

    public:
        vector2<type> mMinimum;
        vector2<type> mMaximum;
    };
}

#include "WNMath/inc/Internal/WNBounds2.inl"

#endif // __WN_MATH_BOUNDS_2_H__