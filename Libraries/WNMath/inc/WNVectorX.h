////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_VECTOR_X_H__
#define __WN_MATH_VECTOR_X_H__

#include "WNMath/inc/Internal/WNVectorBase.h"

namespace WNMath {
    template <typename Type, WN_UINT32 Dimension>
    class WNVectorX : public __WNInternal::__WNVectorBase<WNVectorX<Type, Dimension>, Type, Dimension> {
        WN_STATIC_ASSERT_DESC(Dimension > 0, "You cannot have a vector of 0 dimension.  Please use a value of 1 or greater.");

    public:
        WN_FORCE_INLINE WNVectorX() {}
        WNVectorX(const WNVectorX& _vector);
        explicit WNVectorX(const Type& _number);
        explicit WNVectorX(const Type* _numbers);

        template <typename NewType>
        WNVectorX<NewType, Dimension> ConvertTo() const;

        template <typename NewType, WN_UINT32 NewDimension>
        WNVectorX<NewType, NewDimension> ConvertTo() const;

    private:
        template <typename OtherType, WN_UINT32 OtherDimension> friend class WNVectorX;

    private:
        typedef class __WNInternal::__WNVectorBase<WNVectorX<Type, Dimension>, Type, Dimension> Base;
    };

    template <typename Type, WN_UINT32 Dimension>
    WNVectorX<Type, Dimension> operator + (const Type& _number, const WNVectorX<Type, Dimension>& _vector);

    template <typename Type, WN_UINT32 Dimension>
    WNVectorX<Type, Dimension> operator - (const Type& _number, const WNVectorX<Type, Dimension>& _vector);

    template <typename Type, WN_UINT32 Dimension>
    WNVectorX<Type, Dimension> operator * (const Type& _number, const WNVectorX<Type, Dimension>& _vector);

    template <typename Type, WN_UINT32 Dimension>
    WNVectorX<Type, Dimension> operator / (const Type& _number, const WNVectorX<Type, Dimension>& _vector);
}

#include "WNMath/inc/Internal/WNVectorX.inl"

#endif // __WN_MATH_VECTOR_X_H__