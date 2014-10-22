// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_2_INL__
#define __WN_MATH_INTERNAL_MATRIX_2_INL__

#ifndef __WN_MATH_MATRIX_2_H__
    #error "Internal/WNMatrix2.inl should never be included directly. Please include WNMatrix2.h instead"
#endif

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type>::WNMatrix2(const WNMatrix2& _matrix) {
        Base::Set(_matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type>::WNMatrix2(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type>::WNMatrix2(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type> operator + (const Type& _number, const WNMatrix2<Type>& _matrix) {
        WNMatrix2<Type> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type> operator - (const Type& _number, const WNMatrix2<Type>& _matrix) {
        WNMatrix2<Type> matrix = WNMatrix2<Type>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type> operator * (const Type& _number, const WNMatrix2<Type>& _matrix) {
        WNMatrix2<Type> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix2<Type> operator / (const Type& _number, const WNMatrix2<Type>& _matrix) {
        WNMatrix2<Type> matrix = WNMatrix2<Type>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_2_INL__