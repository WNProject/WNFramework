////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__
#define __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__

#ifndef __WN_MATH_MATRIX_4_H__
    #error "Internal/WNMatrix4Base.inl should never be included directly. Please include WNMatrix4.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNQuaternion.h"
#include "WNMath/inc/WNVector3.h"
#include "WNMath/inc/WNVector4.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type>
        WN_FORCE_INLINE WNVector3<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector3<Type>& _vector) const {
            WNVector3<Type> vector = _vector;

            return(this->TransformVectorInPlace(vector));
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector3<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector3<Type>& _vector, Type& _wComponent) const {
            WNVector4<Type> vector = _vector.ToVector4();

            this->TransformVectorInPlace(vector);

            _wComponent = vector.GetW();

            return(vector.ToVector3());
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVector(const WNVector4<Type>& _vector) const {
            WNVector4<Type> vector = _vector;

            return(this->TransformVectorInPlace(vector));
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector3<Type>& __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVectorInPlace(WNVector3<Type>& _vector) const {
            const Type x = _vector.GetX();
            const Type y = _vector.GetY();
            const Type z = _vector.GetZ();

            _vector[0] = x * this->mElements.mValues[0] + y * this->mElements.mValues[4] + z * this->mElements.mValues[8] + this->mElements.mValues[12];
            _vector[1] = x * this->mElements.mValues[1] + y * this->mElements.mValues[5] + z * this->mElements.mValues[9] + this->mElements.mValues[13];
            _vector[2] = x * this->mElements.mValues[2] + y * this->mElements.mValues[6] + z * this->mElements.mValues[10] + this->mElements.mValues[14];

            return(_vector);
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector3<Type>& __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVectorInPlace(WNVector3<Type>& _vector, Type& _wComponent) const {
            WNVector4<Type> vector = _vector.ToVector4();

            this->TransformVectorInPlace(vector);

            _vector = vector.ToVector3();
            _wComponent = vector.GetW();

            return(_vector);
        }

        template <typename Type>
        WN_FORCE_INLINE WNVector4<Type>& __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::TransformVectorInPlace(WNVector4<Type>& _vector) const {
            const Type x = _vector.GetX();
            const Type y = _vector.GetY();
            const Type z = _vector.GetZ();
            const Type w = _vector.GetW();

            _vector[0] = x * this->mElements.mValues[0] + y * this->mElements.mValues[4] + z * this->mElements.mValues[8] + w * this->mElements.mValues[12];
            _vector[1] = x * this->mElements.mValues[1] + y * this->mElements.mValues[5] + z * this->mElements.mValues[9] + w * this->mElements.mValues[13];
            _vector[2] = x * this->mElements.mValues[2] + y * this->mElements.mValues[6] + z * this->mElements.mValues[10] + w * this->mElements.mValues[14];
            _vector[3] = x * this->mElements.mValues[3] + y * this->mElements.mValues[7] + z * this->mElements.mValues[11] + w * this->mElements.mValues[15];

            return(_vector);
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotation(const WNQuaternion<Type>& _rotation) {
            *this *= _rotation.ToMatrix4();
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotationX(const Type& _angle) {
            const Type sinTheta = WNSin(_angle);
            const Type cosTheta = WNCos(_angle);

            // First Column
            this->mElements.mValues[0] = static_cast<Type>(1);
            this->mElements.mValues[1] = static_cast<Type>(0);
            this->mElements.mValues[2] = static_cast<Type>(0);
            this->mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[4] = static_cast<Type>(0);
            this->mElements.mValues[5] = cosTheta;
            this->mElements.mValues[6] = sinTheta;
            this->mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[8] = static_cast<Type>(0);
            this->mElements.mValues[9] = -sinTheta;
            this->mElements.mValues[10] = cosTheta;
            this->mElements.mValues[11] = static_cast<Type>(0);

            // Fourth Column
            this->mElements.mValues[12] = static_cast<Type>(0);
            this->mElements.mValues[13] = static_cast<Type>(0);
            this->mElements.mValues[14] = static_cast<Type>(0);
            this->mElements.mValues[15] = static_cast<Type>(1);
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotationY(const Type& _angle) {
            const Type sinTheta = WNSin(_angle);
            const Type cosTheta = WNCos(_angle);

            // First Column
            this->mElements.mValues[0] = cosTheta;
            this->mElements.mValues[1] = static_cast<Type>(0);
            this->mElements.mValues[2] = -sinTheta;
            this->mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[4] = static_cast<Type>(0);
            this->mElements.mValues[5] = static_cast<Type>(0);
            this->mElements.mValues[6] = static_cast<Type>(1);
            this->mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[8] = sinTheta;
            this->mElements.mValues[9] = static_cast<Type>(0);
            this->mElements.mValues[10] = cosTheta;
            this->mElements.mValues[11] = static_cast<Type>(0);

            // Fourth Column
            this->mElements.mValues[12] = static_cast<Type>(0);
            this->mElements.mValues[13] = static_cast<Type>(0);
            this->mElements.mValues[14] = static_cast<Type>(0);
            this->mElements.mValues[15] = static_cast<Type>(1);
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotationZ(const Type& _angle) {
            const Type sinTheta = WNSin(_angle);
            const Type cosTheta = WNCos(_angle);

            // First Column
            this->mElements.mValues[0] = cosTheta;
            this->mElements.mValues[1] = sinTheta;
            this->mElements.mValues[2] = static_cast<Type>(0);
            this->mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[4] = -sinTheta;
            this->mElements.mValues[5] = cosTheta;
            this->mElements.mValues[6] = static_cast<Type>(1);
            this->mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[8] = static_cast<Type>(0);
            this->mElements.mValues[9] = static_cast<Type>(0);
            this->mElements.mValues[10] = static_cast<Type>(1);
            this->mElements.mValues[11] = static_cast<Type>(0);

            // Fourth Column
            this->mElements.mValues[12] = static_cast<Type>(0);
            this->mElements.mValues[13] = static_cast<Type>(0);
            this->mElements.mValues[14] = static_cast<Type>(0);
            this->mElements.mValues[15] = static_cast<Type>(1);
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotationAxis(const Type& _angle, const WNVector3<Type>& _axis) {
            this->MakeRotationAxis(_angle, _axis.GetX(), _axis.GetY(), _axis.GetZ());
        }

        template <typename Type>
        WN_FORCE_INLINE WN_VOID __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::MakeRotationAxis(const Type& _angle, const Type& _x, const Type& _y, const Type& _z) {
            const Type x2 = _x * _x;
            const Type y2 = _y * _y;
            const Type z2 = _z * _z;
            const Type xy = _x * _y;
            const Type xz = _x * _z;
            const Type yz = _y * _z;
            const Type sinTheta = WNSin(_angle);
            const Type cosTheta = WNCos(_angle);
            const Type oneMinusCosTheta = static_cast<Type>(1) - cosTheta;
            const Type xyOneMinusCosTheta = xy * oneMinusCosTheta;
            const Type xzOneMinusCosTheta = xz * oneMinusCosTheta;
            const Type yzOneMinusCosTheta = yz * oneMinusCosTheta;
            const Type xSinTheta = _x * sinTheta;
            const Type ySinTheta = _y * sinTheta;
            const Type zSinTheta = _z * sinTheta;

            // First Column
            this->mElements.mValues[0] = cosTheta + x2 * oneMinusCosTheta;
            this->mElements.mValues[1] = xyOneMinusCosTheta + zSinTheta;
            this->mElements.mValues[2] = xzOneMinusCosTheta - ySinTheta;
            this->mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            this->mElements.mValues[4] = xyOneMinusCosTheta - zSinTheta;
            this->mElements.mValues[5] = cosTheta + y2 * oneMinusCosTheta;
            this->mElements.mValues[6] = yzOneMinusCosTheta + xSinTheta;
            this->mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            this->mElements.mValues[8] = xzOneMinusCosTheta + ySinTheta;
            this->mElements.mValues[9] = yzOneMinusCosTheta - xSinTheta;
            this->mElements.mValues[10] = cosTheta + z2 * oneMinusCosTheta;
            this->mElements.mValues[11] = static_cast<Type>(0);

            // Fourth Column
            this->mElements.mValues[12] = static_cast<Type>(0);
            this->mElements.mValues[13] = static_cast<Type>(0);
            this->mElements.mValues[14] = static_cast<Type>(0);
            this->mElements.mValues[15] = static_cast<Type>(1);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotation(const WNQuaternion<Type>& _rotation) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotation(_rotation), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotationX(const Type& _angle) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotationX(_angle), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotationY(const Type& _angle) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotationY(_angle), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotationZ(const Type& _angle) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotationZ(_angle), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotationAxis(const Type& _angle, const WNVector3<Type>& _axis) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotationAxis(_angle, _axis), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateRotationAxis(const Type& _angle, const Type& _x, const Type& _y, const Type& _z) {
            WNMatrix4<Type> matrix;

            return(matrix.MakeRotationAxis(_angle, _x, _y, _z), matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreatePerspective(const Type& _fov, const Type& _aspect, const Type& _near, const Type& _far) {
            const Type yScale = static_cast<Type>(1) / WNTan(_fov / static_cast<Type>(2));
            const Type xScale = yScale / _aspect;
            const Type nearFarDiff = _near - _far;
            WNMatrix4<Type> matrix;

            // First Column
            matrix.mElements.mValues[0] = xScale;
            matrix.mElements.mValues[1] = static_cast<Type>(0);
            matrix.mElements.mValues[2] = static_cast<Type>(0);
            matrix.mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            matrix.mElements.mValues[4] = static_cast<Type>(0);
            matrix.mElements.mValues[5] = yScale;
            matrix.mElements.mValues[6] = static_cast<Type>(0);
            matrix.mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            matrix.mElements.mValues[8] = static_cast<Type>(0);
            matrix.mElements.mValues[9] = static_cast<Type>(0);
            matrix.mElements.mValues[10] = _far / nearFarDiff;
            matrix.mElements.mValues[11] = static_cast<Type>(-1);

            // Fourth Column
            matrix.mElements.mValues[12] = static_cast<Type>(0);
            matrix.mElements.mValues[13] = static_cast<Type>(0);
            matrix.mElements.mValues[14] = (_near * _far) / nearFarDiff;
            matrix.mElements.mValues[15] = static_cast<Type>(1);

            return(matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateOrthographic(const Type& _width, const Type& _height, const Type& _near, const Type& _far) {
            const Type farNearDiff = _far - _near;
            WNMatrix4<Type> matrix;

            // First Column
            matrix.mElements.mValues[0] = static_cast<Type>(2) / _width;
            matrix.mElements.mValues[1] = static_cast<Type>(0);
            matrix.mElements.mValues[2] = static_cast<Type>(0);
            matrix.mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            matrix.mElements.mValues[4] = static_cast<Type>(0);
            matrix.mElements.mValues[5] = static_cast<Type>(2) / _height;
            matrix.mElements.mValues[6] = static_cast<Type>(0);
            matrix.mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            matrix.mElements.mValues[8] = static_cast<Type>(0);
            matrix.mElements.mValues[9] = static_cast<Type>(0);
            matrix.mElements.mValues[10] = -static_cast<Type>(2) / farNearDiff;
            matrix.mElements.mValues[11] = static_cast<Type>(0);

            // Fourth Column
            matrix.mElements.mValues[12] = static_cast<Type>(0);
            matrix.mElements.mValues[13] = static_cast<Type>(0);
            matrix.mElements.mValues[14] = -(_far + _near) / farNearDiff;
            matrix.mElements.mValues[15] = static_cast<Type>(1);

            return(matrix);
        }

        template <typename Type>
        WN_FORCE_INLINE WNMatrix4<Type> __WNMatrix4Base<Type, typename WNCore::WNEnableWhen<WNCore::WNIsReal<Type>::Value>::Value>::CreateView(const WNVector3<Type>& _position, const WNVector3<Type>& _target, const WNVector3<Type>& _up) {
            const WNVector3<Type> forward = (_position - _target).GetNormalized();
            const WNVector3<Type> right = (_up.Cross(forward)).GetNormalized();
            const WNVector3<Type> up = forward.Cross(right);
            WNMatrix4<Type> matrix;

            // First Column
            matrix.mElements.mValues[0] = right.GetX();
            matrix.mElements.mValues[1] = right.GetY();
            matrix.mElements.mValues[2] = right.GetZ();
            matrix.mElements.mValues[3] = static_cast<Type>(0);

            // Second Column
            matrix.mElements.mValues[4] = up.GetX();
            matrix.mElements.mValues[5] = up.GetY();
            matrix.mElements.mValues[6] = up.GetZ();
            matrix.mElements.mValues[7] = static_cast<Type>(0);

            // Third Column
            matrix.mElements.mValues[8] = forward.GetX();
            matrix.mElements.mValues[9] = forward.GetY();
            matrix.mElements.mValues[10] = forward.GetZ();
            matrix.mElements.mValues[11] = static_cast<Type>(0);

            // Forth Column
            matrix.mElements.mValues[12] = -right.Dot(_position);
            matrix.mElements.mValues[13] = -up.Dot(_position);
            matrix.mElements.mValues[14] = -forward.Dot(_position);
            matrix.mElements.mValues[15] = static_cast<Type>(1);

            return(matrix);
        }
    }
}

#endif // __WN_MATH_INTERNAL_MATRIX_4_BASE_INL__