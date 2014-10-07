////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_VECTOR_4_H__
#define __WN_MATH_VECTOR_4_H__

#include "WNMath/inc/Internal/WNVectorBase.h"

namespace WNMath {
    template <typename Type> class WNVector2;
    template <typename Type> class WNVector3;
    template <typename Type, WN_UINT32 Dimension> class WNVectorX;

    template <typename Type>
    class WNVector4 : public __WNInternal::__WNVectorBase<WNVector4<Type>, Type, 4> {
    public:
        WN_FORCE_INLINE WNVector4() {}
        WNVector4(const WNVector4& _vector);
        explicit WNVector4(const Type& _number);
        explicit WNVector4(const Type* _numbers);
        explicit WNVector4(const WNVector2<Type>& _vector1, const WNVector2<Type>& _vector2);
        explicit WNVector4(const WNVector3<Type>& _vector, const Type _w);
        explicit WNVector4(const WNVectorX<Type, 4>& _vector);
        explicit WNVector4(const Type& _x, const Type& _y, const Type& _z, const Type& _w);

        WNVector4 Cross(const WNVector4& _vector1, const WNVector4& _vector2) const;
        WNVector4 Cross(const WNVectorX<Type, 4>& _vector1, const WNVectorX<Type, 4>& _vector2) const;

        template <WN_UINT8 Index1, WN_UINT8 Index2, WN_UINT8 Index3, WN_UINT8 Index4>
        WN_FORCE_INLINE WN_VOID Swizzle() {}
        WN_FORCE_INLINE WN_VOID Swizzle(const WN_UINT8 _index1, const WN_UINT8 _index2, const WN_UINT8 _index3, const WN_UINT8 _index4) {}

        WN_VOID Set(const WNVector2<Type>& _vector1, const WNVector2<Type>& _vector2);
        WN_VOID Set(const WNVector3<Type>& _vector, const Type _w);
        WN_VOID Set(const WNVectorX<Type, 4>& _vector);
        WN_VOID Set(const Type& _x, const Type& _y, const Type& _z, const Type& _w);

        const Type& GetX() const;
        const Type& GetY() const;
        const Type& GetZ() const;
        const Type& GetW() const;

        WNVector2<Type> ToVector2() const;
        WNVector3<Type> ToVector3() const;
        WNVectorX<Type, 4> ToVectorX() const;

        template <typename NewType>
        WNVector4<NewType> ConvertTo() const;

    private:
        template <typename OtherType> friend class WNVector4;

    private:
        typedef class __WNInternal::__WNVectorBase<WNVector4<Type>, Type, 4> Base;
    };

    template <typename Type>
    WNVector4<Type> operator + (const Type& _number, const WNVector4<Type>& _vector);

    template <typename Type>
    WNVector4<Type> operator - (const Type& _number, const WNVector4<Type>& _vector);

    template <typename Type>
    WNVector4<Type> operator * (const Type& _number, const WNVector4<Type>& _vector);

    template <typename Type>
    WNVector4<Type> operator / (const Type& _number, const WNVector4<Type>& _vector);
}

#include "WNMath/inc/Internal/WNVector4.inl"

#endif // __WN_MATH_VECTOR_4_H__