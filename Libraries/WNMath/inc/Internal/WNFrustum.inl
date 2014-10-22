// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_FRUSTUM_INL__
#define __WN_MATH_FRUSTUM_INL__

#ifndef __WN_MATH_FRUSTUM_H__
    #error "WNFrustum.inl should never be included directly. Please include WNFrustum.h instead"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNBox.h"
#include "WNMath/inc/WNBounds3.h"
#include "WNMath/inc/WNSphere.h"

namespace WNMath {
    template <typename Type>
    WN_VOID CalculatePoints(Type _near, Type _far, Type _hFov, Type _vFov, WNVector4<Type>* _vectors) {
        const Type hTan = WNTan(_hFov / static_cast<Type>(2));
        const Type vTan = WNTan(_vFov / static_cast<Type>(2));
        const Type hTanNear = hTan * _near;
        const Type hTanFar = hTan * _far;
        const Type vTanNear = vTan * _near;
        const Type vTanFar = vTan * _far;

        _vectors[0].mX = _vectors[3].mX = -hTanNear;
        _vectors[1].mX = _vectors[2].mX = hTanNear;
        _vectors[4].mX = _vectors[7].mX = -hTanFar;
        _vectors[5].mX = _vectors[6].mX = hTanFar;

        _vectors[0].mY = _vectors[1].mY = vTanNear;
        _vectors[3].mY = _vectors[2].mY = -vTanNear;
        _vectors[4].mY = _vectors[5].mY = vTanFar;
        _vectors[7].mY = _vectors[6].mY = -vTanFar;

        _vectors[0].mZ = _vectors[1].mZ = _vectors[2].mZ = _vectors[3].mZ = _near;
        _vectors[0].mZ = _vectors[1].mZ = _vectors[2].mZ = _vectors[3].mZ = _far;
    }

    template <typename Type>
    WN_VOID TransformPoints(const WNMatrix4<Type>& _transform, WNVector4<Type>* _vectors) {
        _vectors[0] = _transform.Transform(_vectors[0]);
        _vectors[1] = _transform.Transform(_vectors[1]);
        _vectors[2] = _transform.Transform(_vectors[2]);
        _vectors[3] = _transform.Transform(_vectors[3]);
        _vectors[4] = _transform.Transform(_vectors[4]);
        _vectors[5] = _transform.Transform(_vectors[5]);
        _vectors[6] = _transform.Transform(_vectors[6]);
        _vectors[7] = _transform.Transform(_vectors[7]);
    }

    template <typename Type>
    WN_BOOL OverlapOnNormal(const WNVector4<Type>& _normal, const WNVector4<Type>* _pointsFrustum, const WNVector4<Type>* _vectors, WN_UINT32 _numPoints) {
        WN_DEBUG_ASSERT(_numPoints > 0);

        Type frustProj[2];
        Type pointProj[2];

        frustProj[0] = frustProj[1] = WNVector4<Type>::Dot(_pointsFrustum[0].ToVector3(), _normal);

        for (WN_UINT8 i = 1; i < 8; ++i) {
            const Type dotProd = WNVector4<Type>::Dot(_pointsFrustum[i].ToVector3(), _normal);

            frustProj[0] = (dotProd < frustProj[0]) ? dotProd : frustProj[0];
            frustProj[1] = (dotProd > frustProj[1]) ? dotProd : frustProj[1];
        }

        pointProj[0] = pointProj[1] = WNVector4<Type>::Dot(_vectors[0].ToVector3(), _normal);

        for (WN_UINT32 i = 1; i < _numPoints; ++i) {
            const Type dotProd = WNVector4<Type>::Dot(_vectors[i].ToVector3(), _normal);

            pointProj[0] = (dotProd < pointProj[0]) ? dotProd : pointProj[0];
            pointProj[1] = (dotProd > pointProj[1]) ? dotProd : pointProj[1];
        }

        return((frustProj[0] < pointProj[0]) ? (pointProj[0] < frustProj[1]):(frustProj[0] < pointProj[1]));
    }

    template <typename Type>
    WNFrustum<Type>::WNFrustum(const WNMatrix4<Type>& _transform, Type _near, Type _far, Type _aspect, Type _fov, WNFieldOfView::WNHorizontal _horizontal) :
        mNearPlane(_near),
        mFarPlane(_far),
        mHorizontalFOV(_fov),
        mVerticalFOV(_fov / _aspect) {
        mTransform = _transform;

        CalculatePoints(mNearPlane, mFarPlane, mHorizontalFOV, mVerticalFOV, mPoints);
        TransformPoints(mTransform, mPoints);
    }

    template <typename Type>
    WNFrustum<Type>::WNFrustum(const WNMatrix4<Type>& _transform, Type _near, Type _far, Type _aspect, Type _fov, WNFieldOfView::WNVertical _vertical) :
        mNearPlane(_near),
        mFarPlane(_far),
        mHorizontalFOV(_fov * _aspect),
        mVerticalFOV(_fov) {
        mTransform = _transform;

        CalculatePoints(mNearPlane, mFarPlane, mHorizontalFOV, mVerticalFOV, mPoints);
        TransformPoints(mTransform, mPoints);
    }

    template <typename Type>
    WNFrustum<Type>::WNFrustum(Type _near, Type _far, Type _fov, Type _aspect, WNFieldOfView::WNHorizontal _horizontal) :
        mNearPlane(_near),
        mFarPlane(_far),
        mHorizontalFOV(_fov),
        mVerticalFOV(_fov / _aspect) {
        CalculatePoints(mNearPlane, mFarPlane, mHorizontalFOV, mVerticalFOV, mPoints);
    }

    template <typename Type>
    WNFrustum<Type>::WNFrustum(Type _near, Type _far, Type _fov, Type _aspect, WNFieldOfView::WNVertical _vertical) :
        mNearPlane(_near),
        mFarPlane(_far),
        mHorizontalFOV(_fov * _aspect),
        mVerticalFOV(_fov) {
        CalculatePoints(mNearPlane, mFarPlane, mHorizontalFOV, mVerticalFOV, mPoints);
    }

    template <typename Type>
    WN_VOID WNFrustum<Type>::Transform(const WNMatrix4<Type>& _update) {
        mTransform *= _update;

        TransformPoints(mTransform, mPoints);
    }

    template <typename Type>
    WN_VOID WNFrustum<Type>::SetTransform(const WNMatrix4<Type>& _newTransform) {
        mTransform = _newTransform;

        CalculatePoints(mNearPlane, mFarPlane, mHorizontalFOV, mVerticalFOV, mPoints);
        TransformPoints(mTransform, mPoints);
    }

    template <typename Type>
    WN_BOOL WNFrustum<Type>::Intersects(const WNBox<Type>& _box) const {
        static const WN_UINT8 checks[3][3] =   {{0, 1, 2},
                                                {4, 5, 1},
                                                {3, 7, 4}};
        static const WN_UINT8 frustumChecks[5][3] = {{1, 0, 3},
                                                     {4, 0, 1},
                                                     {5, 1, 2},
                                                     {6, 2, 3},
                                                     {7, 3, 0}};

        WNVector4<Type> boxPoints[8];

        _box.GetPoints(boxPoints);

        for (WN_UINT8 i = 0; i < 3; ++i) {
            //FIRST try to find a separating plane against the edges of the box (there are fewer)
            const WNVector4<Type> edge1 = (mPoints[frustumChecks[i][0]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> edge2 = (mPoints[frustumChecks[i][2]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> sideNormal = WNVector4<Type>::Cross(edge1, edge2);

            if (!OverlapOnNormal(sideNormal, mPoints, boxPoints, 8)) {
                return(WN_FALSE);
            }
        }

        for (WN_UINT8 i = 0; i < 5; ++i) {
            //NOW we try and find a separating plane from the frustum
            const WNVector4<Type> edge1 = (boxPoints[checks[i][0]] - boxPoints[checks[i][1]]).GetNormalized();
            const WNVector4<Type> edge2 = (boxPoints[checks[i][2]] - boxPoints[checks[i][1]]).GetNormalized();
            const WNVector4<Type> sideNormal = WNVector4<Type>::Cross(edge1, edge2);

            if (!OverlapOnNormal(sideNormal, mPoints, boxPoints, 8)) {
                return(WN_FALSE);
            }
        }

        return(WN_TRUE);
    }

    template <typename Type>
    WN_BOOL WNFrustum<Type>::Intersects(const WNSphere<Type>& _sphere) const {
        static const WN_UINT8 frustumChecks[6][3] = {{1, 0, 3},
                                                     {4, 0, 1},
                                                     {5, 1, 2},
                                                     {6, 2, 3},
                                                     {7, 3, 0},
                                                     {5, 4, 7}};

        for (WN_UINT8 i = 0; i < 6; ++i) {
            // Lets see if the sphere falls INSIDE (it is outside, if it is entirely on the OUTSIDE of one half-space
            const WNVector4<Type> edge1 = (mPoints[frustumChecks[i][0]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> edge2 = (mPoints[frustumChecks[i][2]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> sideNormal = WNVector4<Type>::Cross(edge1, edge2);
            const Type sideProj = WNVector4<Type>::Dot(mPoints[frustumChecks[i][0]].ToVector3(), sideNormal);
            const Type sphereProj = WNVector4<Type>::Dot(_sphere.mLocation.ToVector3(), sideNormal);

            if (sphereProj + _sphere.mRadius < sideProj) {
                return(WN_FALSE);
            }
        }

        return(WN_TRUE);
    }

    template <typename Type>
    WN_BOOL WNFrustum<Type>::Intersects(const WNBounds3<Type>& _bounds) const {
        static const WN_UINT8 checks[3][3] =   {{0, 1, 2},
                                                {4, 5, 1},
                                                {3, 7, 4}};
        static const WN_UINT8 frustumChecks[5][3] = {{1, 0, 3},
                                                     {4, 0, 1},
                                                     {5, 1, 2},
                                                     {6, 2, 3},
                                                     {7, 3, 0}};

        WNVector4<Type> boxPoints[8];

        _bounds.GetPoints(boxPoints);

        for (WN_UINT8 i = 0; i < 3; ++i) {
            // First try to find a separating plane against the edges of the box (there are fewer)
            const WNVector4<Type> edge1 = (mPoints[frustumChecks[i][0]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> edge2 = (mPoints[frustumChecks[i][2]] - mPoints[frustumChecks[i][1]]).GetNormalized();
            const WNVector4<Type> sideNormal = WNVector4<Type>::Cross(edge1, edge2);

            if (!OverlapOnNormal(sideNormal, mPoints, boxPoints, 8)) {
                return(WN_FALSE);
            }
        }

        for (WN_UINT8 i = 0; i < 5; ++i) {
            // Now try and find a separating plane from the frustum
            const WNVector4<Type> edge1 = (boxPoints[checks[i][0]] - boxPoints[checks[i][1]]).GetNormalized();
            const WNVector4<Type> edge2 = (boxPoints[checks[i][2]] - boxPoints[checks[i][1]]).GetNormalized();
            const WNVector4<Type> sideNormal = WNVector4<Type>::Cross(edge1, edge2);

            if (!OverlapOnNormal(sideNormal, mPoints, boxPoints, 8)) {
                return(WN_FALSE);
            }
        }

        return(WN_TRUE);
    }
}

#endif // __WN_MATH_FRUSTUM_INL__