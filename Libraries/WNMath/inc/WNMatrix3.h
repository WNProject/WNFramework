////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_MATRIX_3_H__
#define __WN_MATH_MATRIX_3_H__

#include "WNMath/inc/Internal/WNMatrix3Base.h"

namespace WNMath {
    template <typename Type>
    class WNMatrix3 : public __WNInternal::__WNMatrix3Base<Type> {
    public:
        WN_FORCE_INLINE WNMatrix3() {}
        WNMatrix3(const WNMatrix3& _matrix);
        explicit WNMatrix3(const Type& _number);
        explicit WNMatrix3(const Type* _numbers);

        WN_VOID MakeTranslation(const Type& _translation);
        WN_VOID MakeTranslation(const Type& _x, const Type& _y);
        WN_VOID MakeTranslation(const WNVector2<Type>& _translation);
        WN_VOID MakeScale(const Type& _scale);
        WN_VOID MakeScale(const Type& _x, const Type& _y);
        WN_VOID MakeScale(const WNVector2<Type>& _scale);

    public:
        static WNMatrix3 CreateTranslation(const Type& _translation);
        static WNMatrix3 CreateTranslation(const Type& _x, const Type& _y);
        static WNMatrix3 CreateTranslation(const WNVector2<Type>& _translation);
        static WNMatrix3 CreateScale(const Type& _scale);
        static WNMatrix3 CreateScale(const Type& _x, const Type& _y);
        static WNMatrix3 CreateScale(const WNVector2<Type>& _scale);

    private:
        typedef class __WNInternal::__WNMatrixBase<WNMatrix3<Type>, Type, 3> Base;
    };

    template <typename Type>
    WNMatrix3<Type> operator + (const Type& _number, const WNMatrix3<Type>& _matrix);

    template <typename Type>
    WNMatrix3<Type> operator - (const Type& _number, const WNMatrix3<Type>& _matrix);

    template <typename Type>
    WNMatrix3<Type> operator * (const Type& _number, const WNMatrix3<Type>& _matrix);

    template <typename Type>
    WNMatrix3<Type> operator / (const Type& _number, const WNMatrix3<Type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix3.inl"

#endif // __WN_MATH_MATRIX_3_H__