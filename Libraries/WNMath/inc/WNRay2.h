// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_RAY_2_H__
#define __WN_MATH_RAY_2_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNVector2.h"

namespace WNMath {
    template <typename Type> class WNLine2;
    template <typename Type> class WNLine3;

    template <typename Type>
    class WNRay2 {
    public:
        WNRay2();
        explicit WNRay2(const Type* _numbers);
        explicit WNRay2(const WNVector2<Type>& _location, const WNVector2<Type>& _direction);

        WNRay2 operator + () const;
        WNRay2 operator - () const;

        WN_BOOL operator == (const WNRay2& _ray) const;
        WN_BOOL operator != (const WNRay2& _ray) const;

        WN_VOID Zero();

        WN_BOOL IsZero() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector2<Type>& _location, const WNVector2<Type>& _direction);

        WNLine2<Type> ToLine2() const;
        WNLine3<Type> ToLine3() const;

    public:
        WNVector2<Type> mLocation;
        WNVector2<Type> mDirection;
    };
}

#include "WNMath/inc/Internal/WNRay2.inl"

#endif // __WN_MATH_RAY_2_H__