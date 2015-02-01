// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_X_INL__
#define __WN_MATH_INTERNAL_VECTOR_X_INL__

#ifndef __WN_MATH_VECTOR_X_H__
    #error "Internal/WNVectorX.inl should never be included directly. Please include WNVectorX.h instead"
#endif

namespace WNMath {
    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension>::WNVectorX(const WNVectorX& _vector) {
        Base::Set(_vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension>::WNVectorX(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension>::WNVectorX(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type, WN_UINT32 Dimension>
    template <typename NewType>
    WN_FORCE_INLINE WNVectorX<NewType, Dimension> WNVectorX<Type, Dimension>::ConvertTo() const {
        WNVectorX<NewType, Dimension> vector;

        for (WN_UINT32 i = 0; i < Dimension; ++i) {
            vector.mElements.mValues[i] = static_cast<NewType>(this->mElements.mValues[i]);
        }

        return(vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    template <typename NewType, WN_UINT32 NewDimension>
    WN_FORCE_INLINE WNVectorX<NewType, NewDimension> WNVectorX<Type, Dimension>::ConvertTo() const {
        WNVectorX<NewType, NewDimension> vector;
        WN_UINT32 count = 0;

        for (; count < NewDimension; ++count) {
            vector.mElements.mValues[count] = static_cast<NewType>(this->mElements.mValues[count]);
        }

        for (; count < Dimension; ++count) {
            vector.mElements.mValues[count] = static_cast<NewType>(0);
        }

        return(vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension> operator + (const Type& _number, const WNVectorX<Type, Dimension>& _vector) {
        WNVectorX<Type, Dimension> vector = _vector;

        vector += _number;

        return(vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension> operator - (const Type& _number, const WNVectorX<Type, Dimension>& _vector) {
        WNVectorX<Type, Dimension> vector = WNVectorX<Type, Dimension>(_number);

        vector -= _vector;

        return(vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension> operator * (const Type& _number, const WNVectorX<Type, Dimension>& _vector) {
        WNVectorX<Type, Dimension> vector = _vector;

        vector *= _number;

        return(vector);
    }

    template <typename Type, WN_UINT32 Dimension>
    WN_FORCE_INLINE WNVectorX<Type, Dimension> operator / (const Type& _number, const WNVectorX<Type, Dimension>& _vector) {
        WNVectorX<Type, Dimension> vector = WNVectorX<Type, Dimension>(_number);

        vector /= _vector;

        return(vector);
    }
}

#endif // __WN_MATH_INTERNAL_VECTOR_X_INL__