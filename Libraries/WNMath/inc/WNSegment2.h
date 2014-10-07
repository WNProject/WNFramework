////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_SEGMENT_2_H__
#define __WN_MATH_SEGMENT_2_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNLine2;
    template <typename Type> class WNLine3;
    template <typename Type> class WNRay2;
    template <typename Type> class WNRay3;
    template <typename Type> class WNVector2;

    template <typename Type>
    class WNSegment2 {
    public:
        WNSegment2();
        explicit WNSegment2(const Type* _numbers);
        explicit WNSegment2(const WNVector2<Type>& _start, const WNVector2<Type>& _end);

        WNSegment2 operator + () const;
        WNSegment2 operator - () const;

        WN_BOOL operator == (const WNSegment2& _segment) const;
        WN_BOOL operator != (const WNSegment2& _segment) const;

        WN_VOID Zero();

        Type Length() const;
        Type LengthSquared() const;
        WNVector2<Type> Direction() const;

        WN_BOOL IsZero() const;

        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector2<Type>& _start, const WNVector2<Type>& _end);

        WNLine2<Type> ToLine2() const;
        WNLine3<Type> ToLine3() const;
        WNRay2<Type> ToRay2() const;
        WNRay3<Type> ToRay3() const;

    public:
        WNVector2<Type> mStart;
        WNVector2<Type> mEnd;
    };
}

#include "WNMath/inc/Internal/WNSegment2.inl"

#endif // __WN_MATH_SEGMENT_2_H__