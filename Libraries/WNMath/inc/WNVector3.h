// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_VECTOR_3_H__
#define __WN_MATH_VECTOR_3_H__

#include "WNMath/inc/Internal/WNVectorBase.h"

namespace WNMath {
    template <typename Type>
    class WNVector2;

    template <typename Type>
    class WNVector4;

    template <typename Type, WN_UINT32 Dimension>
    class WNVectorX;

    template <typename Type>
    class WNVector3 : public __WNInternal::__WNVectorBase<WNVector3<Type>, Type, 3> {
    public:
        WN_FORCE_INLINE WNVector3() {}
        WNVector3(const WNVector3& _vector);
        explicit WNVector3(const Type& _number);
        explicit WNVector3(const Type* _numbers);
        explicit WNVector3(const WNVector2<Type>& _vector, const Type _w);
        explicit WNVector3(const WNVector4<Type>& _vector);
        explicit WNVector3(const WNVectorX<Type, 3>& _vector);
        explicit WNVector3(const Type& _x, const Type& _y, const Type& _z);

        WNVector3 Cross(const WNVector3& _vector) const;
        WNVectorX<Type, 3> Cross(const WNVectorX<Type, 3>& _vector) const;

        template <WN_UINT8 Index1, WN_UINT8 Index2, WN_UINT8 Index3>
        WN_FORCE_INLINE WN_VOID Swizzle() {}
        WN_FORCE_INLINE WN_VOID Swizzle(const WN_UINT8 _index1, const WN_UINT8 _index2, const WN_UINT8 _index3) {}

        WN_VOID Set(const WNVector2<Type>& _vector, const Type _w);
        WN_VOID Set(const WNVector4<Type>& _vector);
        WN_VOID Set(const WNVectorX<Type, 3>& _vector);
        WN_VOID Set(const Type& _x, const Type& _y, const Type& _z);

        const Type& GetX() const;
        const Type& GetY() const;
        const Type& GetZ() const;

        WNVector2<Type> ToVector2() const;
        WNVector4<Type> ToVector4(const Type& _number = static_cast<Type>(0)) const;

        template <typename NewType>
        WNVector3<NewType> ConvertTo() const;

    private:
        template <typename OtherType> friend class WNVector3;

    private:
        typedef class __WNInternal::__WNVectorBase<WNVector3<Type>, Type, 3> Base;
    };

    template <typename Type>
    WNVector3<Type> operator + (const Type& _number, const WNVector3<Type>& _vector);

    template <typename Type>
    WNVector3<Type> operator - (const Type& _number, const WNVector3<Type>& _vector);

    template <typename Type>
    WNVector3<Type> operator * (const Type& _number, const WNVector3<Type>& _vector);

    template <typename Type>
    WNVector3<Type> operator / (const Type& _number, const WNVector3<Type>& _vector);
}

#include "WNMath/inc/Internal/WNVector3.inl"

#endif // __WN_MATH_VECTOR_3_H__
