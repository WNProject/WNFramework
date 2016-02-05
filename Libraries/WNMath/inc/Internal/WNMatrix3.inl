// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_INL__
#define __WN_MATH_INTERNAL_MATRIX_3_INL__

#ifndef __WN_MATH_MATRIX_3_H__
    #error "Internal/WNMatrix3.inl should never be included directly. Please include WNMatrix3.h instead"
#endif

namespace wn {
    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type>::WNMatrix3(const WNMatrix3& _matrix) {
        Base::Set(_matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type>::WNMatrix3(const type& _number) {
        Base::Set(_number);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type>::WNMatrix3(const type* _numbers) {
        Base::Set(_numbers);
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeTranslation(const type& _translation) {
        this->MakeTranslation(_translation, _translation);
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeTranslation(const type& _x, const type& _y) {
        // First Column
        this->mElements.mValues[0] = static_cast<type>(1);
        this->mElements.mValues[1] = static_cast<type>(0);
        this->mElements.mValues[2] = static_cast<type>(0);

        // Second Column
        this->mElements.mValues[3] = static_cast<type>(0);
        this->mElements.mValues[4] = static_cast<type>(1);
        this->mElements.mValues[5] = static_cast<type>(0);

        // Third Column
        this->mElements.mValues[6] = _x;
        this->mElements.mValues[7] = _y;
        this->mElements.mValues[8] = static_cast<type>(1);
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeTranslation(const WNVector2<type>& _translation) {
        this->MakeScale(_translation.GetX(), _translation.GetY());
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeScale(const type& _scale) {
        this->MakeScale(_scale, _scale);
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeScale(const type& _x, const type& _y) {
        // First Column
        this->mElements.mValues[0] = _x;
        this->mElements.mValues[1] = static_cast<type>(0);
        this->mElements.mValues[2] = static_cast<type>(0);

        // Second Column
        this->mElements.mValues[3] = static_cast<type>(0);
        this->mElements.mValues[4] = _y;
        this->mElements.mValues[5] = static_cast<type>(0);

        // Third Column
        this->mElements.mValues[6] = static_cast<type>(0);
        this->mElements.mValues[7] = static_cast<type>(0);
        this->mElements.mValues[8] = static_cast<type>(1);
    }

    template <typename type>
    WN_FORCE_INLINE void WNMatrix3<type>::MakeScale(const WNVector2<type>& _scale) {
        this->MakeScale(_scale.GetX(), _scale.GetY());
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> WNMatrix3<type>::CreateTranslation(const type& _translation) {
        WNMatrix3<type> matrix;

        return(matrix.MakeTranslation(_translation), matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> WNMatrix3<type>::CreateTranslation(const type& _x, const type& _y) {
        WNMatrix3<type> matrix;

        return(matrix.MakeTranslation(_x, _y), matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> WNMatrix3<type>::CreateScale(const type& _scale) {
        WNMatrix3<type> matrix;

        return(matrix.MakeScale(_scale), matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> WNMatrix3<type>::CreateScale(const type& _x, const type& _y) {
        WNMatrix3<type> matrix;

        return(matrix.MakeScale(_x, _y), matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> operator + (const type& _number, const WNMatrix3<type>& _matrix) {
        WNMatrix3<type> matrix = _matrix;

        matrix += _number;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> operator - (const type& _number, const WNMatrix3<type>& _matrix) {
        WNMatrix3<type> matrix = WNMatrix3<type>(_number);

        matrix -= _matrix;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> operator * (const type& _number, const WNMatrix3<type>& _matrix) {
        WNMatrix3<type> matrix = _matrix;

        matrix *= _number;

        return(matrix);
    }

    template <typename type>
    WN_FORCE_INLINE WNMatrix3<type> operator / (const type& _number, const WNMatrix3<type>& _matrix) {
        WNMatrix3<type> matrix = WNMatrix3<type>(_number);

        matrix /= _matrix;

        return(matrix);
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_3_INL__