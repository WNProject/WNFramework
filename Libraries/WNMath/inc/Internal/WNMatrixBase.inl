////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_BASE_INL__

#ifndef __WN_MATH_INTERNAL_MATRIX_BASE_H__
    #error "Internal/WNMatrixBase.inl should never be included directly."
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNGeneralOps.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator [] (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < (Dimension), "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE const Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator [] (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < (Dimension), "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator () (const WN_UINT32 _index) {
            WN_DEBUG_ASSERT_DESC(_index < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE const Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator () (const WN_UINT32 _index) const {
            WN_DEBUG_ASSERT_DESC(_index < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[_index]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator () (const WN_UINT32 _index1, const WN_UINT32 _index2) {
            WN_DEBUG_ASSERT_DESC(_index1 < Order, "Index out of range of matrix elements");
            WN_DEBUG_ASSERT_DESC(_index2 < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[(_index1 * Order) + _index2]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE const Type& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator () (const WN_UINT32 _index1, const WN_UINT32 _index2) const {
            WN_DEBUG_ASSERT_DESC(_index1 < Order, "Index out of range of matrix elements");
            WN_DEBUG_ASSERT_DESC(_index2 < Order, "Index out of range of matrix elements");

            return(this->mElements.mValues[(_index1 * Order) + _index2]);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator + () const {
            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator - () const {
            this->mElements = __WNGeneralOpNegate<Type, Dimension>::Execute(this->mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator += (const MatrixType& _matrix) {
            this->mElements = __WNGeneralOpAdd<Type, Dimension>::Execute(this->mElements, _matrix.mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator -= (const MatrixType& _matrix) {
            this->mElements = __WNGeneralOpSubtract<Type, Dimension>::Execute(this->mElements, _matrix.mElements);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator *= (const MatrixType& _matrix) {
            const MatrixType matrix = *static_cast<MatrixType*>(this);

            for (WN_UINT32 i = 0; i < Order; ++i) {
                for (WN_UINT32 j = 0; j < Order; ++j) {
                    Type result = static_cast<Type>(0);

                    for (WN_UINT32 k = 0; k < Order; ++k) {
                        result += (matrix.mElements.mValues[i * Order + k] * _matrix.mElements.mValues[k * Order + j]);
                    }

                    this->mElements.mValues[i * Order + j] = result;
                }
            }

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator += (const Type& _number) {
            this->mElements = __WNGeneralOpAddNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator -= (const Type& _number) {
            this->mElements = __WNGeneralOpSubtractNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator *= (const Type& _number) {
            this->mElements = __WNGeneralOpMultiplyNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType& __WNMatrixBaseCommon<MatrixType, Type, Order>::operator /= (const Type& _number) {
            this->mElements = __WNGeneralOpDivideNumber<Type, Dimension>::Execute(this->mElements, _number);

            return(*static_cast<MatrixType*>(this));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator + (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix += _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator - (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix -= _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator * (const MatrixType& _matrix) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix *= _matrix;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator + (const Type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix += _number;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator - (const Type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix -= _number;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator * (const Type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix *= _number;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::operator / (const Type& _number) const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);

            matrix /= _number;

            return(matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNMatrixBaseCommon<MatrixType, Type, Dimension>::operator == (const MatrixType& _matrix) const {
            return(__WNGeneralOpEqual<Type, Dimension>::Execute(this->mElements, _matrix.mElements));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Dimension>
        WN_FORCE_INLINE WN_BOOL __WNMatrixBaseCommon<MatrixType, Type, Dimension>::operator != (const MatrixType& _matrix) const {
            return(__WNGeneralOpNotEqual<Type, Dimension>::Execute(this->mElements, _matrix.mElements));
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Zero() {
            this->mElements = __WNGeneralOpZero<Type, Dimension>::Execute(this->mElements);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Identity() {
            for (WN_UINT32 i = 0; i < Order; ++i) {
                for (WN_UINT32 j = 0; j < Order; ++j) {
                    if (i == j) {
                        this->mElements.mValues[(j * Order) + i] = static_cast<Type>(1);
                    } else {
                        this->mElements.mValues[(j * Order) + i] = static_cast<Type>(0);
                    }
                }
            }
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Transpose() {
            const MatrixType matrix = *static_cast<MatrixType*>(this);

            for (WN_UINT32 i = 0; i < Order; ++i) {
                for (WN_UINT32 j = 0; j < Order; ++j) {
                    this->mElements.mValues[(j * Order) + i] = matrix.mElements.mValues[(i * Order) + j];
                }
            }
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE Type __WNMatrixBaseCommon<MatrixType, Type, Order>::Trace() const {
            Type trace = 0;

            for (WN_UINT32 i = 0; i < Order; ++i) {
                trace += this->mElements.mValues[(i * Order) + i];
            }

            return(trace);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE Type __WNMatrixBaseCommon<MatrixType, Type, Order>::Determinant() const {
            MatrixType matrix = *static_cast<const MatrixType*>(this);
            WN_UINT32 count = 0;

            for (WN_UINT32 i = 0; i < Order - 1; ++i) {
                if (matrix.mElements.mValues[(i * Order) + i] == static_cast<Type>(0)) {
                    for (WN_UINT32 k = i; k < Order; ++k) {
                        if (matrix.mElements.mValues[(k * Order) + i] != static_cast<Type>(0)) {
                            for (WN_UINT32 j = 0; j < Order; ++j) {
                                const Type temp = matrix.mElements.mValues[(i * Order) + j];

                                matrix.mElements.mValues[(i * Order) + j] = matrix.mElements.mValues[(k * Order) + j];
                                matrix.mElements.mValues[(k * Order) + j] = temp;
                            }

                            k = Order;
                        }
                    }

                    count++;
                }

                if (matrix.mElements.mValues[(i * Order) + i] != static_cast<Type>(0)) {
                    for (WN_UINT32 k = i + 1; k < Order; ++k) {
                        const Type factor = matrix.mElements.mValues[(k * Order) + i] / matrix.mElements.mValues[(i * Order) + i];

                        for (WN_UINT32 j = i; j < Order; ++j) {
                            matrix.mElements.mValues[(k * Order) + j] -= (factor * matrix.mElements.mValues[(i * Order) + j]);
                        }
                    }
                }
            }

            Type determinant = static_cast<Type>(1);

            for (WN_UINT32 i = 0; i < Order; ++i) {
                determinant *= matrix.mElements.mValues[(i * Order) + i];
            }

            if (count % 2 == 0) {
                return(determinant);
            } else {
                return(static_cast<Type>(-1) * determinant);
            }
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_BOOL __WNMatrixBaseCommon<MatrixType, Type, Order>::IsIdentity() const {
            for (WN_UINT32 i = 0; i < Order; ++i) {
                for (WN_UINT32 j = 0; j < Order; ++j) {
                    if (i == j) {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<Type>(1)) {
                            return(WN_FALSE);
                        }
                    } else {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<Type>(0)) {
                            return(WN_FALSE);
                        }
                    }
                }
            }
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_BOOL __WNMatrixBaseCommon<MatrixType, Type, Order>::IsSymmetric() const {
            return(WN_FALSE);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_BOOL __WNMatrixBaseCommon<MatrixType, Type, Order>::IsDiagonal() const {
            for (WN_UINT32 i = 0; i < Order; ++i) {
                for (WN_UINT32 j = 0; j < Order; ++j) {
                    if (i != j) {
                        if (this->mElements.mValues[(j * Order) + i] != static_cast<Type>(0)) {
                            return(WN_FALSE);
                        }
                    }
                }
            }
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Set(const MatrixType& _matrix) {
            *static_cast<MatrixType*>(this) = _matrix;
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Set(const Type& _number) {
            this->mElements = __WNGeneralOpSetNumber<Type, Dimension>::Execute(this->mElements, _number);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE WN_VOID __WNMatrixBaseCommon<MatrixType, Type, Order>::Set(const Type* _numbers) {
            this->mElements = __WNGeneralOpSetNumbers<Type, Dimension>::Execute(this->mElements, _numbers);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::GetTransposed() const {
            MatrixType matrix = *static_cast<MatrixType*>(this);

            return(matrix.Transpose(), matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::CreateZero() {
            MatrixType matrix;

            return(matrix.Zero(), matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBaseCommon<MatrixType, Type, Order>::CreateIdentity() {
            MatrixType matrix;

            return(matrix.Identity(), matrix);
        }

        template <typename MatrixType, typename Type, WN_UINT32 Order>
        WN_FORCE_INLINE MatrixType __WNMatrixBase<MatrixType, Type, Order, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::GetInverted() const {
            MatrixType matrix = *static_cast<MatrixType*>(this);

            return(matrix.Invert(), matrix);
        }
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_BASE_INL__