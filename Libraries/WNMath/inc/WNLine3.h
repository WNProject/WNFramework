// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_LINE_3_H__
#define __WN_MATH_LINE_3_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNVector3;

    template <typename Type>
    class WNLine3 {
    public:
        WNLine3();
        WNLine3(const WNLine3& _line);
        explicit WNLine3(const Type* _numbers);
        explicit WNLine3(const WNVector3<Type>& _point1, const WNVector3<Type>& _point2);

        WNLine3 operator + () const;
        WNLine3 operator - () const;

        WN_BOOL operator == (const WNLine3& _line) const;
        WN_BOOL operator != (const WNLine3& _line) const;

        WN_VOID Zero();

        WNVector3<Type> Direction() const;

        WN_VOID Set(const WNLine3& _line);
        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector3<Type>& _point1, const WNVector3<Type>& _point2);

    public:
        WNVector3<Type> mPoint1;
        WNVector3<Type> mPoint2;
    };
}

#include "WNMath/inc/Internal/WNLine3.inl"

#endif // __WN_MATH_LINE_3_H__