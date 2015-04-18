// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_QUATERNION_INL__
#define __WN_MATH_QUATERNION_INL__

#ifndef __WN_MATH_QUATERNION_H__
    #error "WNQuaternion.inl should never be included directly. Please include WNQuaternion.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNMatrix3.h"
#include "WNMath/inc/WNMatrix4.h"
#include "WNMath/inc/WNVector4.h"

namespace wn {
    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type>::WNQuaternion(const WNQuaternion<type>& _quaternion) {
        Base::Set(_quaternion);
    }

    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type>::WNQuaternion(const type& _number) {
        Base::Set(_number);
    }

    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type>::WNQuaternion(const type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type>::WNQuaternion(const type& _x, const type& _y, const type& _z, const type& _w) {
        Base::Set(_x, _y, _z, _w);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> WNQuaternion<type>::ToMatrix3() const {
        const type x2 = this->mElements[0] + this->mElements[0];
        const type y2 = this->mElements[1] + this->mElements[1];
        const type z2 = this->mElements[2] + this->mElements[2];
        const type wx = this->mElements[3] * x2;
        const type wy = this->mElements[3] * y2;
        const type wz = this->mElements[3] * z2;
        const type xx = this->mElements[0] * x2;
        const type yy = this->mElements[1] * y2;
        const type yz = this->mElements[1] * z2;
        const type xy = this->mElements[0] * y2;
        const type xz = this->mElements[0] * z2;
        const type zz = this->mElements[2] * z2;

        return(WNMatrix3<type>(static_cast<type>(1) - (yy + zz), xy - wz, xz + wy,
                               xy + wz, static_cast<type>(1) - (xx + zz), yz - wx,
                               xz - wy, yz + wx, static_cast<type>(1) - (xx + yy)));
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix4<type> WNQuaternion<type>::ToMatrix4() const {
        const type x2 = this->mElements[0] + this->mElements[0];
        const type y2 = this->mElements[1] + this->mElements[1];
        const type z2 = this->mElements[2] + this->mElements[2];
        const type wx = this->mElements[3] * x2;
        const type wy = this->mElements[3] * y2;
        const type wz = this->mElements[3] * z2;
        const type xx = this->mElements[0] * x2;
        const type yy = this->mElements[1] * y2;
        const type yz = this->mElements[1] * z2;
        const type xy = this->mElements[0] * y2;
        const type xz = this->mElements[0] * z2;
        const type zz = this->mElements[2] * z2;

        return(WNMatrix4<type>(static_cast<type>(1) - (yy + zz), xy - wz, xz + wy, static_cast<type>(0),
                               xy + wz, static_cast<type>(1) - (xx + zz), yz - wx, static_cast<type>(0),
                               xz - wy, yz + wx, static_cast<type>(1) - (xx + yy), static_cast<type>(0),
                               static_cast<type>(0), static_cast<type>(0), static_cast<type>(0), static_cast<type>(1)));
    }

    template <typename type>
    template <typename NewType>
    WN_FORCE_INLINE WNQuaternion<NewType> WNQuaternion<type>::ConvertTo() const {
        return(WNQuaternion<NewType>(static_cast<NewType>(this->mElements[0]), static_cast<NewType>(this->mElements[1]),
                                     static_cast<NewType>(this->mElements[2]), static_cast<NewType>(this->mElements[3])));
    }

    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type> operator * (const type& _number, const WNQuaternion<type>& _quaternion) {
        WNQuaternion<type> quaternion = _quaternion;

        quaternion *= _number;

        return(quaternion);
    }

    template <typename type>
    WN_FORCE_INLINE WNQuaternion<type> operator / (const type& _number, const WNQuaternion<type>& _quaternion) {
        WNQuaternion<type> quaternion = WNQuaternion<type>(_number);

        quaternion /= _quaternion;

        return(quaternion);
    }
}

#endif // __WN_MATH_QUATERNION_2_INL__