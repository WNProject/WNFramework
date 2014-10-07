////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_3_INL__
#define __WN_MATH_INTERNAL_VECTOR_3_INL__

#ifndef __WN_MATH_VECTOR_3_H__
    #error "Internal/WNVector3.inl should never be included directly. Please include WNVector3.h instead"
#endif

#include "WNMath/inc/WNVector2.h"
#include "WNMath/inc/WNVector4.h"
#include "WNMath/inc/WNVectorX.h"

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const WNVector3<Type>& _vector) {
        Base::Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const WNVector2<Type>& _vector, const Type _w) {
        this->Set(_vector, _w);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const WNVector4<Type>& _vector) {
        this->Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const WNVectorX<Type, 3>& _vector) {
        this->Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type>::WNVector3(const Type& _x, const Type& _y, const Type& _z) {
        this->Set(_x, _y, _z);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> WNVector3<Type>::Cross(const WNVector3<Type>& _vector) const {

    }

    template <typename Type>
    WN_FORCE_INLINE WNVectorX<Type, 3> WNVector3<Type>::Cross(const WNVectorX<Type, 3>& _vector) const {

    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector3<Type>::Set(const WNVector2<Type>& _vector, const Type _w) {
        this->mElements.mValues[0] = _vector.GetX();
        this->mElements.mValues[1] = _vector.GetY();
        this->mElements.mValues[2] = _w;
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector3<Type>::Set(const WNVector4<Type>& _vector) {
        this->mElements.mValues[0] = _vector.GetX();
        this->mElements.mValues[1] = _vector.GetY();
        this->mElements.mValues[2] = _vector.GetZ();
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector3<Type>::Set(const WNVectorX<Type, 3>& _vector) {
        this->mElements.mValues[0] = _vector.mElements.mValues[0];
        this->mElements.mValues[1] = _vector.mElements.mValues[1];
        this->mElements.mValues[2] = _vector.mElements.mValues[2];
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector3<Type>::Set(const Type& _x, const Type& _y, const Type& _z) {
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = _y;
        this->mElements.mValues[2] = _z;
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector3<Type>::GetX() const {
        return(this->mElements.mValues[0]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector3<Type>::GetY() const {
        return(this->mElements.mValues[1]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector3<Type>::GetZ() const {
        return(this->mElements.mValues[2]);
    }

    template <typename Type>
    template <typename NewType>
    WN_FORCE_INLINE WNVector3<NewType> WNVector3<Type>::ConvertTo() const {
        return(WNVector3<NewType>(static_cast<NewType>(this->GetX()),
                                  static_cast<NewType>(this->GetY()),
                                  static_cast<NewType>(this->GetZ())));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> WNVector3<Type>::ToVector2() const {
        return(WNVector2<Type>(this->GetX(), this->GetY()));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> WNVector3<Type>::ToVector4(const Type& _number) const {
        return(WNVector4<Type>(this->GetX(), this->GetY(), this->GetZ(), _number));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> operator + (const Type& _number, const WNVector3<Type>& _vector) {
        WNVector3<Type> vector = _vector;

        vector += _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> operator - (const Type& _number, const WNVector3<Type>& _vector) {
        WNVector3<Type> vector = WNVector3<Type>(_number);

        vector -= _vector;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> operator * (const Type& _number, const WNVector3<Type>& _vector) {
        WNVector3<Type> vector = _vector;

        vector *= _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> operator / (const Type& _number, const WNVector3<Type>& _vector) {
        WNVector3<Type> vector = WNVector3<Type>(_number);

        vector /= _vector;

        return(vector);
    }
}

#endif // __WN_MATH_INTERNAL_VECTOR_3_INL__