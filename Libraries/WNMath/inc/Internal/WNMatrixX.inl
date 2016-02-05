// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_X_INL__
#define __WN_MATH_INTERNAL_MATRIX_X_INL__

#ifndef __WN_MATH_MATRIX_X_H__
    #error "Internal/WNMatrixX.inl should never be included directly. Please include WNMatrixX.h instead"
#endif

namespace wn {
    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order>::WNMatrixX(const WNMatrixX& _vector) {
        Base::Set(_vector);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order>::WNMatrixX(const type& _number) {
        Base::Set(_number);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order>::WNMatrixX(const type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename type, uint32_t Order>
    template <typename NewType>
    WN_FORCE_INLINE WNMatrixX<NewType, Order> WNMatrixX<type, Order>::ConvertTo() const {
        WNMatrixX<NewType, Order> matrix;

        for (uint32_t i = 0; i < Base::dimension; ++i) {
            matrix.mElements.mValues[i] = static_cast<NewType>(this->mElements.mValues[i]);
        }

        return(matrix);
    }

    template <typename type, uint32_t Order>
    template <typename NewType, uint32_t NewOrder>
    WN_FORCE_INLINE WNMatrixX<NewType, NewOrder> WNMatrixX<type, Order>::ConvertTo() const {
        WNMatrixX<NewType, NewOrder> matrix;

        for (uint32_t i = 0; i < NewOrder; ++i) {
            for (uint32_t j = 0; j < NewOrder; ++j) {
                if (i < Order && j < Order) {
                    matrix.mElements.mValues[(i * NewOrder) + j] = static_cast<NewType>(this->mElements.mValues[(i * Order) + j]);
                } else {
                    matrix.mElements.mValues[(i * NewOrder) + j] = static_cast<NewType>(0);
                }
            }
        }

        return(matrix);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order> operator + (const type& _number, const WNMatrixX<type, Order>& _matrix) {
        WNMatrixX<type, Order> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order> operator - (const type& _number, const WNMatrixX<type, Order>& _matrix) {
        WNMatrixX<type, Order> matrix = WNMatrixX<type, Order>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order> operator * (const type& _number, const WNMatrixX<type, Order>& _matrix) {
        WNMatrixX<type, Order> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename type, uint32_t Order>
    WN_FORCE_INLINE WNMatrixX<type, Order> operator / (const type& _number, const WNMatrixX<type, Order>& _matrix) {
        WNMatrixX<type, Order> matrix = WNMatrixX<type, Order>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_X_INL__