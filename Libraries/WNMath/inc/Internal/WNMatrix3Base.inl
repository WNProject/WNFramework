// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__

#ifndef __WN_MATH_MATRIX_3_H__
    #error "Internal/WNMatrix3Base.inl should never be included directly. Please include WNMatrix3.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNVector2.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type>
        WN_FORCE_INLINE WNVector2<Type> __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector2<Type>& _vector) const {
            WNVector2<Type> vector = _vector;

            return(this->TransformVectorInPlace(vector));
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector2<Type> __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector2<Type>& _vector, Type& _zComponent) const {
            WNVector3<Type> vector = _vector.ToVector3();

            this->TransformVectorInPlace(vector);

            _zComponent = vector.GetZ();

            return(vector.ToVector2());
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector3<Type> __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector3<Type>& _vector) const {
            WNVector3<Type> vector = _vector;

            return(this->TransformVectorInPlace(vector));
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotation(const Type& _angle) {
            const Type sinTheta = WNSin(_angle);
            const Type cosTheta = WNCos(_angle);

            // First Column
            this->mElements.mValues[0] = cosTheta;
            this->mElements.mValues[1] = sinTheta;
            this->mElements.mValues[2] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[3] = -sinTheta;
            this->mElements.mValues[4] = cosTheta;
            this->mElements.mValues[5] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[6] = static_cast<Type>(0);
            this->mElements.mValues[7] = static_cast<Type>(0);
            this->mElements.mValues[8] = static_cast<Type>(1);
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far) {
            // First Column
            this->mElements.mValues[0] = static_cast<Type>(2) / _width;
            this->mElements.mValues[1] = static_cast<Type>(0);
            this->mElements.mValues[2] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[3] = static_cast<Type>(0);
            this->mElements.mValues[4] = static_cast<Type>(-2) / _height;
            this->mElements.mValues[5] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[6] = static_cast<Type>(0);
            this->mElements.mValues[7] = static_cast<Type>(0);
            this->mElements.mValues[8] = static_cast<Type>(1) / (_near - _far);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix3<Type> __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotation(const Type& _angle) {
            WNMatrix3<Type> matrix;

            return(matrix.MakeRotation(_angle), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix3<Type> __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far) {
            WNMatrix3<Type> matrix;

            return(matrix.MakeOrthographic(_width, _height, _near, _far), matrix);
        }
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_3_BASE_INL__