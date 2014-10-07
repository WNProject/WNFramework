////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_SEGMENT_3_H__
#define __WN_MATH_SEGMENT_3_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNLine2;
    template <typename Type> class WNLine3;
    template <typename Type> class WNRay2;
    template <typename Type> class WNRay3;
    template <typename Type> class WNVector3;

    template <typename Type>
    class WNSegment3 {
    public:
        WNSegment3();
        explicit WNSegment3(const Type* _numbers);
        explicit WNSegment3(const WNVector3<Type>& _start, const WNVector3<Type>& _end);

        WNSegment3 operator + () const;
        WNSegment3 operator - () const;

        WN_BOOL operator == (const WNSegment3& _segment) const;
        WN_BOOL operator != (const WNSegment3& _segment) const;

        WN_VOID Zero();

        Type Length() const;
        Type LengthSquared() const;
        WNVector3<Type> Direction() const;

        WN_BOOL IsZero() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector3<Type>& _start, const WNVector3<Type>& _end);

        WNLine2<Type> ToLine2() const;
        WNLine3<Type> ToLine3() const;
        WNRay2<Type> ToRay2() const;
        WNRay3<Type> ToRay3() const;

    public:
        WNVector3<Type> mStart;
        WNVector3<Type> mEnd;
    };
}

#include "WNMath/inc/Internal/WNSegment3.inl"

#endif // __WN_MATH_SEGMENT_3_H__