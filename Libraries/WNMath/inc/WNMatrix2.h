// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_MATRIX_2_H__
#define __WN_MATH_MATRIX_2_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace WNMath {
    template <typename Type>
    class WNMatrix2 : public __WNInternal::__WNMatrixBase<WNMatrix2<Type>, Type, 2> {
    public:
        WN_FORCE_INLINE WNMatrix2() {}
        WNMatrix2(const WNMatrix2& _matrix);
        explicit WNMatrix2(const Type& _number);
        explicit WNMatrix2(const Type* _numbers);

    private:
        typedef class __WNInternal::__WNMatrixBase<WNMatrix2<Type>, Type, 2> Base;
    };

    template <typename Type>
    WNMatrix2<Type> operator + (const Type& _number, const WNMatrix2<Type>& _matrix);

    template <typename Type>
    WNMatrix2<Type> operator - (const Type& _number, const WNMatrix2<Type>& _matrix);

    template <typename Type>
    WNMatrix2<Type> operator * (const Type& _number, const WNMatrix2<Type>& _matrix);

    template <typename Type>
    WNMatrix2<Type> operator / (const Type& _number, const WNMatrix2<Type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix2.inl"

#endif // __WN_MATH_MATRIX_2_H__