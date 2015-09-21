// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_LINE_3_H__
#define __WN_MATH_LINE_3_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    template <typename type> class vector3;

    template <typename type>
    class WNLine3 {
    public:
        WNLine3();
        WNLine3(const WNLine3& _line);
        explicit WNLine3(const type* _numbers);
        explicit WNLine3(const vector3<type>& _point1, const vector3<type>& _point2);

        WNLine3 operator + () const;
        WNLine3 operator - () const;

        wn_bool operator == (const WNLine3& _line) const;
        wn_bool operator != (const WNLine3& _line) const;

        wn_void Zero();

        vector3<type> Direction() const;

        wn_void Set(const WNLine3& _line);
        wn_void Set(const type* _numbers);
        wn_void Set(const vector3<type>& _point1, const vector3<type>& _point2);

    public:
        vector3<type> mPoint1;
        vector3<type> mPoint2;
    };
}

#include "WNMath/inc/Internal/WNLine3.inl"

#endif // __WN_MATH_LINE_3_H__