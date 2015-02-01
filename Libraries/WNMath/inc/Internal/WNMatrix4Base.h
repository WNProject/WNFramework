// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_4_BASE_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace WNMath {
    template <typename Type>
    class WNVector3;

    template <typename Type>
    class WNVector4;

    template <typename Type>
    class WNMatrix4;

    template <typename Type>
    class WNQuaternion;

    namespace __WNInternal {
        template <typename Type, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNMatrix4Base : public __WNMatrixBase<WNMatrix4<Type>, Type, 4> {
        protected:
            WN_FORCE_INLINE __WNMatrix4Base() {}
        };

        template <typename Type>
        class __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value> : public __WNMatrixBase<WNMatrix4<Type>, Type, 4> {
        public:
            WNVector3<Type> TransformVector(const WNVector3<Type>& _vector) const;
            WNVector3<Type> TransformVector(const WNVector3<Type>& _vector, Type& _wComponent) const;
            WNVector4<Type> TransformVector(const WNVector4<Type>& _vector) const;
            WNVector3<Type>& TransformVectorInPlace(WNVector3<Type>& _vector) const;
            WNVector3<Type>& TransformVectorInPlace(WNVector3<Type>& _vector, Type& _wComponent) const;
            WNVector4<Type>& TransformVectorInPlace(WNVector4<Type>& _vector) const;

            WN_VOID MakeRotation(const WNQuaternion<Type>& _rotation);
            WN_VOID MakeRotationX(const Type& _angle);
            WN_VOID MakeRotationY(const Type& _angle);
            WN_VOID MakeRotationZ(const Type& _angle);
            WN_VOID MakeRotationAxis(const Type& _angle, const WNVector3<Type>& _axis);
            WN_VOID MakeRotationAxis(const Type& _angle, const Type& _x, const Type& _y, const Type& _z);

        public:
            static WNMatrix4<Type> CreateRotation(const WNQuaternion<Type>& _rotation);
            static WNMatrix4<Type> CreateRotationX(const Type& _angle);
            static WNMatrix4<Type> CreateRotationY(const Type& _angle);
            static WNMatrix4<Type> CreateRotationZ(const Type& _angle);
            static WNMatrix4<Type> CreateRotationAxis(const Type& _angle, const WNVector3<Type>& _axis);
            static WNMatrix4<Type> CreateRotationAxis(const Type& _angle, const Type& _x, const Type& _y, const Type& _z);
            static WNMatrix4<Type> CreatePerspective(const Type& _fov, const Type& _aspect, const Type& _near, const Type& _far);
            static WNMatrix4<Type> CreateOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far);
            static WNMatrix4<Type> CreateView(const WNVector3<Type>& _position, const WNVector3<Type>& _target, const WNVector3<Type>& _up);

        protected:
            WN_FORCE_INLINE __WNMatrix4Base() {}
        };
    }
}

#include "WNMath/inc/Internal/WNMatrix4Base.inl"

#endif // __WN_MATH_INTERNAL_MATRIX_4_BASE_H__