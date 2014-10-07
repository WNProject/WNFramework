////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_RAY_3_H__
#define __WN_MATH_RAY_3_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNVector3.h"

namespace WNMath {
    template <typename Type> class WNLine2;
    template <typename Type> class WNLine3;

    template <typename Type>
    class WNRay3 {
    public:
        WNRay3();
        explicit WNRay3(const Type* _numbers);
        explicit WNRay3(const WNVector3<Type>& _location, const WNVector3<Type>& _direction);

        WNRay3 operator + () const;
        WNRay3 operator - () const;

        WN_BOOL operator == (const WNRay3& _ray) const;
        WN_BOOL operator != (const WNRay3& _ray) const;

        WN_VOID Zero();

        WN_BOOL IsZero() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector3<Type>& _location, const WNVector3<Type>& _direction);

        WNLine2<Type> ToLine2() const;
        WNLine3<Type> ToLine3() const;

    public:
        WNVector3<Type> mLocation;
        WNVector3<Type> mDirection;
    };
}

#include "WNMath/inc/Internal/WNRay3.inl"

#endif // __WN_MATH_RAY_3_H__