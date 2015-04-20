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

namespace wn {
    namespace internal {
        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE __WNQuaternionBaseCommon<QuaternionType, type>::__WNQuaternionBaseCommon() {
            this->mElements = __WNGeneralOpInitialize<type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE type& __WNQuaternionBaseCommon<QuaternionType, type>::operator [] (const wn_uint32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE const type& __WNQuaternionBaseCommon<QuaternionType, type>::operator [] (const wn_uint32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE type& __WNQuaternionBaseCommon<QuaternionType, type>::operator () (const wn_uint32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE const type& __WNQuaternionBaseCommon<QuaternionType, type>::operator () (const wn_uint32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Index out of range of quaternion elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator + () const {
            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator - () const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion.mElements = __WNGeneralOpNegate<type, 4>::Execute(quaternion.mElements);

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, type>::operator += (const QuaternionType& _quaternion) {
            this->mElements = __WNGeneralOpAdd<type, 4>::Execute(this->mElements, _quaternion.mElements);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, type>::operator -= (const QuaternionType& _quaternion) {
            this->mElements = __WNGeneralOpSubtract<type, 4>::Execute(this->mElements, _quaternion.mElements);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, type>::operator *= (const QuaternionType& _quaternion) {
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

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, type>::operator *= (const type& _number) {
            this->mElements = __WNGeneralOpMultiplyNumber<type, 4>::Execute(this->mElements, _number);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType& __WNQuaternionBaseCommon<QuaternionType, type>::operator /= (const type& _number) {
            this->mElements = __WNGeneralOpDivideNumber<type, 4>::Execute(this->mElements, _number);

            return(*static_cast<QuaternionType*>(this));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator + (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion += _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator - (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion -= _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator * (const QuaternionType& _quaternion) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion *= _quaternion;

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator * (const type& _number) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion *= _number;

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBaseCommon<QuaternionType, type>::operator / (const type& _number) const {
            QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            quaternion /= _number;

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_bool __WNQuaternionBaseCommon<QuaternionType, type>::operator == (const QuaternionType& _quaternion) const {
            return(__WNGeneralOpEqual<type, 4>::Execute(this->mElements, _quaternion.mElements));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_bool __WNQuaternionBaseCommon<QuaternionType, type>::operator != (const QuaternionType& _quaternion) const {
            return(__WNGeneralOpNotEqual<type, 4>::Execute(this->mElements, _quaternion.mElements));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Zero() {
            this->mElements = __WNGeneralOpZero<type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Identity() {
            this->mElements[0] = static_cast<type>(0);
            this->mElements[1] = static_cast<type>(0);
            this->mElements[2] = static_cast<type>(0);
            this->mElements[3] = static_cast<type>(1);
        }

        template <typename QuaternionType, typename type>
        type __WNQuaternionBaseCommon<QuaternionType, type>::Length() const {
             return(__WNVectorOpLength<type, 4>::Execute(this->mElements));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE type __WNQuaternionBaseCommon<QuaternionType, type>::LengthSquared() const {
             return(__WNVectorOpLengthSquared<type, 4>::Execute(this->mElements));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE type __WNQuaternionBaseCommon<QuaternionType, type>::Dot(const QuaternionType& _quaternion) const {
            const QuaternionType quaternion = *static_cast<const QuaternionType*>(this);

            return((quaternion.mElements[0] * _quaternion.mElements[0]) + (quaternion.mElements[1] * _quaternion.mElements[1]) +
                   (quaternion.mElements[2] * _quaternion.mElements[2]) + (quaternion.mElements[3] * _quaternion.mElements[3]));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_bool __WNQuaternionBaseCommon<QuaternionType, type>::IsZero() const {
            return(this->mElements[0] == static_cast<type>(0) && this->mElements[1] == static_cast<type>(0) &&
                   this->mElements[2] == static_cast<type>(0) && this->mElements[3] == static_cast<type>(0));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_bool __WNQuaternionBaseCommon<QuaternionType, type>::IsIdentity() const {
            return(this->mElements[0] == static_cast<type>(0) && this->mElements[1] == static_cast<type>(0) &&
                   this->mElements[2] == static_cast<type>(0) && this->mElements[3] == static_cast<type>(1));
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Set(const QuaternionType& _quaternion) {
            *static_cast<QuaternionType*>(this) = _quaternion;
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Set(const type& _number) {
            this->mElements = __WNGeneralOpSetNumber<type, 4>::Execute(this->mElements, _number);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Set(const type* _numbers) {
            this->mElements = __WNGeneralOpSetNumbers<type, 4>::Execute(this->mElements, _numbers);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBaseCommon<QuaternionType, type>::Set(const type& _x, const type& _y, const type& _z, const type& _w) {
            this->mElements[0] = _x;
            this->mElements[1] = _y;
            this->mElements[2] = _z;
            this->mElements[3] = _w;
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::Normalize() {
            this->mElements = __WNVectorOpNormalize<type, 4>::Execute(this->mElements);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::Invert() {
            *this = GetConjugate();

            Normalize();
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::Logarithm() {
            if (WNAbs(this->mElements[3]) <= static_cast<type>(1)) {
                const type theta = WNACos(this->mElements[3]);
                const type thetaOverSinTheta  = theta / WNSin(theta);

                this->mElements[0] *= thetaOverSinTheta;
                this->mElements[1] *= thetaOverSinTheta;
                this->mElements[2] *= thetaOverSinTheta;
            }

            this->mElements[3] = static_cast<type>(0);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::Exponent() {
            const type theta = wn::sqrt(this->mElements[0] * this->mElements[0] +
                                      this->mElements[1] * this->mElements[1] +
                                      this->mElements[2] * this->mElements[2]);
            const type cosTheta = WNCos(theta);

            if (WNAbs(theta) >= static_cast<type>(0)) {
                const type oneOverTheta = static_cast<type>(1) / theta;
                const type sinTheta = WNSin(theta);

                this->mElements[0] *= oneOverTheta * sinTheta;
                this->mElements[1] *= oneOverTheta * sinTheta;
                this->mElements[2] *= oneOverTheta * sinTheta;
            }

            this->mElements[3] = cosTheta;
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE wn_void __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::Rotate(const QuaternionType& _rotation) {
            *this *= _rotation;
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetNormalized() const {
            QuaternionType quaternion = *this;

            return(quaternion.Normalize(), quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetInverted() const {
            QuaternionType quaternion = *this;

            return(quaternion.Invert(), quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetConjugate() const {
            QuaternionType quaternion = *this;

            quaternion.mElements[0] = -quaternion.mElements[0];
            quaternion.mElements[1] = -quaternion.mElements[1];
            quaternion.mElements[2] = -quaternion.mElements[2];

            return(quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetLogarithm() const {
            QuaternionType quaternion = *this;

            return(quaternion.Logarithm(), quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetExponent() const {
            QuaternionType quaternion = *this;

            return(quaternion.Exponent(), quaternion);
        }

        template <typename QuaternionType, typename type>
        WN_FORCE_INLINE QuaternionType __WNQuaternionBase<QuaternionType, type, typename std::enable_if<wn::is_real<type>::value>::type>::GetRotated(const QuaternionType& _rotation) const {
            QuaternionType quaternion = *this;

            return(quaternion.Rotate(_rotation), quaternion);
        }
    }
}

#endif // __WN_MATH_INTERNAL_QUATERNION_BASE_INL__
