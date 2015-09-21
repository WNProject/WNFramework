// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_QUATERNION_H__
#define __WN_MATH_QUATERNION_H__

#include "WNMath/inc/Internal/WNQuaternionBase.h"

namespace wn {
    template <typename type>
    class WNMatrix3;

    template <typename type>
    class WNMatrix4;

    template <typename type>
    class WNQuaternion : public internal::__WNQuaternionBase<WNQuaternion<type>, type> {
    public:
        WNQuaternion() {}
        WNQuaternion(const WNQuaternion& _quaternion);
        explicit WNQuaternion(const type& _number);
        explicit WNQuaternion(const type* _number);
        explicit WNQuaternion(const type& _x, const type& _y, const type& _z, const type& _w);

        WNMatrix3<type> ToMatrix3() const;
        WNMatrix4<type> ToMatrix4() const;

        template <typename NewType>
        WNQuaternion<NewType> ConvertTo() const;

    private:
        typedef class internal::__WNQuaternionBase<WNQuaternion<type>, type> Base;

    };

    template <typename type>
    WNQuaternion<type> operator * (const type& _number, const WNQuaternion<type>& _quaternion);

    template <typename type>
    WNQuaternion<type> operator / (const type& _number, const WNQuaternion<type>& _quaternion);
}

#include "WNMath/inc/Internal/WNQuaternion.inl"

#endif // __WN_MATH_QUATERNION_H__