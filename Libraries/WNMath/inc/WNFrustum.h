// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_FRUSTUM_H__
#define __WN_MATH_FRUSTUM_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNPlane.h"
#include "WNMath/inc/WNMatrix4.h"
#include "WNMath/inc/WNCommon.h"

namespace wn {
    template <typename type> class WNBox;
    template <typename type> class sphere;
    template <typename type> class WNBounds3;

    template <typename type>
    class WNFrustum {
        static_assert(wn::is_real<type>::value == true, "Frustums of non real number types don't make sense.  Please use one of the available real number types.");

    public:
        explicit WNFrustum(const WNMatrix4<type>& _transform, type _near, type _far, type _aspect, type _fov, WNFieldOfView::WNHorizontal _horizontal);
        explicit WNFrustum(const WNMatrix4<type>& _transform, type _near, type _far, type _aspect, type _fov, WNFieldOfView::WNVertical _vertical);
        explicit WNFrustum(type _near, type _far, type _fov, type _aspect, WNFieldOfView::WNHorizontal _horizontal);
        explicit WNFrustum(type _near, type _far, type _fov, type _aspect, WNFieldOfView::WNVertical _vertical);

        void Transform(const WNMatrix4<type>& _updateTransform);
        void SetTransform(const WNMatrix4<type>& _setTransform);

        bool Intersects(const WNBox<type>& _box) const;
        bool Intersects(const sphere<type>& _sphere) const;
        bool Intersects(const WNBounds3<type>& _bounds) const;

    public:
        vector3<type> mPoints[8];
        WNMatrix4<type> mTransform;
        type mNearPlane;
        type mFarPlane;
        type mHorizontalFOV;
        type mVerticalFOV;
    };
}

#include "WNMath/inc/Internal/WNFrustum.inl"

#endif // __WN_MATH_FRUSTUM_H__