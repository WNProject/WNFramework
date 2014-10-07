////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_4_INL__
#define __WN_MATH_INTERNAL_VECTOR_4_INL__

#ifndef __WN_MATH_VECTOR_4_H__
    #error "WNVector4.inl should never be included directly. Please include WNVector4.h instead"
#endif

#include "WNMath/inc/WNVector3.h"
#include "WNMath/inc/WNVectorX.h"

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const WNVector4<Type>& _vector) {
        Base::Set(_vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const WNVector2<Type>& _vector1, const WNVector2<Type>& _vector2) {
        this->Set(_vector1, _vector2);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const WNVector3<Type>& _vector, const Type _w) {
        this->Set(_vector, _w);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const WNVectorX<Type, 4>& _vector) {
        this->Set(_vector);
    }
    \
    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type>::WNVector4(const Type& _x, const Type& _y, const Type& _z, const Type& _w) {
        this->Set(_x, _y, _z, _w);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> WNVector4<Type>::Cross(const WNVector4<Type>& _vector1, const WNVector4<Type>& _vector2) const {

    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> WNVector4<Type>::Cross(const WNVectorX<Type, 4>& _vector1, const WNVectorX<Type, 4>& _vector2) const {

    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector4<Type>::Set(const WNVector2<Type>& _vector1, const WNVector2<Type>& _vector2) {
        this->mElements.mValues[0] = _vector1.GetX();
        this->mElements.mValues[1] = _vector1.GetY();
        this->mElements.mValues[2] = _vector2.GetX();
        this->mElements.mValues[3] = _vector2.GetY();
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector4<Type>::Set(const WNVector3<Type>& _vector, const Type _w) {
        this->mElements.mValues[0] = _vector.GetX();
        this->mElements.mValues[1] = _vector.GetY();
        this->mElements.mValues[2] = _vector.GetZ();
        this->mElements.mValues[3] = _w;
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector4<Type>::Set(const WNVectorX<Type, 4>& _vector) {
        this->mElements.mValues[0] = _vector.mElements.mValues[0];
        this->mElements.mValues[1] = _vector.mElements.mValues[1];
        this->mElements.mValues[2] = _vector.mElements.mValues[2];
        this->mElements.mValues[3] = _vector.mElements.mValues[3];
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNVector4<Type>::Set(const Type& _x, const Type& _y, const Type& _z, const Type& _w) {
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = _y;
        this->mElements.mValues[2] = _z;
        this->mElements.mValues[3] = _w;
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector4<Type>::GetX() const {
        return(this->mElements.mValues[0]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector4<Type>::GetY() const {
        return(this->mElements.mValues[1]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector4<Type>::GetZ() const {
        return(this->mElements.mValues[2]);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type& WNVector4<Type>::GetW() const {
        return(this->mElements.mValues[3]);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector2<Type> WNVector4<Type>::ToVector2() const {
        return(WNVector2<Type>(this->GetX(), this->GetY()));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector3<Type> WNVector4<Type>::ToVector3() const {
        return(WNVector3<Type>(this->GetX(), this->GetY(), this->GetZ()));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVectorX<Type, 4> WNVector4<Type>::ToVectorX() const {
        WNVectorX<Type, 4> vector;

        vector.mElements.mValues[0] = this->GetX();
        vector.mElements.mValues[1] = this->GetY();
        vector.mElements.mValues[2] = this->GetZ();
        vector.mElements.mValues[3] = this->GetW();

        return(vector);
    }

    template <typename Type>
    template <typename NewType>
    WN_FORCE_INLINE WNVector4<NewType> WNVector4<Type>::ConvertTo() const {
        return(WNVector4<NewType>(static_cast<NewType>(this->GetX()), static_cast<NewType>(this->GetY()),
                                  static_cast<NewType>(this->GetZ()), static_cast<NewType>(this->GetW())));
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> operator + (const Type& _number, const WNVector4<Type>& _vector) {
        WNVector4<Type> vector = _vector;

        vector += _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> operator - (const Type& _number, const WNVector4<Type>& _vector) {
        WNVector4<Type> vector = WNVector4<Type>(_number);

        vector -= _vector;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> operator * (const Type& _number, const WNVector4<Type>& _vector) {
        WNVector4<Type> vector = _vector;

        vector *= _number;

        return(vector);
    }

    template <typename Type>
    WN_FORCE_INLINE WNVector4<Type> operator / (const Type& _number, const WNVector4<Type>& _vector) {
        WNVector4<Type> vector = WNVector4<Type>(_number);

        vector /= _vector;

        return(vector);
    }
}

#endif // __WN_MATH_INTERNAL_VECTOR_4_INL__