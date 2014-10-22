// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename MatrixType, typename Type, WN_UINT32 Order>
        class __WNMatrixBaseCommon {
        public:
            Type& operator [] (const WN_UINT32 _index);
            const Type& operator [] (const WN_UINT32 _index) const;

            Type& operator () (const WN_UINT32 _index);
            const Type& operator () (const WN_UINT32 _index) const;

            Type& operator () (const WN_UINT32 _index1, const WN_UINT32 _index2);
            const Type& operator () (const WN_UINT32 _index1, const WN_UINT32 _index2) const;

            MatrixType operator + () const;
            MatrixType operator - () const;

            MatrixType& operator += (const MatrixType& _matrix);
            MatrixType& operator -= (const MatrixType& _matrix);
            MatrixType& operator *= (const MatrixType& _matrix);
            MatrixType& operator += (const Type& _number);
            MatrixType& operator -= (const Type& _number);
            MatrixType& operator *= (const Type& _number);
            MatrixType& operator /= (const Type& _number);

            MatrixType operator + (const MatrixType& _matrix) const;
            MatrixType operator - (const MatrixType& _matrix) const;
            MatrixType operator * (const MatrixType& _matrix) const;
            MatrixType operator + (const Type& _number) const;
            MatrixType operator - (const Type& _number) const;
            MatrixType operator * (const Type& _number) const;
            MatrixType operator / (const Type& _number) const;

            WN_BOOL operator == (const MatrixType& _matrix) const;
            WN_BOOL operator != (const MatrixType& _matrix) const;

            WN_VOID Zero();
            WN_VOID Identity();
            WN_VOID Transpose();

            Type Trace() const;
            Type Determinant() const; // need to fix

            WN_BOOL IsIdentity() const;
            WN_BOOL IsSymmetric() const;
            WN_BOOL IsDiagonal() const;

            WN_VOID Set(const MatrixType& _matrix);
            WN_VOID Set(const Type& _number);
            WN_VOID Set(const Type* _numbers);

            MatrixType GetTransposed() const;

        public:
            static WN_FORCE_INLINE MatrixType CreateZero();
            static WN_FORCE_INLINE MatrixType CreateIdentity();

        protected:
            enum {
                Dimension = (Order * Order)
            };

        protected:
            WN_FORCE_INLINE __WNMatrixBaseCommon() {}

        protected:
            __WNElements<Type, Dimension> mElements;
        };

        template <typename MatrixType, typename Type, WN_UINT32 Order, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNMatrixBase : public __WNMatrixBaseCommon<MatrixType, Type, Order> {
        protected:
            WN_FORCE_INLINE __WNMatrixBase() {}
        };

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        class __WNMatrixBase<MatrixType, Type, Order, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value> : public __WNMatrixBaseCommon<MatrixType, Type, Order> {
        public:
            WN_FORCE_INLINE WN_BOOL Invert() {}

            MatrixType GetInverted() const;
            WN_FORCE_INLINE MatrixType GetAdjugate() const {}

        protected:
            WN_FORCE_INLINE __WNMatrixBase() {}
        };
    }
}

#include "WNMath/inc/Internal/WNMatrixBase.inl"

#endif // __WN_MATH_INTERNAL_MATRIX_BASE_H__