////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_LINE_2_H__
#define __WN_MATH_LINE_2_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMath {
    template <typename Type> class WNVector2;

    template <typename Type>
    class WNLine2 {
    public:
        WNLine2();
        WNLine2(const WNLine2& _line);
        explicit WNLine2(const Type* _numbers);
        explicit WNLine2(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2);

        WNLine2 operator + () const;
        WNLine2 operator - () const;

        WN_BOOL operator == (const WNLine2& _line) const;
        WN_BOOL operator != (const WNLine2& _line) const;

        WN_VOID Zero();

        WNVector2<Type> Direction() const;

        WN_VOID Set(const WNLine2& _line);
        WN_VOID Set(const Type* _numbers);
        WN_VOID Set(const WNVector2<Type>& _point1, const WNVector2<Type>& _point2);

    public:
        WNVector2<Type> mPoint1;
        WNVector2<Type> mPoint2;
    };
}

#include "WNMath/inc/Internal/WNLine2.inl"

#endif // __WN_MATH_LINE_2_H__