// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_BASE_H__
#define __WN_MATH_INTERNAL_VECTOR_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElements.h"
#include "WNMath/inc/WNCommon.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        class __WNVectorBaseCommon {
        public:
            Type& operator [] (const WN_UINT32 _index);
            const Type& operator [] (const WN_UINT32 _index) const;

            Type& operator () (const WN_UINT32 _index);
            const Type& operator () (const WN_UINT32 _index) const;

            VectorType operator + () const;
            VectorType operator - () const;

            VectorType& operator += (const VectorType& _vector);
            VectorType& operator -= (const VectorType& _vector);
            VectorType& operator *= (const VectorType& _vector);
            VectorType& operator /= (const VectorType& _vector);

            VectorType& operator += (const Type& _number);
            VectorType& operator -= (const Type& _number);
            VectorType& operator *= (const Type& _number);
            VectorType& operator /= (const Type& _number);

            VectorType operator + (const VectorType& _vector) const;
            VectorType operator - (const VectorType& _vector) const;
            VectorType operator * (const VectorType& _vector) const;
            VectorType operator / (const VectorType& _vector) const;

            VectorType operator + (const Type& _number) const;
            VectorType operator - (const Type& _number) const;
            VectorType operator * (const Type& _number) const;
            VectorType operator / (const Type& _number) const;

            WN_BOOL operator == (const VectorType& _vector) const;
            WN_BOOL operator != (const VectorType& _vector) const;

            WN_VOID Zero();
            WN_VOID One();

            WN_VOID MultiplyAdd(const VectorType& _vector1, const VectorType& _vector2);
            WN_VOID MultiplyAdd(const Type& _number1, const Type& _number2);
            WN_VOID MultiplySubtract(const VectorType& _vector1, const VectorType& _vector2);
            WN_VOID MultiplySubtract(const Type& _number1, const Type& _number2);

            WN_VOID Clamp(const VectorType& _minimum, const VectorType& _maximum);
            WN_VOID Clamp(const Type& _minimum, const Type& _maximum);
            WN_VOID Minimum(const VectorType& _vector);
            WN_VOID Maximum(const VectorType& _vector);
            WN_VOID Scale(const Type& _factor);
            WN_VOID Scale(const VectorType& _vector);
            WN_VOID Average(const VectorType& _vector);
            WN_VOID Saturate();
            WN_VOID Reverse();

            Type Length() const;
            Type LengthSquared() const;
            Type Dot(const VectorType& _vector) const;

            WN_BOOL IsZero() const;
            WN_BOOL IsOne() const;

            WN_VOID Set(const VectorType& _vector);
            WN_VOID Set(const Type& _number);
            WN_VOID Set(const Type* _numbers);

            VectorType GetClamped(const VectorType& _minimum, const VectorType& _maximum) const;
            VectorType GetMinimum(const VectorType& _vector) const;
            VectorType GetMaximum(const VectorType& _vector) const;
            VectorType GetScaled(const Type& _factor) const;
            VectorType GetScaled(const VectorType& _vector) const;
            VectorType GetAverage(const VectorType& _vector) const;
            VectorType GetSaturated() const;
            VectorType GetReversed() const;

        public:
            static VectorType CreateZero();

        protected:
            __WNVectorBaseCommon();

        protected:
            __WNElements<Type, Dimension> mElements;
        };

        template <typename VectorType, typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNVectorBase : public __WNVectorBaseCommon<VectorType, Type, Dimension> {
        protected:
            WN_FORCE_INLINE __WNVectorBase() {}
        };

        template <typename VectorType, typename Type, WN_UINT32 Dimension>
        class __WNVectorBase<VectorType, Type, Dimension, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value> : public __WNVectorBaseCommon<VectorType, Type, Dimension> {
        public:
            WN_VOID Normalize();
            WN_VOID Truncate(const Type& _length);
            WN_VOID Snap(const WNSnapDirection _direction = eWNNearest);
            WN_VOID Recipricol();

            VectorType GetNormalized() const;
            VectorType GetTruncated(const Type& _length) const;
            VectorType GetSnaped(const WNSnapDirection _direction = eWNNearest) const;
            VectorType GetRecipricol() const;

        protected:
            WN_FORCE_INLINE __WNVectorBase() {}
        };
    }
}

#include "WNMath/inc/Internal/WNVectorBase.inl"

#endif // __WN_MATH_INTERNAL_VECTOR_BASE_H__