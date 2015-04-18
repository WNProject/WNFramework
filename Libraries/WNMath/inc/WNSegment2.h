// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_SEGMENT_2_H__
#define __WN_MATH_SEGMENT_2_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    template <typename type> class WNLine2;
    template <typename type> class WNLine3;
    template <typename type> class WNRay2;
    template <typename type> class WNRay3;
    template <typename type> class vector2;

    template <typename type>
    class WNSegment2 {
    public:
        WNSegment2();
        explicit WNSegment2(const type* _numbers);
        explicit WNSegment2(const vector2<type>& _start, const vector2<type>& _end);

        WNSegment2 operator + () const;
        WNSegment2 operator - () const;

        wn_bool operator == (const WNSegment2& _segment) const;
        wn_bool operator != (const WNSegment2& _segment) const;

        wn_void Zero();

        type Length() const;
        type LengthSquared() const;
        vector2<type> Direction() const;

        wn_bool IsZero() const;

        wn_void Set(const type* _numbers);
        wn_void Set(const vector2<type>& _start, const vector2<type>& _end);

        WNLine2<type> ToLine2() const;
        WNLine3<type> ToLine3() const;
        WNRay2<type> ToRay2() const;
        WNRay3<type> ToRay3() const;

    public:
        vector2<type> mStart;
        vector2<type> mEnd;
    };
}

#include "WNMath/inc/Internal/WNSegment2.inl"

#endif // __WN_MATH_SEGMENT_2_H__