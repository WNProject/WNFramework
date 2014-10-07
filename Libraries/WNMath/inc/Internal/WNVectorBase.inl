////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_BASE_INL__
#define __WN_MATH_INTERNAL_VECTOR_BASE_INL__

#ifndef __WN_MATH_INTERNAL_VECTOR_BASE_H__
    #error "Internal/WNVectorBase.inl should never be included directly."
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNGeneralOps.h"
#include "WNMath/inc/Internal/WNVectorOps.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE __WNVectorBaseCommon<VectorType, Type, Dimension>::__WNVectorBaseCommon() {
            this->mElements = __WNGeneralOpInitialize<Type, Dimension>::Execute(this->mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE Type& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator [] (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < Dimension, "Index out of range of vector elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE const Type& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator [] (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < Dimension, "Index out of range of vector elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE Type& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator () (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < Dimension, "Index out of range of vector elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE const Type& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator () (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < Dimension, "Index out of range of vector elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator + () const {
            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator - () const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector.mElements = __WNGeneralOpNegate<Type, Dimension>::Execute(vector.mElements);

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator += (const VectorType& _vector) {
            this->mElements = __WNGeneralOpAdd<Type, Dimension>::Execute(this->mElements, _vector.mElements);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator -= (const VectorType& _vector) {
            this->mElements = __WNGeneralOpSubtract<Type, Dimension>::Execute(this->mElements, _vector.mElements);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator *= (const VectorType& _vector) {
            this->mElements = __WNGeneralOpMultiply<Type, Dimension>::Execute(this->mElements, _vector.mElements);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator /= (const VectorType& _vector) {
            this->mElements = __WNGeneralOpDivide<Type, Dimension>::Execute(this->mElements, _vector.mElements);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator += (const Type& _number) {
            this->mElements = __WNGeneralOpAddNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator -= (const Type& _number) {
            this->mElements = __WNGeneralOpSubtractNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator *= (const Type& _number) {
            this->mElements = __WNGeneralOpMultiplyNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType& __WNVectorBaseCommon<VectorType, Type, Dimension>::operator /= (const Type& _number) {
            this->mElements = __WNGeneralOpDivideNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<VectorType*>(this));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator + (const VectorType& _vector) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector += _vector;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator - (const VectorType& _vector) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector -= _vector;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator * (const VectorType& _vector) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector *= _vector;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator / (const VectorType& _vector) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector /= _vector;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator + (const Type& _number) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector += _number;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator - (const Type& _number) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector -= _number;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator * (const Type& _number) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector *= _number;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::operator / (const Type& _number) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            vector /= _number;

            return(vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNVectorBaseCommon<VectorType, Type, Dimension>::operator == (const VectorType& _vector) const {
            return(__WNGeneralOpEqual<Type, Dimension>::Execute(this->mElements, _vector.mElements));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNVectorBaseCommon<VectorType, Type, Dimension>::operator != (const VectorType& _vector) const {
            return(__WNGeneralOpNotEqual<Type, Dimension>::Execute(this->mElements, _vector.mElements));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Zero() {
            this->mElements = __WNGeneralOpZero<Type, Dimension>::Execute(this->mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::One() {
            this->mElements = __WNGeneralOpOne<Type, Dimension>::Execute(this->mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::MultiplyAdd(const VectorType& _vector1, const VectorType& _vector2) {
            this->mElements = __WNGeneralOpMultiplyAdd<Type, Dimension>::Execute(this->mElements, _vector1.mElements, _vector2.mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::MultiplyAdd(const Type& _number1, const Type& _number2) {
            this->mElements = __WNGeneralOpMultiplyAddNumbers<Type, Dimension>::Execute(this->mElements, _number1, _number2);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::MultiplySubtract(const VectorType& _vector1, const VectorType& _vector2) {
            this->mElements = __WNGeneralOpMultiplySubtract<Type, Dimension>::Execute(this->mElements, _vector1.mElements, _vector2.mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::MultiplySubtract(const Type& _number1, const Type& _number2) {
            this->mElements = __WNGeneralOpMultiplySubtractNumbers<Type, Dimension>::Execute(this->mElements, _number1, _number2);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Clamp(const VectorType& _minimum, const VectorType& _maximum) {
            this->mElements = __WNGeneralOpClamp<Type, Dimension>::Execute(this->mElements, _minimum.mElements, _maximum.mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Clamp(const Type& _minimum, const Type& _maximum) {
            this->mElements = __WNGeneralOpClampNumbers<Type, Dimension>::Execute(this->mElements, _minimum, _maximum);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Minimum(const VectorType& _vector) {
            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] = WNMin(this->mElements.mValues[i], _vector.mElements.mValues[i]);
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Maximum(const VectorType& _vector) {
            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] = WNMax(this->mElements.mValues[i], _vector.mElements.mValues[i]);
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Scale(const Type& _factor) {
            *static_cast<VectorType*>(this) *= _factor;
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Scale(const VectorType& _vector) {
            *static_cast<VectorType*>(this) *= _vector;
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Average(const VectorType& _vector) {
            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] += _vector.mElements.mValues[i];
                this->mElements.mValues[i] /= static_cast<Type>(2);
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Saturate() {
            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] = WNClamp(this->mElements.mValues[i], static_cast<Type>(0), static_cast<Type>(1));
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Reverse() {
            VectorType vector = *static_cast<VectorType*>(this);

            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] = vector.mElements.mValues[(Dimension - 1) - i];
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE Type __WNVectorBaseCommon<VectorType, Type, Dimension>::Length() const {
            return(__WNVectorOpLength<Type, Dimension>::Execute(this->mElements));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE Type __WNVectorBaseCommon<VectorType, Type, Dimension>::LengthSquared() const {
            return(__WNVectorOpLengthSquared<Type, Dimension>::Execute(this->mElements));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE Type __WNVectorBaseCommon<VectorType, Type, Dimension>::Dot(const VectorType& _vector) const {
            Type dot = static_cast<Type>(0);

            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                dot += (this->mElements.mValues[i] * _vector.mElements.mValues[i]);
            }

            return(dot);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNVectorBaseCommon<VectorType, Type, Dimension>::IsZero() const {
            return(__WNGeneralOpIsZero<Type, Dimension>::Execute(this->mElements));
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNVectorBaseCommon<VectorType, Type, Dimension>::IsOne() const {
            for (WN_UINT32 i  = 0; i < Dimension; ++i) {
                if (this->mElements.mValues[i] != static_cast<Type>(1)) {
                    return(WN_FALSE);
                }
            }

            return(WN_TRUE);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Set(const VectorType& _vector) {
            *static_cast<VectorType*>(this) = _vector;
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Set(const Type& _number) {
            this->mElements = __WNGeneralOpSetNumber<Type, Dimension>::Execute(this->mElements, _number);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBaseCommon<VectorType, Type, Dimension>::Set(const Type* _numbers) {
            this->mElements = __WNGeneralOpSetNumbers<Type, Dimension>::Execute(this->mElements, _numbers);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetClamped(const VectorType& _minimum, const VectorType& _maximum) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Clamp(_minimum, _maximum), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetMinimum(const VectorType& _vector) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Minimum(_vector), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetMaximum(const VectorType& _vector) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Maximum(_vector), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetScaled(const Type& _factor) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Scale(_factor), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetScaled(const VectorType& _vector) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Scale(_vector), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetAverage(const VectorType& _vector) const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Average(_vector), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetSaturated() const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Saturate(), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::GetReversed() const {
            VectorType vector = *static_cast<VectorType*>(this);

            return(vector.Reverse(), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBaseCommon<VectorType, Type, Dimension>::CreateZero() {
            VectorType vector;

            return(vector.Zero(), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Normalize() {
            this->mElements = __WNVectorOpNormalize<Type, Dimension>::Execute(this->mElements);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Truncate(const Type& _length) {
            if (_length != static_cast<Type>(0)) {
                const Type lengthSquared = this->LengthSquared();

                if (lengthSquared > (_length * _length)) {
                    const Type length = _length / WNInvSqrt(lengthSquared);

                    *static_cast<VectorType*>(this) /= length;
                }
            } else {
                this->Zero();
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Snap(const WNSnapDirection _direction) {
            switch (_direction) {
            case eWNNearest:
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    this->mElements.mValues[i] = WNRound(this->mElements.mValues[i]);
                }

                break;
            case eWNUp:
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    this->mElements.mValues[i] = WNCeil(this->mElements.mValues[i]);
                }

                break;
            case eWNDown:
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    this->mElements.mValues[i] = WNFloor(this->mElements.mValues[i]);
                }

                break;
            case eWNTruncate:
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    this->mElements.mValues[i] = WNTrunc(this->mElements.mValues[i]);
                }
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_VOID __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::Recipricol() {
            for (WN_UINT32 i = 0; i < Dimension; ++i) {
                this->mElements.mValues[i] = static_cast<Type>(1) / (this->mElements.mValues[i]);
            }
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetNormalized() const {
            VectorType vector = *static_cast<const VectorType*>(this);

            return(vector.Normalize(), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetTruncated(const Type& _length) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            return(vector.Truncate(_length), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetSnaped(const WNSnapDirection _direction) const {
            VectorType vector = *static_cast<const VectorType*>(this);

            return(vector.Snap(_direction), vector);
        }

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE VectorType __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetRecipricol() const {
            VectorType vector = *static_cast<const VectorType*>(this);

            return(vector.Recipricol(), vector);
        }
    }
}

#endif // __WN_MATH_INTERNAL_VECTOR_BASE_INL__