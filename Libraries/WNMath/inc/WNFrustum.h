// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_FRUSTUM_H__
#define __WN_MATH_FRUSTUM_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNPlane.h"
#include "WNMath/inc/WNMatrix4.h"
#include "WNMath/inc/WNCommon.h"

namespace WNMath {
    template <typename Type> class WNBox;
    template <typename Type> class WNSphere;
    template <typename Type> class WNBounds3;

    template <typename Type>
    class WNFrustum {
        WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value == WN_TRUE, "Frustums of non real number types don't make sense.  Please use one of the available real number types.");

    public:
        explicit WNFrustum(const WNMatrix4<Type>& _transform, Type _near, Type _far, Type _aspect, Type _fov, WNFieldOfView::WNHorizontal _horizontal);
        explicit WNFrustum(const WNMatrix4<Type>& _transform, Type _near, Type _far, Type _aspect, Type _fov, WNFieldOfView::WNVertical _vertical);
        explicit WNFrustum(Type _near, Type _far, Type _fov, Type _aspect, WNFieldOfView::WNHorizontal _horizontal);
        explicit WNFrustum(Type _near, Type _far, Type _fov, Type _aspect, WNFieldOfView::WNVertical _vertical);

        WN_VOID Transform(const WNMatrix4<Type>& _updateTransform);
        WN_VOID SetTransform(const WNMatrix4<Type>& _setTransform);

        WN_BOOL Intersects(const WNBox<Type>& _box) const;
        WN_BOOL Intersects(const WNSphere<Type>& _sphere) const;
        WN_BOOL Intersects(const WNBounds3<Type>& _bounds) const;

    public:
        WNVector3<Type> mPoints[8];
        WNMatrix4<Type> mTransform;
        Type mNearPlane;
        Type mFarPlane;
        Type mHorizontalFOV;
        Type mVerticalFOV;
    };
}

#include "WNMath/inc/Internal/WNFrustum.inl"

#endif // __WN_MATH_FRUSTUM_H__