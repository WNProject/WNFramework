////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_MATRIX_X_H__
#define __WN_MATH_MATRIX_X_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace WNMath {
    template <typename Type, WN_UINT32 Order>
    class WNMatrixX : public __WNInternal::__WNMatrixBase<WNMatrixX<Type, Order>, Type, Order> {
        WN_STATIC_ASSERT_DESC(Order > 0, "You cannot have a matrix of 0 order.  Please use a value of 1 or greater.");

    public:
        WN_FORCE_INLINE WNMatrixX() {}
        WNMatrixX(const WNMatrixX& _matrix);
        explicit WNMatrixX(const Type& _number);
        explicit WNMatrixX(const Type* _numbers);

        template <typename NewType>
        WNMatrixX<NewType, Order> ConvertTo() const;

        template <typename NewType, WN_UINT32 NewOrder>
        WNMatrixX<NewType, NewOrder> ConvertTo() const;

    private:
        typedef class __WNInternal::__WNMatrixBase<WNMatrixX<Type, Order>, Type, Order> Base;
    };

    template <typename Type, WN_UINT32 Dimension>
    WNMatrixX<Type, Dimension> operator + (const Type& _number, const WNMatrixX<Type, Dimension>& _matrix);

    template <typename Type, WN_UINT32 Dimension>
    WNMatrixX<Type, Dimension> operator - (const Type& _number, const WNMatrixX<Type, Dimension>& _matrix);

    template <typename Type, WN_UINT32 Dimension>
    WNMatrixX<Type, Dimension> operator * (const Type& _number, const WNMatrixX<Type, Dimension>& _matrix);

    template <typename Type, WN_UINT32 Dimension>
    WNMatrixX<Type, Dimension> operator / (const Type& _number, const WNMatrixX<Type, Dimension>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrixX.inl"

#endif // __WN_MATH_MATRIX_X_H__