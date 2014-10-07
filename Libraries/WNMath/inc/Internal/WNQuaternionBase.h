////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_QUATERNION_BASE_H__
#define __WN_MATH_INTERNAL_QUATERNION_BASE_H__

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename QuaternionType, typename Type>
        class __WNQuaternionBaseCommon {
        public:
            Type& operator [] (const WN_UINT32 _index);
            const Type& operator [] (const WN_UINT32 _index) const;

            Type& operator () (const WN_UINT32 _index);
            const Type& operator () (const WN_UINT32 _index) const;

            QuaternionType operator + () const;
            QuaternionType operator - () const;

            QuaternionType& operator += (const QuaternionType& _quaternion);
            QuaternionType& operator -= (const QuaternionType& _quaternion);
            QuaternionType& operator *= (const QuaternionType& _quaternion);
            QuaternionType& operator *= (const Type& _number);
            QuaternionType& operator /= (const Type& _number);

            QuaternionType operator + (const QuaternionType& _quaternion) const;
            QuaternionType operator - (const QuaternionType& _quaternion) const;
            QuaternionType operator * (const QuaternionType& _quaternion) const;
            QuaternionType operator * (const Type& _number) const;
            QuaternionType operator / (const Type& _number) const;

            WN_BOOL operator == (const QuaternionType& _quaternion) const;
            WN_BOOL operator != (const QuaternionType& _quaternion) const;

            WN_VOID Zero();
            WN_VOID Identity();

            Type Length() const;
            Type LengthSquared() const;

            Type Dot(const QuaternionType& _quaternion) const;

            WN_BOOL IsZero() const;
            WN_BOOL IsIdentity() const;

            WN_VOID Set(const QuaternionType& _quaternion);
            WN_VOID Set(const Type& _number);
            WN_VOID Set(const Type* _number);
            WN_VOID Set(const Type& _x, const Type& _y, const Type& _z, const Type& _w);

        public:
            static QuaternionType CreateZero();
            static QuaternionType CreateIdentity();

        protected:
            __WNQuaternionBaseCommon();

        protected:
            __WNElements<Type, 4> mElements;
        };

        template <typename QuaternionType, typename Type, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNQuaternionBase : public __WNQuaternionBaseCommon<QuaternionType, Type> {
        protected:
            WN_FORCE_INLINE __WNQuaternionBase() {}
        };

        template <typename QuaternionType, typename Type>
        class __WNQuaternionBase<QuaternionType, Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value> : public __WNQuaternionBaseCommon<QuaternionType, Type> {
        public:
            WN_VOID Normalize();
            WN_VOID Invert();
            WN_VOID Logarithm();
            WN_VOID Exponent();
            WN_VOID Rotate(const QuaternionType& _rotation);

            QuaternionType GetNormalized() const;
            QuaternionType GetInverted() const;
            QuaternionType GetConjugate() const;
            QuaternionType GetLogarithm() const;
            QuaternionType GetExponent() const;
            QuaternionType GetRotated(const QuaternionType& _rotation) const;

        protected:
            WN_FORCE_INLINE __WNQuaternionBase() {}
        };
    }
}

#include "WNMath/inc/Internal/WNQuaternionBase.inl"

#endif // __WN_MATH_INTERNAL_QUATERNION_BASE_H__