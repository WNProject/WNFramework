// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_QUATERNION_H__
#define __WN_MATH_QUATERNION_H__

#include "WNMath/inc/Internal/WNQuaternionBase.h"

namespace WNMath {
    template <typename Type>
    class WNMatrix3;

    template <typename Type>
    class WNMatrix4;

    template <typename Type>
    class WNQuaternion : public __WNInternal::__WNQuaternionBase<WNQuaternion<Type>, Type> {
    public:
        WNQuaternion() {}
        WNQuaternion(const WNQuaternion& _quaternion);
        explicit WNQuaternion(const Type& _number);
        explicit WNQuaternion(const Type* _number);
        explicit WNQuaternion(const Type& _x, const Type& _y, const Type& _z, const Type& _w);

        WNMatrix3<Type> ToMatrix3() const;
        WNMatrix4<Type> ToMatrix4() const;

        template <typename NewType>
        WNQuaternion<NewType> ConvertTo() const;

    private:
        typedef class __WNInternal::__WNQuaternionBase<WNQuaternion<Type>, Type> Base;

    };

    template <typename Type>
    WNQuaternion<Type> operator * (const Type& _number, const WNQuaternion<Type>& _quaternion);

    template <typename Type>
    WNQuaternion<Type> operator / (const Type& _number, const WNQuaternion<Type>& _quaternion);
}

#include "WNMath/inc/Internal/WNQuaternion.inl"

#endif // __WN_MATH_QUATERNION_H__