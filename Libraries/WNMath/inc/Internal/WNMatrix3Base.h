////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_3_BASE_H__
#define __WN_MATH_INTERNAL_MATRIX_3_BASE_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace WNMath {
    template <typename Type>
    class WNVector2;

    template <typename Type>
    class WNVector3;

    template <typename Type>
    class WNMatrix3;

    namespace __WNInternal {
        template <typename Type, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNMatrix3Base : public __WNMatrixBase<WNMatrix3<Type>, Type, 3> {
        protected:
            WN_FORCE_INLINE __WNMatrix3Base() {}
        };

        template <typename Type>
        class __WNMatrix3Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value> : public __WNMatrixBase<WNMatrix3<Type>, Type, 3> {
        public:
            WNVector2<Type> TransformVector(const WNVector2<Type>& _vector) const;
            WNVector2<Type> TransformVector(const WNVector2<Type>& _vector, Type& _zComponent) const;
            WNVector3<Type> TransformVector(const WNVector3<Type>& _vector) const;
            WN_FORCE_INLINE WNVector2<Type>& TransformVectorInPlace(WNVector2<Type>& _vector) const {}
            WN_FORCE_INLINE WNVector2<Type>& TransformVectorInPlace(WNVector2<Type>& _vector, Type& _zComponent) const {}
            WN_FORCE_INLINE WNVector3<Type>& TransformVectorInPlace(WNVector3<Type>& _vector) const {}

            WN_VOID MakeRotation(const Type& _angle);
            WN_VOID MakeOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far);

        public:
            static WNMatrix3<Type> CreateRotation(const Type& _angle);
            static WNMatrix3<Type> CreateOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far);

        protected:
            WN_FORCE_INLINE __WNMatrix3Base() {}
        };
    }
}

#include "WNMath/inc/Internal/WNMatrix3Base.inl"

#endif // __WN_MATH_INTERNAL_MATRIX_3_BASE_H__