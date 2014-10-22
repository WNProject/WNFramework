// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_VECTOR_2_H__
#define __WN_MATH_VECTOR_2_H__

#include "WNMath/inc/Internal/WNVectorBase.h"

namespace WNMath {
    template <typename Type>
    class WNVector3;

    template <typename Type>
    class WNVector4;

    template <typename Type, WN_UINT32 Dimension>
    class WNVectorX;

    template <typename Type>
    class WNVector2 : public __WNInternal::__WNVectorBase<WNVector2<Type>, Type, 2> {
    public:
        WN_FORCE_INLINE WNVector2() {}
        WNVector2(const WNVector2& _vector);
        explicit WNVector2(const Type& _number);
        explicit WNVector2(const Type* _numbers);
        explicit WNVector2(const WNVector3<Type>& _vector);
        explicit WNVector2(const WNVector4<Type>& _vector);
        explicit WNVector2(const WNVectorX<Type, 2>& _vector);
        explicit WNVector2(const Type& _x, const Type& _y);

        template <WN_UINT8 Index1, WN_UINT8 Index2>
        WN_FORCE_INLINE WN_VOID Swizzle() {}
        WN_FORCE_INLINE WN_VOID Swizzle(const WN_UINT8 _index1, const WN_UINT8 _index2) {}

        WN_VOID Set(const WNVector3<Type>& _vector);
        WN_VOID Set(const WNVector4<Type>& _vector);
        WN_VOID Set(const WNVectorX<Type, 2>& _vector);
        WN_VOID Set(const Type& _x, const Type& _y);

        const Type& GetX() const;
        const Type& GetY() const;

        template <typename NewType>
        WNVector2<NewType> ConvertTo() const;

    private:
        template <typename OtherType> friend class WNVector2;

    private:
        typedef class __WNInternal::__WNVectorBase<WNVector2<Type>, Type, 2> Base;
    };

    template <typename Type>
    WNVector2<Type> operator + (const Type& _number, const WNVector2<Type>& _vector);

    template <typename Type>
    WNVector2<Type> operator - (const Type& _number, const WNVector2<Type>& _vector);

    template <typename Type>
    WNVector2<Type> operator * (const Type& _number, const WNVector2<Type>& _vector);

    template <typename Type>
    WNVector2<Type> operator / (const Type& _number, const WNVector2<Type>& _vector);
}

#include "WNMath/inc/Internal/WNVector2.inl"

#endif // __WN_MATH_VECTOR_2_H__
