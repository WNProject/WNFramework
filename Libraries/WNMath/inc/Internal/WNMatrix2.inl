// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_2_INL__
#define __WN_MATH_INTERNAL_MATRIX_2_INL__

#ifndef __WN_MATH_MATRIX_2_H__
    #error "Internal/WNMatrix2.inl should never be included directly. Please include WNMatrix2.h instead"
#endif

namespace wn {
    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type>::WNMatrix2(const WNMatrix2& _matrix) {
        Base::Set(_matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type>::WNMatrix2(const type& _number) {
        Base::Set(_number);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type>::WNMatrix2(const type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type> operator + (const type& _number, const WNMatrix2<type>& _matrix) {
        WNMatrix2<type> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type> operator - (const type& _number, const WNMatrix2<type>& _matrix) {
        WNMatrix2<type> matrix = WNMatrix2<type>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type> operator * (const type& _number, const WNMatrix2<type>& _matrix) {
        WNMatrix2<type> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix2<type> operator / (const type& _number, const WNMatrix2<type>& _matrix) {
        WNMatrix2<type> matrix = WNMatrix2<type>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_2_INL__