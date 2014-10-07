////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_INL__
#define __WN_MATH_INTERNAL_MATRIX_3_INL__

#ifndef __WN_MATH_MATRIX_3_H__
    #error "Internal/WNMatrix3.inl should never be included directly. Please include WNMatrix3.h instead"
#endif

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type>::WNMatrix3(const WNMatrix3& _matrix) {
        Base::Set(_matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type>::WNMatrix3(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type>::WNMatrix3(const Type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeTranslation(const Type& _translation) {
        this->MakeTranslation(_translation, _translation);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeTranslation(const Type& _x, const Type& _y) {
        // First Column
        this->mElements.mValues[0] = static_cast<Type>(1);
        this->mElements.mValues[1] = static_cast<Type>(0);
        this->mElements.mValues[2] = static_cast<Type>(0);

        // Second Column
        this->mElements.mValues[3] = static_cast<Type>(0);
        this->mElements.mValues[4] = static_cast<Type>(1);
        this->mElements.mValues[5] = static_cast<Type>(0);

        // Third Column
        this->mElements.mValues[6] = _x;
        this->mElements.mValues[7] = _y;
        this->mElements.mValues[8] = static_cast<Type>(1);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeTranslation(const WNVector2<Type>& _translation) {
        this->MakeScale(_translation.GetX(), _translation.GetY());
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeScale(const Type& _scale) {
        this->MakeScale(_scale, _scale);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeScale(const Type& _x, const Type& _y) {
        // First Column
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = static_cast<Type>(0);
        this->mElements.mValues[2] = static_cast<Type>(0);

        // Second Column
        this->mElements.mValues[3] = static_cast<Type>(0);
        this->mElements.mValues[4] = _y;
        this->mElements.mValues[5] = static_cast<Type>(0);

        // Third Column
        this->mElements.mValues[6] = static_cast<Type>(0);
        this->mElements.mValues[7] = static_cast<Type>(0);
        this->mElements.mValues[8] = static_cast<Type>(1);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix3<Type>::MakeScale(const WNVector2<Type>& _scale) {
        this->MakeScale(_scale.GetX(), _scale.GetY());
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> WNMatrix3<Type>::CreateTranslation(const Type& _translation) {
        WNMatrix3<Type> matrix;

        return(matrix.MakeTranslation(_translation), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> WNMatrix3<Type>::CreateTranslation(const Type& _x, const Type& _y) {
        WNMatrix3<Type> matrix;

        return(matrix.MakeTranslation(_x, _y), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> WNMatrix3<Type>::CreateScale(const Type& _scale) {
        WNMatrix3<Type> matrix;

        return(matrix.MakeScale(_scale), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> WNMatrix3<Type>::CreateScale(const Type& _x, const Type& _y) {
        WNMatrix3<Type> matrix;

        return(matrix.MakeScale(_x, _y), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> operator + (const Type& _number, const WNMatrix3<Type>& _matrix) {
        WNMatrix3<Type> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> operator - (const Type& _number, const WNMatrix3<Type>& _matrix) {
        WNMatrix3<Type> matrix = WNMatrix3<Type>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> operator * (const Type& _number, const WNMatrix3<Type>& _matrix) {
        WNMatrix3<Type> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix3<Type> operator / (const Type& _number, const WNMatrix3<Type>& _matrix) {
        WNMatrix3<Type> matrix = WNMatrix3<Type>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_3_INL__