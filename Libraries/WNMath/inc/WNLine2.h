// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_LINE_2_H__
#define __WN_MATH_LINE_2_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    template <typename type> class vector2;

    template <typename type>
    class WNLine2 {
    public:
        WNLine2();
        WNLine2(const WNLine2& _line);
        explicit WNLine2(const type* _numbers);
        explicit WNLine2(const vector2<type>& _point1, const vector2<type>& _point2);

        WNLine2 operator + () const;
        WNLine2 operator - () const;

        wn_bool operator == (const WNLine2& _line) const;
        wn_bool operator != (const WNLine2& _line) const;

        wn_void Zero();

        vector2<type> Direction() const;

        wn_void Set(const WNLine2& _line);
        wn_void Set(const type* _numbers);
        wn_void Set(const vector2<type>& _point1, const vector2<type>& _point2);

    public:
        vector2<type> mPoint1;
        vector2<type> mPoint2;
    };
}

#include "WNMath/inc/Internal/WNLine2.inl"

#endif // __WN_MATH_LINE_2_H__