// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_QUATERNION_BASE_INL__
#define __WN_MATH_INTERNAL_QUATERNION_BASE_INL__

#ifndef __WN_MATH_QUATERNION_H__
    #error "Internal/WNQuaternionBase.inl should never be included directly. Please include WNQuaternion.h instead"
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNGeneralOps.h"
#include "WNMath/inc/Internal/WNVectorOpLength.h"
#include "WNMath/inc/Internal/WNVectorOpNormalize.h"
#include "WNMath/inc/Internal/WNVectorOpLengthSquared.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE __WNQuaternionBaseCommon<QuaternionType, Type>::__WNQuaternionBaseCommon() {
            this->mElements = __WNGeneralOpInitialize<Type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE Type& __WNQuaternionBaseCommon<QuaternionType, Type>::operator [] (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE const Type& __WNQuaternionBaseCommon<QuaternionType, Type>::operator [] (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE Type& __WNQuaternionBaseCommon<QuaternionType, Type>::operator () (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE const Type& __WNQuaternionBaseCommon<QuaternionType, Type>::operator () (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator + () const {
            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator - () const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion.mElements = __WNGeneralOpNegate<Type, 4>::Execute(quaternion.mElements);

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, Type>::operator += (const QuaternionType& _quaternion) {
            this->mElements = __WNGeneralOpAdd<Type, 4>::Execute(this->mElements, _quaternion.mElements);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, Type>::operator -= (const QuaternionType& _quaternion) {
            this->mElements = __WNGeneralOpSubtract<Type, 4>::Execute(this->mElements, _quaternion.mElements);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, Type>::operator *= (const QuaternionType& _quaternion) {
            const QuaternionType quaternion = *static_cast<QuaternionType*>(this);

            this->mElements[0] = quaternion.mElements[3] * _quaternion.mElements[0] + quaternion.mElements[0] * _quaternion.mElements[3] +
                                 quaternion.mElements[1] * _quaternion.mElements[2] - quaternion.mElements[2] * _quaternion.mElements[1];
            this->mElements[1] = quaternion.mElements[3] * _quaternion.mElements[1] + quaternion.mElements[1] * _quaternion.mElements[3] +
                                 quaternion.mElements[2] * _quaternion.mElements[0] - quaternion.mElements[0] * _quaternion.mElements[2];
            this->mElements[2] = quaternion.mElements[3] * _quaternion.mElements[2] + quaternion.mElements[2] * _quaternion.mElements[3] +
                                 quaternion.mElements[0] * _quaternion.mElements[1] - quaternion.mElements[1] * _quaternion.mElements[0];
            this->mElements[3] = quaternion.mElements[3] * _quaternion.mElements[3] - quaternion.mElements[0] * _quaternion.mElements[0] -
                                 quaternion.mElements[1] * _quaternion.mElements[1] - quaternion.mElements[2] * _quaternion.mElements[2];

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, Type>::operator *= (const Type& _number) {
            this->mElements = __WNGeneralOpMultiplyNumber<Type, 4>::Execute(this->mElements, _number);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, Type>::operator /= (const Type& _number) {
            this->mElements = __WNGeneralOpDivideNumber<Type, 4>::Execute(this->mElements, _number);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator + (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion += _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator - (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion -= _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator * (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion *= _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator * (const Type& _number) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion *= _number;

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, Type>::operator / (const Type& _number) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion /= _number;

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_BOOL __WNQuaternionBaseCommon<QuaternionType, Type>::operator == (const QuaternionType& _quaternion) const {
            return(__WNGeneralOpEqual<Type, 4>::Execute(this->mElements, _quaternion.mElements));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_BOOL __WNQuaternionBaseCommon<QuaternionType, Type>::operator != (const QuaternionType& _quaternion) const {
            return(__WNGeneralOpNotEqual<Type, 4>::Execute(this->mElements, _quaternion.mElements));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Zero() {
            this->mElements = __WNGeneralOpZero<Type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Identity() {
            this->mElements[0] = static_cast<Type>(0);
            this->mElements[1] = static_cast<Type>(0);
            this->mElements[2] = static_cast<Type>(0);
            this->mElements[3] = static_cast<Type>(1);
        }

        template <typename QuaternionType, typename Type>
        Type __WNQuaternionBaseCommon<QuaternionType, Type>::Length() const {
             return(__WNVectorOpLength<Type, 4>::Execute(this->mElements));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE Type __WNQuaternionBaseCommon<QuaternionType, Type>::LengthSquared() const {
             return(__WNVectorOpLengthSquared<Type, 4>::Execute(this->mElements));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE Type __WNQuaternionBaseCommon<QuaternionType, Type>::Dot(const QuaternionType& _quaternion) const {
            const QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            return((quaternion.mElements[0] * _quaternion.mElements[0]) + (quaternion.mElements[1] * _quaternion.mElements[1]) +
                   (quaternion.mElements[2] * _quaternion.mElements[2]) + (quaternion.mElements[3] * _quaternion.mElements[3]));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_BOOL __WNQuaternionBaseCommon<QuaternionType, Type>::IsZero() const {
            return(this->mElements[0] == static_cast<Type>(0) && this->mElements[1] == static_cast<Type>(0) &&
                   this->mElements[2] == static_cast<Type>(0) && this->mElements[3] == static_cast<Type>(0));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_BOOL __WNQuaternionBaseCommon<QuaternionType, Type>::IsIdentity() const {
            return(this->mElements[0] == static_cast<Type>(0) && this->mElements[1] == static_cast<Type>(0) &&
                   this->mElements[2] == static_cast<Type>(0) && this->mElements[3] == static_cast<Type>(1));
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Set(const QuaternionType& _quaternion) {
            *static_cast<QuaternionType*>(this) = _quaternion;
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Set(const Type& _number) {
            this->mElements = __WNGeneralOpSetNumber<Type, 4>::Execute(this->mElements, _number);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Set(const Type* _numbers) {
            this->mElements = __WNGeneralOpSetNumbers<Type, 4>::Execute(this->mElements, _numbers);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBaseCommon<QuaternionType, Type>::Set(const Type& _x, const Type& _y, const Type& _z, const Type& _w) {
            this->mElements[0] = _x;
            this->mElements[1] = _y;
            this->mElements[2] = _z;
            this->mElements[3] = _w;
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Normalize() {
            this->mElements = __WNVectorOpNormalize<Type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Invert() {
            *this = GetConjugate();

            Normalize();
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Logarithm() {
            if (WNAbs(this->mElements[3]) <= static_cast<Type>(1)) {
                const Type theta = WNACos(this->mElements[3]);
                const Type thetaOverSinTheta  = theta / WNSin(theta);

                this->mElements[0] *= thetaOverSinTheta;
                this->mElements[1] *= thetaOverSinTheta;
                this->mElements[2] *= thetaOverSinTheta;
            }

            this->mElements[3] = static_cast<Type>(0);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Exponent() {
            const Type theta = WNSqrt(this->mElements[0] * this->mElements[0] +
                                      this->mElements[1] * this->mElements[1] +
                                      this->mElements[2] * this->mElements[2]);
            const Type cosTheta = WNCos(theta);

            if (WNAbs(theta) >= static_cast<Type>(0)) {
                const Type oneOverTheta = static_cast<Type>(1) / theta;
                const Type sinTheta = WNSin(theta);

                this->mElements[0] *= oneOverTheta * sinTheta;
                this->mElements[1] *= oneOverTheta * sinTheta;
                this->mElements[2] *= oneOverTheta * sinTheta;
            }

            this->mElements[3] = cosTheta;
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE WN_VOID __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Rotate(const QuaternionType& _rotation) {
            *this *= _rotation;
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetNormalized() const {
            QuaternionType quaternion = *this;

            return(quaternion.Normalize(), quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetInverted() const {
            QuaternionType quaternion = *this;

            return(quaternion.Invert(), quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetConjugate() const {
            QuaternionType quaternion = *this;

            quaternion.mElements[0] = -quaternion.mElements[0];
            quaternion.mElements[1] = -quaternion.mElements[1];
            quaternion.mElements[2] = -quaternion.mElements[2];

            return(quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetLogarithm() const {
            QuaternionType quaternion = *this;

            return(quaternion.Logarithm(), quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetExponent() const {
            QuaternionType quaternion = *this;

            return(quaternion.Exponent(), quaternion);
        }

        template <typename QuaternionType, typename Type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetRotated(const QuaternionType& _rotation) const {
            QuaternionType quaternion = *this;

            return(quaternion.Rotate(_rotation), quaternion);
        }
    }
}

#endif // __WN_MATH_INTERNAL_QUATERNION_BASE_INL__
