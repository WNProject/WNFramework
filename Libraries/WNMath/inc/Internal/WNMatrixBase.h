// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElementArray.h"

namespace wn {
    namespace internal {
        template <typename MatrixType, typename type, wn_uint32 Order>
        class __WNMatrixBaseCommon {
        public:
            type& operator [] (const wn_uint32 _index);
            const type& operator [] (const wn_uint32 _index) const;

            type& operator () (const wn_uint32 _index);
            const type& operator () (const wn_uint32 _index) const;

            type& operator () (const wn_uint32 _index1, const wn_uint32 _index2);
            const type& operator () (const wn_uint32 _index1, const wn_uint32 _index2) const;

            MatrixType operator + () const;
            MatrixType operator - () const;

            MatrixType& operator += (const MatrixType& _matrix);
            MatrixType& operator -= (const MatrixType& _matrix);
            MatrixType& operator *= (const MatrixType& _matrix);
            MatrixType& operator += (const type& _number);
            MatrixType& operator -= (const type& _number);
            MatrixType& operator *= (const type& _number);
            MatrixType& operator /= (const type& _number);

            MatrixType operator + (const MatrixType& _matrix) const;
            MatrixType operator - (const MatrixType& _matrix) const;
            MatrixType operator * (const MatrixType& _matrix) const;
            MatrixType operator + (const type& _number) const;
            MatrixType operator - (const type& _number) const;
            MatrixType operator * (const type& _number) const;
            MatrixType operator / (const type& _number) const;

            wn_bool operator == (const MatrixType& _matrix) const;
            wn_bool operator != (const MatrixType& _matrix) const;

            wn_void Zero();
            wn_void Identity();
            wn_void Transpose();

            type Trace() const;
            type Determinant() const; // need to fix

            wn_bool IsIdentity() const;
            wn_bool IsSymmetric() const;
            wn_bool IsDiagonal() const;

            wn_void Set(const MatrixType& _matrix);
            wn_void Set(const type& _number);
            wn_void Set(const type* _numbers);

            MatrixType GetTransposed() const;

        public:
            static WN_FORCE_INLINE MatrixType CreateZero();
            static WN_FORCE_INLINE MatrixType CreateIdentity();

        protected:
            enum {
                dimension = (Order * Order)
            };

        protected:
            WN_FORCE_INLINE __WNMatrixBaseCommon() {}

        protected:
            element_array<type, dimension> mElements;
        };

        template <typename MatrixType, typename type, wn_uint32 Order, typename = std::enable_if<wn_true>::type>
        class __WNMatrixBase : public __WNMatrixBaseCommon<MatrixType, type, Order> {
        protected:
            WN_FORCE_INLINE __WNMatrixBase() {}
        };

        template <typename MatrixType, typename type, wn_uint32 Order>
        class __WNMatrixBase<MatrixType, type, Order, typename std::enable_if<wn::is_real<type>::value>::type> : public __WNMatrixBaseCommon<MatrixType, type, Order> {
        public:
            WN_FORCE_INLINE wn_bool Invert() {}

            MatrixType GetInverted() const;
            WN_FORCE_INLINE MatrixType GetAdjugate() const {}

        protected:
            WN_FORCE_INLINE __WNMatrixBase() {}
        };
    }
}

#include "WNMath/inc/Internal/WNMatrixBase.inl"

#endif // __WN_MATH_INTERNAL_MATRIX_BASE_H__