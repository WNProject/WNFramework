// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_PLANE_INL__
#define __WN_MATH_PLANE_INL__

#ifndef __WN_MATH_PLANE_H__
    #error "WNPlane.inl should never be included directly. Please include WNPlane.h instead"
#endif

#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type>
    WNPlane<Type>::WNPlane() {
        Zero();
    }

    template <typename Type>
    WNPlane<Type>::WNPlane(const Type* _numbers) {
        Set(_numbers);
    }

    template <typename Type>
    WNPlane<Type>::WNPlane(const WNVector3<Type>& _normal, Type _distance) {
        Set(_normal, _distance);
    }

    template <typename Type>
    WNPlane<Type>::WNPlane(Type _a, Type _b, Type _c, Type _d) {
        Set(_a, _b, _c, _d);
    }

    template <typename Type>
    WNPlane<Type>::operator Type* () {
        return(&this->mA);
    }

    template <typename Type>
    WNPlane<Type>::operator const Type* () const {
        return(&this->mA);
    }

    template <typename Type>
    WNPlane<Type>& WNPlane<Type>::operator += (const WNPlane<Type>& _plane) {
        this->mA += _plane.mA;
        this->mB += _plane.mB;
        this->mC += _plane.mC;
        this->mD += _plane.mD;

        return(*this);
    }

    template <typename Type>
    WNPlane<Type>& WNPlane<Type>::operator -= (const WNPlane<Type>& _plane) {
        this->mA -= _plane.mA;
        this->mB -= _plane.mB;
        this->mC -= _plane.mC;
        this->mD -= _plane.mD;

        return(*this);
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::operator + () const {
        return(*this);
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::operator - () const {
        return(WNPlane<Type>(-this->mA, -this->mB, -this->mC, -this->mD));
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::operator + (const WNPlane<Type>& _plane) const {
        WNPlane<Type> plane = *this;

        plane += _plane;

        return(plane);
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::operator - (const WNPlane<Type>& _plane) const {
        WNPlane<Type> plane = *this;

        plane -= _plane;

        return(plane);
    }

    template <typename Type>
    WN_BOOL WNPlane<Type>::operator == (const WNPlane<Type>& _plane) const {
        return(this->mA == _plane.mA && this->mB == _plane.mB && this->mC == _plane.mC && this->mD == _plane.mD);
    }

    template <typename Type>
    WN_BOOL WNPlane<Type>::operator != (const WNPlane<Type>& _plane) const {
        return(this->mA != _plane.mA || this->mB != _plane.mB || this->mC != _plane.mC || this->mD != _plane.mD);
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Zero() {
        this->mA = static_cast<Type>(0);
        this->mB = static_cast<Type>(0);
        this->mC = static_cast<Type>(0);
        this->mD = static_cast<Type>(0);
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Set(const Type* _numbers) {
        this->mA = _numbers[0];
        this->mB = _numbers[1];
        this->mC = _numbers[2];
        this->mD = _numbers[3];
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Set(Type _a, Type _b, Type _c, Type _d) {
        this->mA = _a;
        this->mB = _b;
        this->mC = _c;
        this->mD = _d;
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Define(const WNVector3<Type>& _vector1, const WNVector3<Type>& _vector2, const WNVector3<Type>& _vector3) {
        const WNVector3<Type> normal = _vector1.Cross(_vector2);

        this->mA = normal.mX;
        this->mB = normal.mY;
        this->mC = normal.mZ;

        this->mD = -(Normal().Dot(_vector3));
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Normalize() {
        const WNVector3<Type> vector = Normal().GetNormalized();

        this->mA = vector.mX;
        this->mB = vector.mY;
        this->mC = vector.mZ;
    }

    template <typename Type>
    WN_VOID WNPlane<Type>::Translate(const WNVector3<Type>& _translation) {
        this->mD -= _translation.Dot(Normal());
    }

    template <typename Type>
    WNVector3<Type> WNPlane<Type>::Normal() const {
        return(WNVector3<Type>(this->mA, this->mB, this->mC));
    }

    template <typename Type>
    Type WNPlane<Type>::Distance() const {
        return(-this->mD);
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::GetNormalized() const {
        WNPlane<Type> plane = *this;

        return(plane.Normalize(), plane);
    }

    template <typename Type>
    WNPlane<Type> WNPlane<Type>::GetTranslated(const WNVector3<Type>& _translation) const {
        WNPlane<Type> plane = *this;

        return(plane.Translate(_translation), plane);
    }

    #ifndef __WN_MATH_HAS_PLANE_CUSTOM_PREFETCH
        template <typename Type>
        WN_VOID WNPlane<Type>::Prefetch() const {
            // Default
        }
    #endif

    template <typename Type>
    template <typename NewType>
    WNPlane<NewType> WNPlane<Type>::ConvertTo() const {
        return(WNPlane<NewType>(static_cast<NewType>(this->mA), static_cast<NewType>(this->mB),
                                static_cast<NewType>(this->mC), static_cast<NewType>(this->mD)));
    }
}

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        //#include "WNMath/inc/Internal/x86/WNPlane.inl"
    #endif
#endif

#endif // __WN_MATH_PLANE_INL__
