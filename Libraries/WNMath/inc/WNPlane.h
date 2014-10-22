// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_PLANE_H__
#define __WN_MATH_PLANE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNConfig.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type>
        struct __WNPlaneBase {
            Type mA;
            Type mB;
            Type mC;
            Type mD;
        };
    }

    template <typename Type> class WNVector3;

    template <typename Type>
    class WNPlane : public __WNInternal::__WNPlaneBase<Type> {
    public:
        WNPlane();
        explicit WNPlane(const Type* _numbers);
        explicit WNPlane(const WNVector3<Type>& _normal, Type _distance);
        explicit WNPlane(Type _a, Type _b, Type _c, Type _d);

        operator Type* ();
        operator const Type* () const;

        WNPlane& operator += (const WNPlane& _plane);
        WNPlane& operator -= (const WNPlane& _plane);

        WNPlane operator + () const;
        WNPlane operator - () const;

        WNPlane operator + (const WNPlane& _plane) const;
        WNPlane operator - (const WNPlane& _plane) const;

        WN_BOOL operator == (const WNPlane& _plane) const;
        WN_BOOL operator != (const WNPlane& _plane) const;

        WN_VOID Zero();
        WN_VOID Normalize();
        WN_VOID Translate(const WNVector3<Type>& _translation);

        WNVector3<Type> Normal() const;
        Type Distance() const;

        WN_BOOL IsZero() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(Type _a, Type _b, Type _c, Type _d);

        WN_VOID Define(const WNVector3<Type>& _vector1, const WNVector3<Type>& _vector2, const WNVector3<Type>& _vector3);

        WNPlane GetNormalized() const;
        WNPlane GetTranslated(const WNVector3<Type>& _translation) const;

        Type* RetrieveRaw(Type* _numbers);

        WN_VOID Prefetch() const;

        template <typename NewType>
        WNPlane<NewType> ConvertTo() const;
    };
}

#include "WNMath/inc/Internal/WNPlane.inl"

#endif // __WN_MATH_PLANE_H__