// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_BASE_INL__

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_H__
    #error "Internal/WNMatrixBase.inl should never be included directly."
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNGeneralOps.h"

namespace wn {
    namespace internal {
        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator [] (const wn_uint32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < (dimension), "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE const type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator [] (const wn_uint32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < (dimension), "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator () (const wn_uint32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE const type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator () (const wn_uint32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator () (const wn_uint32 _index1, const wn_uint32 _index2) {
            WN_DEBUG_ASSERT_DESC(_index1 < Order, "Index out of range of matrix elements");
            WN_DEBUG_ASSERT_DESC(_index2 < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[(_index1 * Order) + _index2]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE const type& __WNMatrixBaseCommon<MatrixType, type, Order>::operator () (const wn_uint32 _index1, const wn_uint32 _index2) const {
            WN_DEBUG_ASSERT_DESC(_index1 < Order, "Index out of range of matrix elements");
            WN_DEBUG_ASSERT_DESC(_index2 < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[(_index1 * Order) + _index2]);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator + () const {
            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator - () const {
            this->mElements = __WNGeneralOpNegate<type, dimension>::Execute(this->mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator += (const MatrixType& _matrix) {
            this->mElements = __WNGeneralOpAdd<type, dimension>::Execute(this->mElements, _matrix.mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator -= (const MatrixType& _matrix) {
            this->mElements = __WNGeneralOpSubtract<type, dimension>::Execute(this->mElements, _matrix.mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator *= (const MatrixType& _matrix) {
            const MatrixType matrix = *static_cast<MatrixType*>(this);

            for (wn_uint32 i = 0; i < Order; ++i) {
                for (wn_uint32 j = 0; j < Order; ++j) {
                    type result = static_cast<type>(0);

                    for (wn_uint32 k = 0; k < Order; ++k) {
                        result += (matrix.mElements.mValues[i * Order + k] * _matrix.mElements.mValues[k * Order + j]);
                    }

                    this->mElements.mValues[i * Order + j] = result;
                }
            }

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator += (const type& _number) {
            this->mElements = __WNGeneralOpAddNumber<type, dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator -= (const type& _number) {
            this->mElements = __WNGeneralOpSubtractNumber<type, dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator *= (const type& _number) {
            this->mElements = __WNGeneralOpMultiplyNumber<type, dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, type, Order>::operator /= (const type& _number) {
            this->mElements = __WNGeneralOpDivideNumber<type, dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator + (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix += _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator - (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix -= _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator * (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix *= _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator + (const type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix += _number;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator - (const type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix -= _number;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator * (const type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix *= _number;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::operator / (const type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix /= _number;

            return(matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 dimension>
        WN_FORCE_INLINE wn_bool __WNMatrixBaseCommon<MatrixType, type, dimension>::operator == (const MatrixType& _matrix) const {
            return(__WNGeneralOpEqual<type, dimension>::Execute(this->mElements, _matrix.mElements));
        }

        template <typename MatrixType, typename type, wn_uint32 dimension>
        WN_FORCE_INLINE wn_bool __WNMatrixBaseCommon<MatrixType, type, dimension>::operator != (const MatrixType& _matrix) const {
            return(__WNGeneralOpNotEqual<type, dimension>::Execute(this->mElements, _matrix.mElements));
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Zero() {
            this->mElements = __WNGeneralOpZero<type, dimension>::Execute(this->mElements);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Identity() {
            for (wn_uint32 i = 0; i < Order; ++i) {
                for (wn_uint32 j = 0; j < Order; ++j) {
                    if (i == j) {
                        this->mElements.mValues[(j * Order) + i] = static_cast<type>(1);
                    } else {
                        this->mElements.mValues[(j * Order) + i] = static_cast<type>(0);
                    }
                }
            }
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Transpose() {
            const MatrixType matrix = *static_cast<MatrixType*>(this);

            for (wn_uint32 i = 0; i < Order; ++i) {
                for (wn_uint32 j = 0; j < Order; ++j) {
                    this->mElements.mValues[(j * Order) + i] = matrix.mElements.mValues[(i * Order) + j];
                }
            }
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE type __WNMatrixBaseCommon<MatrixType, type, Order>::Trace() const {
            type trace = 0;

            for (wn_uint32 i = 0; i < Order; ++i) {
                trace += this->mElements.mValues[(i * Order) + i];
            }

            return(trace);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE type __WNMatrixBaseCommon<MatrixType, type, Order>::Determinant() const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);
            wn_uint32 count = 0;

            for (wn_uint32 i = 0; i < Order - 1; ++i) {
                if (matrix.mElements.mValues[(i * Order) + i] == static_cast<type>(0)) {
                    for (wn_uint32 k = i; k < Order; ++k) {
                        if (matrix.mElements.mValues[(k * Order) + i] != static_cast<type>(0)) {
                            for (wn_uint32 j = 0; j < Order; ++j) {
                                const type temp = matrix.mElements.mValues[(i * Order) + j];

                                matrix.mElements.mValues[(i * Order) + j] = matrix.mElements.mValues[(k * Order) + j];
                                matrix.mElements.mValues[(k * Order) + j] = temp;
                            }

                            k = Order;
                        }
                    }

                    count++;
                }

                if (matrix.mElements.mValues[(i * Order) + i] != static_cast<type>(0)) {
                    for (wn_uint32 k = i + 1; k < Order; ++k) {
                        const type factor = matrix.mElements.mValues[(k * Order) + i] / matrix.mElements.mValues[(i * Order) + i];

                        for (wn_uint32 j = i; j < Order; ++j) {
                            matrix.mElements.mValues[(k * Order) + j] -= (factor * matrix.mElements.mValues[(i * Order) + j]);
                        }
                    }
                }
            }

            type determinant = static_cast<type>(1);

            for (wn_uint32 i = 0; i < Order; ++i) {
                determinant *= matrix.mElements.mValues[(i * Order) + i];
            }

            if (count % 2 == 0) {
                return(determinant);
            } else {
                return(static_cast<type>(-1) * determinant);
            }
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_bool __WNMatrixBaseCommon<MatrixType, type, Order>::IsIdentity() const {
            for (wn_uint32 i = 0; i < Order; ++i) {
                for (wn_uint32 j = 0; j < Order; ++j) {
                    if (i == j) {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<type>(1)) {
                            return(wn_false);
                        }
                    } else {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<type>(0)) {
                            return(wn_false);
                        }
                    }
                }
            }
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_bool __WNMatrixBaseCommon<MatrixType, type, Order>::IsSymmetric() const {
            return(wn_false);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_bool __WNMatrixBaseCommon<MatrixType, type, Order>::IsDiagonal() const {
            for (wn_uint32 i = 0; i < Order; ++i) {
                for (wn_uint32 j = 0; j < Order; ++j) {
                    if (i != j) {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<type>(0)) {
                            return(wn_false);
                        }
                    }
                }
            }
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Set(const MatrixType& _matrix) {
            *static_cast<MatrixType*>(this) = _matrix;
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Set(const type& _number) {
            this->mElements = __WNGeneralOpSetNumber<type, dimension>::Execute(this->mElements, _number);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE wn_void __WNMatrixBaseCommon<MatrixType, type, Order>::Set(const type* _numbers) {
            this->mElements = __WNGeneralOpSetNumbers<type, dimension>::Execute(this->mElements, _numbers);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::GetTransposed() const {
            MatrixType matrix = *static_cast<MatrixType*>(this);

            return(matrix.Transpose(), matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::CreateZero() {
            MatrixType matrix;

            return(matrix.Zero(), matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, type, Order>::CreateIdentity() {
            MatrixType matrix;

            return(matrix.Identity(), matrix);
        }

        template <typename MatrixType, typename type, wn_uint32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBase<MatrixType, type, Order, typename std::enable_if<wn::is_real<type>::value>::type>::GetInverted() const {
            MatrixType matrix = *static_cast<MatrixType*>(this);

            return(matrix.Invert(), matrix);
        }
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_BASE_INL__