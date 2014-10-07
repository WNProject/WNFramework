////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_BOUNDS_3_H__
#define __WN_MATH_BOUNDS_3_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNBox;
    template <typename Type> class WNSphere;
    template <typename Type> class WNVector3;

    template <typename Type>
    class WNBounds3 {
    public:
        WNBounds3();
        explicit WNBounds3(const Type* _numbers);
        explicit WNBounds3(const WNVector3<Type>& _vector);
        explicit WNBounds3(const WNVector3<Type>& _min, const WNVector3<Type>& _max);

        WN_BOOL operator == (const WNBounds3& _bounds) const;
        WN_BOOL operator != (const WNBounds3& _bounds) const;

        WN_VOID Zero();
        WN_VOID Expand(Type _amount);
        WN_BOOL Expand(const WNVector3<Type>& _vector);
        WN_BOOL Expand(const WNBox<Type>& _box);
        WN_BOOL Expand(const WNSphere<Type>& _sphere);
        WN_BOOL Expand(const WNBounds3& _bounds);
        WN_VOID Translate(const WNVector3<Type>& _vector);

        Type Length() const;
        Type Width() const;
        Type Height() const;
        Type Radius() const;
        Type SurfaceArea() const;
        Type Volume() const;

        WN_BOOL IsZero() const;
        WN_BOOL IsInsideOut() const;

        WNBounds3 GetExpanded(Type _amount) const;
        WNBounds3 GetExpanded(const WNVector3<Type>& _vector) const;
        WNBounds3 GetExpanded(const WNBox<Type>& _box) const;
        WNBounds3 GetExpanded(const WNSphere<Type>& _sphere) const;
        WNBounds3 GetExpanded(const WNBounds3& _bounds) const;
        WNBounds3 GetTranslated(const WNVector3<Type>& _vector) const;

        WN_VOID GetPoints(WNVector3<Type>* _vectors) const;

        WNBox<Type> ToBox() const;

    public:
        WNVector3<Type> mMinimum;
        WNVector3<Type> mMaximum;
    };
}

#include "WNMath/inc/Internal/WNBounds3.inl"

#endif // __WN_MATH_BOUNDS_3_H__