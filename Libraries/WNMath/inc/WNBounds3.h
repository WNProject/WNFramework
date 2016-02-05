// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BOUNDS_3_H__
#define __WN_MATH_BOUNDS_3_H__

#include "WNMath/inc/WNVector.h"

namespace wn {
    template <typename type> class WNBox;
    template <typename type> class sphere;
    template <typename type> class vector3;

    template <typename type>
    class WNBounds3 {
    public:
        WNBounds3();
        explicit WNBounds3(const type* _numbers);
        explicit WNBounds3(const vector3<type>& _vector);
        explicit WNBounds3(const vector3<type>& _min, const vector3<type>& _max);

        bool operator == (const WNBounds3& _bounds) const;
        bool operator != (const WNBounds3& _bounds) const;

        void Zero();
        void Expand(type _amount);
        bool Expand(const vector3<type>& _vector);
        bool Expand(const WNBox<type>& _box);
        bool Expand(const sphere<type>& _sphere);
        bool Expand(const WNBounds3& _bounds);
        void Translate(const vector3<type>& _vector);

        type Length() const;
        type Width() const;
        type Height() const;
        type Radius() const;
        type SurfaceArea() const;
        type Volume() const;

        bool IsZero() const;
        bool IsInsideOut() const;

        WNBounds3 GetExpanded(type _amount) const;
        WNBounds3 GetExpanded(const vector3<type>& _vector) const;
        WNBounds3 GetExpanded(const WNBox<type>& _box) const;
        WNBounds3 GetExpanded(const sphere<type>& _sphere) const;
        WNBounds3 GetExpanded(const WNBounds3& _bounds) const;
        WNBounds3 GetTranslated(const vector3<type>& _vector) const;

        void GetPoints(vector3<type>* _vectors) const;

        WNBox<type> ToBox() const;

    public:
        vector3<type> mMinimum;
        vector3<type> mMaximum;
    };
}

#include "WNMath/inc/Internal/WNBounds3.inl"

#endif // __WN_MATH_BOUNDS_3_H__