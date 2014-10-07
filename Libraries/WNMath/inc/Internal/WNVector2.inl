////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_2_INL__
#define __WN_MATH_INTERNAL_VECTOR_2_INL__

#ifndef __WN_MATH_VECTOR_2_H__
    #error "Internal/WNVector2.inl should never be included directly. Please include WNVector2.h instead"
#endif

#include "WNMath/inc/WNVector2.h"
#include "WNMath/inc/WNVector4.h"
#include "WNMath/inc/WNVectorX.h"

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const WNVector2<Type>& _vector) {
        Base::Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const WNVector3<Type>& _vector) {
        this->Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const WNVector4<Type>& _vector) {
        this->Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const WNVectorX<Type, 2>& _vector) {
        this->Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type>::WNVector2(const Type& _x, const Type& _y) {
        this->Set(_x, _y);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector2<Type>::Set(const WNVector3<Type>& _vector) {
        this->mElements.mValues[0] = _vector.GetX();
        this->mElements.mValues[1] = _vector.GetY();
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector2<Type>::Set(const WNVector4<Type>& _vector) {
        this->mElements.mValues[0] = _vector.GetX();
        this->mElements.mValues[1] = _vector.GetY();
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector2<Type>::Set(const WNVectorX<Type, 2>& _vector) {
        this->mElements.mValues[0] = _vector.mElements.mValues[0];
        this->mElements.mValues[1] = _vector.mElements.mValues[1];
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector2<Type>::Set(const Type& _x, const Type& _y) {
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = _y;
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector2<Type>::GetX() const {
        return(this->mElements.mValues[0]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector2<Type>::GetY() const {
        return(this->mElements.mValues[1]);
    }

    template <typename Type>
    template <typename NewType>
    WN_FORCE_INLINE WNVector2<NewType> WNVector2<Type>::ConvertTo() const {
        return(WNVector2<NewType>(static_cast<NewType>(this->GetX()),
                                  static_cast<NewType>(this->GetY())));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> operator + (const Type& _number, const WNVector2<Type>& _vector) {
        WNVector2<Type> vector = _vector;

        vector += _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> operator - (const Type& _number, const WNVector2<Type>& _vector) {
        WNVector2<Type> vector = WNVector2<Type>(_number);

        vector -= _vector;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> operator * (const Type& _number, const WNVector2<Type>& _vector) {
        WNVector2<Type> vector = _vector;

        vector *= _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> operator / (const Type& _number, const WNVector2<Type>& _vector) {
        WNVector2<Type> vector = WNVector2<Type>(_number);

        vector /= _vector;

        return(vector);
    }
}

#endif // __WN_MATH_INTERNAL_VECTOR_2_INL__