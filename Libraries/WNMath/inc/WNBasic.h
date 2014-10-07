////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_BASIC_H__
#define __WN_MATH_BASIC_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNConfig.h"

namespace WNMath {
    template <typename Type>
    WN_FORCE_INLINE Type WNAbs(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNMod(const Type _dividend, const Type _divisor);

    template <typename Type>
    WN_FORCE_INLINE Type WNRound(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNCeil(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNFloor(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNTrunc(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNSqrt(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNInvSqrt(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNPow(const Type& _base, const Type& _exponent);

    template <typename Type>
    WN_FORCE_INLINE Type WNSin(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNCos(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNTan(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNASin(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNACos(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNATan(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNATan(const Type& _ycoord, const Type& _xcoord);

    template <typename Type>
    WN_FORCE_INLINE Type WNExp(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNExp2(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNLn(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNLog2(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNLog10(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNMin(const Type& _first, const Type& _second);

    template <typename Type>
    WN_FORCE_INLINE Type WNMax(const Type& _first, const Type& _second);

    template <typename Type>
    WN_FORCE_INLINE Type WNClamp(const Type& _number, const Type& _min, const Type& _max);

    template <typename Type>
    WN_FORCE_INLINE Type WNDegToRad(const Type& _number);

    template <typename Type>
    WN_FORCE_INLINE Type WNRadToDeg(const Type& _number);
}

#include "WNMath/inc/Internal/WNBasic.inl"

#endif // __WN_MATH_BASIC_H__