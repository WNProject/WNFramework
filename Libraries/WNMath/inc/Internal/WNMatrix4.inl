////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_INL__
#define __WN_MATH_INTERNAL_MATRIX_4_INL__

#ifndef __WN_MATH_MATRIX_4_H__
    #error "Internal/WNMatrix4.inl should never be included directly. Please include WNMatrix4.h instead"
#endif

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type>::WNMatrix4(const WNMatrix4& _matrix) {
        Base::Set(_matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type>::WNMatrix4(const Type& _number) {
        Base::Set(_number);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type>::WNMatrix4(const Type* _numbers) {
        Base::Set(_numbers);
    }
    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeTranslation(const Type& _translation) {
       this->MakeTranslation(_translation, _translation, _translation);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeTranslation(const WNVector3<Type>& _translation) {
       this->MakeTranslation(_translation.GetX(), _translation.GetY(), _translation.GetZ());
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeTranslation(const Type& _x, const Type& _y, const Type& _z) {
        // First Column
        this->mElements.mValues[0] = static_cast<Type>(1);
        this->mElements.mValues[1] = static_cast<Type>(0);
        this->mElements.mValues[2] = static_cast<Type>(0);
        this->mElements.mValues[3] = static_cast<Type>(0);

        // Second Column
        this->mElements.mValues[4] = static_cast<Type>(0);
        this->mElements.mValues[5] = static_cast<Type>(1);
        this->mElements.mValues[6] = static_cast<Type>(0);
        this->mElements.mValues[7] = static_cast<Type>(0);

        // Third Column
        this->mElements.mValues[8] = static_cast<Type>(0);
        this->mElements.mValues[9] = static_cast<Type>(0);
        this->mElements.mValues[10] = static_cast<Type>(1);
        this->mElements.mValues[11] = static_cast<Type>(0);

        // Forth Column
        this->mElements.mValues[13] = _x;
        this->mElements.mValues[14] = _y;
        this->mElements.mValues[15] = _z;
        this->mElements.mValues[15] = static_cast<Type>(1);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeScale(const Type& _scale) {
        this->MakeScale(_scale, _scale, _scale);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeScale(const WNVector3<Type>& _scale) {
        this->MakeScale(_scale.GetX(), _scale.GetY(), _scale.GetZ());
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNMatrix4<Type>::MakeScale(const Type& _x, const Type& _y, const Type& _z) {
        // First Column
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = static_cast<Type>(0);
        this->mElements.mValues[2] = static_cast<Type>(0);
        this->mElements.mValues[3] = static_cast<Type>(0);

        // Second Column
        this->mElements.mValues[4] = static_cast<Type>(0);
        this->mElements.mValues[5] = _y;
        this->mElements.mValues[6] = static_cast<Type>(0);
        this->mElements.mValues[7] = static_cast<Type>(0);

        // Third Column
        this->mElements.mValues[8] = static_cast<Type>(0);
        this->mElements.mValues[9] = static_cast<Type>(0);
        this->mElements.mValues[10] = _z;
        this->mElements.mValues[11] = static_cast<Type>(0);

        // Forth Column
        this->mElements.mValues[13] = static_cast<Type>(0);
        this->mElements.mValues[14] = static_cast<Type>(0);
        this->mElements.mValues[15] = static_cast<Type>(0);
        this->mElements.mValues[15] = static_cast<Type>(1);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateTranslation(const Type& _translation) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeTranslation(_translation), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateTranslation(const WNVector3<Type>& _translation) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeTranslation(_translation), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateTranslation(const Type& _x, const Type& _y, const Type& _z) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeTranslation(_x, _y, _z), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateScale(const Type& _scale) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeScale(_scale), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateScale(const WNVector3<Type>& _scale) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeScale(_scale), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> WNMatrix4<Type>::CreateScale(const Type& _x, const Type& _y, const Type& _z) {
        WNMatrix4<Type> matrix;

        return(matrix.MakeScale(_x, _y, _z), matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> operator + (const Type& _number, const WNMatrix4<Type>& _matrix) {
        WNMatrix4<Type> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> operator - (const Type& _number, const WNMatrix4<Type>& _matrix) {
        WNMatrix4<Type> matrix = WNMatrix4<Type>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> operator * (const Type& _number, const WNMatrix4<Type>& _matrix) {
        WNMatrix4<Type> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename Type>
    WN_FORCE_INLINE WNMatrix4<Type> operator / (const Type& _number, const WNMatrix4<Type>& _matrix) {
        WNMatrix4<Type> matrix = WNMatrix4<Type>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_4_INL__