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

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type>::WNQuaternion(const WNQuaternion<Type>& _quaternion) {
        Base::Set(_quaternion);
    }

    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type>::WNQuaternion(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type>::WNQuaternion(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type>::WNQuaternion(const Type& _x, const Type& _y, const Type& _z, const Type& _w) {
        Base::Set(_x, _y, _z, _w);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> WNQuaternion<Type>::ToMatrix3() const {
        const Type x2 = this->mElements[0] + this->mElements[0];
        const Type y2 = this->mElements[1] + this->mElements[1];
        const Type z2 = this->mElements[2] + this->mElements[2];
        const Type wx = this->mElements[3] * x2;
        const Type wy = this->mElements[3] * y2;
        const Type wz = this->mElements[3] * z2;
        const Type xx = this->mElements[0] * x2;
        const Type yy = this->mElements[1] * y2;
        const Type yz = this->mElements[1] * z2;
        const Type xy = this->mElements[0] * y2;
        const Type xz = this->mElements[0] * z2;
        const Type zz = this->mElements[2] * z2;

        return(WNMatrix3<Type>(static_cast<Type>(1) - (yy + zz), xy - wz, xz + wy,
                               xy + wz, static_cast<Type>(1) - (xx + zz), yz - wx,
                               xz - wy, yz + wx, static_cast<Type>(1) - (xx + yy)));
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNQuaternion<Type>::ToMatrix4() const {
        const Type x2 = this->mElements[0] + this->mElements[0];
        const Type y2 = this->mElements[1] + this->mElements[1];
        const Type z2 = this->mElements[2] + this->mElements[2];
        const Type wx = this->mElements[3] * x2;
        const Type wy = this->mElements[3] * y2;
        const Type wz = this->mElements[3] * z2;
        const Type xx = this->mElements[0] * x2;
        const Type yy = this->mElements[1] * y2;
        const Type yz = this->mElements[1] * z2;
        const Type xy = this->mElements[0] * y2;
        const Type xz = this->mElements[0] * z2;
        const Type zz = this->mElements[2] * z2;

        return(WNMatrix4<Type>(static_cast<Type>(1) - (yy + zz), xy - wz, xz + wy, static_cast<Type>(0),
                               xy + wz, static_cast<Type>(1) - (xx + zz), yz - wx, static_cast<Type>(0),
                               xz - wy, yz + wx, static_cast<Type>(1) - (xx + yy), static_cast<Type>(0),
                               static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)));
    }

    template <typename Type>
    template <typename NewType>
    WN_FORCE_INLINE WNQuaternion<NewType> WNQuaternion<Type>::ConvertTo() const {
        return(WNQuaternion<NewType>(static_cast<NewType>(this->mElements[0]), static_cast<NewType>(this->mElements[1]),
                                     static_cast<NewType>(this->mElements[2]), static_cast<NewType>(this->mElements[3])));
    }

    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type> operator * (const Type& _number, const WNQuaternion<Type>& _quaternion) {
        WNQuaternion<Type> quaternion = _quaternion;

        quaternion *= _number;

        return(quaternion);
    }

    template <typename Type>
    WN_FORCE_INLINE WNQuaternion<Type> operator / (const Type& _number, const WNQuaternion<Type>& _quaternion) {
        WNQuaternion<Type> quaternion = WNQuaternion<Type>(_number);

        quaternion /= _quaternion;

        return(quaternion);
    }
}

#endif // __WN_MATH_QUATERNION_2_INL__