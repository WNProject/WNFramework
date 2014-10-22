// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_X_INL__
#define __WN_MATH_INTERNAL_MATRIX_X_INL__

#ifndef __WN_MATH_MATRIX_X_H__
    #error "Internal/WNMatrixX.inl should never be included directly. Please include WNMatrixX.h instead"
#endif

namespace WNMath {
    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order>::WNMatrixX(const WNMatrixX& _vector) {
        Base::Set(_vector);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order>::WNMatrixX(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order>::WNMatrixX(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type, WN_UINT32 Order>
    template <typename NewType>
    WN_FORCE_INLINE WNMatrixX<NewType, Order> WNMatrixX<Type, Order>::ConvertTo() const {
        WNMatrixX<NewType, Order> matrix;

        for (WN_UINT32 i = 0; i < Base::Dimension; ++i) {
            matrix.mElements.mValues[i] = static_cast<NewType>(this->mElements.mValues[i]);
        }

        return(matrix);
    }

    template <typename Type, WN_UINT32 Order>
    template <typename NewType, WN_UINT32 NewOrder>
    WN_FORCE_INLINE WNMatrixX<NewType, NewOrder> WNMatrixX<Type, Order>::ConvertTo() const {
        WNMatrixX<NewType, NewOrder> matrix;

        for (WN_UINT32 i = 0; i < NewOrder; ++i) {
            for (WN_UINT32 j = 0; j < NewOrder; ++j) {
                if (i < Order && j < Order) {
                    matrix.mElements.mValues[(i * NewOrder) + j] = static_cast<NewType>(this->mElements.mValues[(i * Order) + j]);
                } else {
                    matrix.mElements.mValues[(i * NewOrder) + j] = static_cast<NewType>(0);
                }
            }
        }

        return(matrix);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order> operator + (const Type& _number, const WNMatrixX<Type, Order>& _matrix) {
        WNMatrixX<Type, Order> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order> operator - (const Type& _number, const WNMatrixX<Type, Order>& _matrix) {
        WNMatrixX<Type, Order> matrix = WNMatrixX<Type, Order>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order> operator * (const Type& _number, const WNMatrixX<Type, Order>& _matrix) {
        WNMatrixX<Type, Order> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename Type, WN_UINT32 Order>
    WN_FORCE_INLINE WNMatrixX<Type, Order> operator / (const Type& _number, const WNMatrixX<Type, Order>& _matrix) {
        WNMatrixX<Type, Order> matrix = WNMatrixX<Type, Order>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_X_INL__