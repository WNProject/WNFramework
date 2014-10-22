// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_MATRIX_4_H__
#define __WN_MATH_MATRIX_4_H__

#include "WNMath/inc/Internal/WNMatrix4Base.h"

namespace WNMath {
    template <typename Type>
    class WNMatrix4 : public __WNInternal::__WNMatrix4Base<Type> {
    public:
        WN_FORCE_INLINE WNMatrix4() {}
        WNMatrix4(const WNMatrix4& _matrix);
        explicit WNMatrix4(const Type& _number);
        explicit WNMatrix4(const Type* _numbers);

        WN_VOID MakeTranslation(const Type& _translation);
        WN_VOID MakeTranslation(const WNVector3<Type>& _translation);
        WN_VOID MakeTranslation(const Type& _x, const Type& _y, const Type& _z);
        WN_VOID MakeScale(const Type& _scale);
        WN_VOID MakeScale(const WNVector3<Type>& _scale);
        WN_VOID MakeScale(const Type& _x, const Type& _y, const Type& _z);

    public:
        static WNMatrix4 CreateTranslation(const Type& _translation);
        static WNMatrix4 CreateTranslation(const WNVector3<Type>& _translation);
        static WNMatrix4 CreateTranslation(const Type& _x, const Type& _y, const Type& _z);
        static WNMatrix4 CreateScale(const Type& _scale);
        static WNMatrix4 CreateScale(const WNVector3<Type>& _scale);
        static WNMatrix4 CreateScale(const Type& _x, const Type& _y, const Type& _z);

    private:
        typedef class __WNInternal::__WNMatrixBase<WNMatrix4<Type>, Type, 4> Base;
    };

    template <typename Type>
    WNMatrix4<Type> operator + (const Type& _number, const WNMatrix4<Type>& _matrix);

    template <typename Type>
    WNMatrix4<Type> operator - (const Type& _number, const WNMatrix4<Type>& _matrix);

    template <typename Type>
    WNMatrix4<Type> operator * (const Type& _number, const WNMatrix4<Type>& _matrix);

    template <typename Type>
    WNMatrix4<Type> operator / (const Type& _number, const WNMatrix4<Type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix4.inl"

#endif // __WN_MATH_MATRIX_4_H__