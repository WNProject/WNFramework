// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_MATRIX_3_H__
#define __WN_MATH_MATRIX_3_H__

#include "WNMath/inc/Internal/WNMatrix3Base.h"

namespace wn {
    template <typename type>
    class WNMatrix3 : public internal::__WNMatrix3Base<type> {
    public:
        WN_FORCE_INLINE WNMatrix3() {}
        WNMatrix3(const WNMatrix3& _matrix);
        explicit WNMatrix3(const type& _number);
        explicit WNMatrix3(const type* _numbers);

        wn_void MakeTranslation(const type& _translation);
        wn_void MakeTranslation(const type& _x, const type& _y);
        wn_void MakeTranslation(const vector2<type>& _translation);
        wn_void MakeScale(const type& _scale);
        wn_void MakeScale(const type& _x, const type& _y);
        wn_void MakeScale(const vector2<type>& _scale);

    public:
        static WNMatrix3 CreateTranslation(const type& _translation);
        static WNMatrix3 CreateTranslation(const type& _x, const type& _y);
        static WNMatrix3 CreateTranslation(const vector2<type>& _translation);
        static WNMatrix3 CreateScale(const type& _scale);
        static WNMatrix3 CreateScale(const type& _x, const type& _y);
        static WNMatrix3 CreateScale(const vector2<type>& _scale);

    private:
        typedef class internal::__WNMatrixBase<WNMatrix3<type>, type, 3> Base;
    };

    template <typename type>
    WNMatrix3<type> operator + (const type& _number, const WNMatrix3<type>& _matrix);

    template <typename type>
    WNMatrix3<type> operator - (const type& _number, const WNMatrix3<type>& _matrix);

    template <typename type>
    WNMatrix3<type> operator * (const type& _number, const WNMatrix3<type>& _matrix);

    template <typename type>
    WNMatrix3<type> operator / (const type& _number, const WNMatrix3<type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix3.inl"

#endif // __WN_MATH_MATRIX_3_H__