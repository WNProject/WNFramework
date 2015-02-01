// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_BASIC_INL__
#define __WN_MATH_BASIC_INL__

#ifndef __WN_MATH_BASIC_H__
    #error "WNBasic.inl should never be included directly. Please include WNBasic.h instead"
#endif

#include "WNMath/inc/WNConstants.h"

#if defined _WN_MSVC && _WN_MSVC_MAJOR < 16
    #pragma warning (push)
    #pragma warning (disable: 4985)  // Attributes not present on previous declaration
#endif

#include <math.h>

#if defined _WN_MSVC && _WN_MSVC_MAJOR < 16
    #pragma warning (pop)
#endif

namespace WNMath {
    namespace __WNInternal {
        template <typename Type>
        WN_FORCE_INLINE Type __WNAbsGeneric(const Type& _number) {
            return(_number < static_cast<Type>(0) ? -_number : _number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNAbsGeneric(const WN_UINT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNAbsGeneric(const WN_UINT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNAbsGeneric(const WN_UINT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNAbsGeneric(const WN_UINT64& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNAbsGeneric(const WN_INT8& _number) {
            const WN_INT8 negate = static_cast<WN_INT8>(_number < 0);

            return((_number ^ -negate) + negate);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNAbsGeneric(const WN_INT16& _number) {
            const WN_INT16 negate = static_cast<WN_INT16>(_number < 0);

            return((_number ^ -negate) + negate);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNAbsGeneric(const WN_INT32& _number) {
            const WN_INT32 negate = static_cast<WN_INT32>(_number < 0);

            return((_number ^ -negate) + negate);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNAbsGeneric(const WN_INT64& _number) {
            const WN_INT64 negate = static_cast<WN_INT64>(_number < 0);

            return((_number ^ -negate) + negate);
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT32 __WNAbsGeneric(const WN_FLOAT32& _number) {
            return(abs(_number));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT64 __WNAbsGeneric(const WN_FLOAT64& _number) {
            return(abs(_number));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNModGeneric(const Type _dividend, const Type _divisor) {
            return(static_cast<Type>(fmod(static_cast<WN_FLOAT64>(_dividend), static_cast<WN_FLOAT64>(_divisor))));
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNModGeneric(const WN_UINT8 _dividend, const WN_UINT8 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNModGeneric(const WN_UINT16 _dividend, const WN_UINT16 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNModGeneric(const WN_UINT32 _dividend, const WN_UINT32 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNModGeneric(const WN_UINT64 _dividend, const WN_UINT64 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNModGeneric(const WN_INT8 _dividend, const WN_INT8 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNModGeneric(const WN_INT16 _dividend, const WN_INT16 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNModGeneric(const WN_INT32 _dividend, const WN_INT32 _divisor) {
            return(_dividend % _divisor);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNModGeneric(const WN_INT64 _dividend, const WN_INT64 _divisor) {
            return(_dividend % _divisor);
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNRoundGeneric(const Type& _number) {
            return(static_cast<Type>(floor(static_cast<WN_FLOAT64>(_number) + 0.5)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNRoundGeneric(const WN_UINT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNRoundGeneric(const WN_UINT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNRoundGeneric(const WN_UINT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNRoundGeneric(const WN_UINT64& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNRoundGeneric(const WN_INT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNRoundGeneric(const WN_INT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNRoundGeneric(const WN_INT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNRoundGeneric(const WN_INT64& _number) {
            return(_number);
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNCeilGeneric(const Type& _number) {
            return(static_cast<Type>(ceil(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNCeilGeneric(const WN_UINT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNCeilGeneric(const WN_UINT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNCeilGeneric(const WN_UINT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNCeilGeneric(const WN_UINT64& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNCeilGeneric(const WN_INT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNCeilGeneric(const WN_INT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNCeilGeneric(const WN_INT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNCeilGeneric(const WN_INT64& _number) {
            return(_number);
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNFloorGeneric(const Type& _number) {
            return(static_cast<Type>(floor(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNFloorGeneric(const WN_UINT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNFloorGeneric(const WN_UINT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNFloorGeneric(const WN_UINT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNFloorGeneric(const WN_UINT64& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNFloorGeneric(const WN_INT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNFloorGeneric(const WN_INT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNFloorGeneric(const WN_INT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNFloorGeneric(const WN_INT64& _number) {
            return(_number);
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNTruncGeneric(const Type& _number) {
            if (_number > static_cast<Type>(0)) {
                return(static_cast<Type>(floor(static_cast<WN_FLOAT64>(_number))));
            } else if (_number < static_cast<Type>(0)) {
                return(static_cast<Type>(ceil(static_cast<WN_FLOAT64>(_number))));
            }

            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNTruncGeneric(const WN_UINT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNTruncGeneric(const WN_UINT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNTruncGeneric(const WN_UINT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNTruncGeneric(const WN_UINT64& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNTruncGeneric(const WN_INT8& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNTruncGeneric(const WN_INT16& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNTruncGeneric(const WN_INT32& _number) {
            return(_number);
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNTruncGeneric(const WN_INT64& _number) {
            return(_number);
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNSqrtGeneric(const Type& _number) {
            return(static_cast<Type>(sqrt(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT32 __WNSqrtGeneric(const WN_FLOAT32& _number) {
            #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                const WN_UINT32 eqZero = -(_number == 0.0f);
                const WN_FLOAT32 result = WNInvSqrt(_number);
                WN_INT32 i = *reinterpret_cast<const WN_INT32*>(&result);

                i &= ~(eqZero);

                return(_number * (*reinterpret_cast<WN_FLOAT32*>(&i)));
            #else
                return(sqrt(_number));
            #endif
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNInvSqrtGeneric(const Type& _number) {
            return(static_cast<Type>(1.0 / sqrt(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT32 __WNInvSqrtGeneric(const WN_FLOAT32& _number) {
            #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                const WN_UINT32 ltZero = -(_number < 0.0f);
                const WN_UINT32 eqZero = -(_number == 0.0f);
                const WN_FLOAT32 halfNumber = _number * 0.5f;
                WN_INT32 i = *reinterpret_cast<const WN_INT32*>(&_number);

                i = 0x5F375A86 - (i >> 1);

                WN_FLOAT32 r = *reinterpret_cast<WN_FLOAT32*>(&i);

                r = r * (1.5f - r * r * halfNumber);
                i = *reinterpret_cast<WN_INT32*>(&r);
                i = (i & ~ltZero) ^ (ltZero & 0xFFC00000);
                i = (i & 0x80000000) | ((i & ~eqZero) ^ (eqZero & 0x7F800000));

                return(*reinterpret_cast<WN_FLOAT32*>(&i));
            #else
                return(1.0f / sqrt(_number));
            #endif
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNPowGeneric(const Type& _base, const Type& _exponent) {
            return(static_cast<Type>(pow(static_cast<WN_FLOAT64>(_base), static_cast<WN_FLOAT64>(_exponent))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNSinGeneric(const Type& _number) {
            return(static_cast<Type>(sin(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNCosGeneric(const Type& _number) {
            return(static_cast<Type>(cos(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNTanGeneric(const Type& _number) {
            return(static_cast<Type>(tan(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNASinGeneric(const Type& _number) {
            if (_number <= static_cast<Type>(-1)) {
                return(static_cast<Type>(-WN_PI_2));
            }

            if (_number >= static_cast<Type>(1)) {
                return(static_cast<Type>(WN_PI_2));
            }

            return(static_cast<Type>(asin(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT32 __WNASinGeneric(const WN_FLOAT32& _number) {
            if (_number <= -1.0f) {
                return(static_cast<WN_FLOAT32>(-WN_PI_2));
            }

            if (_number >= 1.0f) {
                return(static_cast<WN_FLOAT32>(WN_PI_2));
            }

            return(asin(_number));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT64 __WNASinGeneric(const WN_FLOAT64& _number) {
            if (_number <= -1.0) {
                return(-WN_PI_2);
            }

            if (_number >= 1.0) {
                return(WN_PI_2);
            }

            return(asin(_number));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNACosGeneric(const Type& _number) {
            if (_number <= static_cast<Type>(-1)) {
                return(static_cast<Type>(WN_PI));
            }

            if (_number >= static_cast<Type>(1)) {
                return(static_cast<Type>(0));
            }

            return(static_cast<Type>(acos(static_cast<WN_FLOAT64>(_number))));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT32 __WNACosGeneric(const WN_FLOAT32& _number) {
            if (_number <= -1.0f) {
                return(static_cast<WN_FLOAT32>(WN_PI));
            }

            if (_number >= 1.0f) {
                return(0.0f);
            }

            return(acos(_number));
        }

        template <>
        WN_FORCE_INLINE WN_FLOAT64 __WNACosGeneric(const WN_FLOAT64& _number) {
            if (_number <= -1.0) {
                return(WN_PI);
            }

            if (_number >= 1.0) {
                return(0.0);
            }

            return(acos(_number));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNATanGeneric(const Type& _number) {
            return(static_cast<Type>(atan(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNATanGeneric(const Type& _ycoord, const Type& _xcoord) {
            return(static_cast<Type>(atan2(static_cast<WN_FLOAT64>(_ycoord), static_cast<WN_FLOAT64>(_xcoord))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNExpGeneric(const Type& _number) {
            return(static_cast<Type>(exp(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNExp2Generic(const Type& _number) {
            return(static_cast<Type>(pow(2.0, static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNLnGeneric(const Type& _number) {
            return(static_cast<Type>(log(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNLog2Generic(const Type& _number) {
            return(static_cast<Type>(log(static_cast<WN_FLOAT64>(_number)) / WN_LN2));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNLog10Generic(const Type& _number) {
            return(static_cast<Type>(log10(static_cast<WN_FLOAT64>(_number))));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNMinGeneric(const Type& _first, const Type& _second) {
            return(_first > _second ? _second : _first);
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNMinGeneric(const WN_UINT8& _first, const WN_UINT8& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNMinGeneric(const WN_UINT16& _first, const WN_UINT16& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNMinGeneric(const WN_UINT32& _first, const WN_UINT32& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNMinGeneric(const WN_UINT64& _first, const WN_UINT64& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNMinGeneric(const WN_INT8& _first, const WN_INT8& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNMinGeneric(const WN_INT16& _first, const WN_INT16& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNMinGeneric(const WN_INT32& _first, const WN_INT32& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNMinGeneric(const WN_INT64& _first, const WN_INT64& _second) {
            return(_second ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNMaxGeneric(const Type& _first, const Type& _second) {
            return(_first < _second ? _second : _first);
        }

        template <>
        WN_FORCE_INLINE WN_UINT8 __WNMaxGeneric(const WN_UINT8& _first, const WN_UINT8& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT16 __WNMaxGeneric(const WN_UINT16& _first, const WN_UINT16& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT32 __WNMaxGeneric(const WN_UINT32& _first, const WN_UINT32& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_UINT64 __WNMaxGeneric(const WN_UINT64& _first, const WN_UINT64& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT8 __WNMaxGeneric(const WN_INT8& _first, const WN_INT8& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT16 __WNMaxGeneric(const WN_INT16& _first, const WN_INT16& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT32 __WNMaxGeneric(const WN_INT32& _first, const WN_INT32& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <>
        WN_FORCE_INLINE WN_INT64 __WNMaxGeneric(const WN_INT64& _first, const WN_INT64& _second) {
            return(_first ^ ((_first ^ _second) & -(_first < _second)));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNClampGeneric(const Type& _number, const Type& _min, const Type& _max) {
            return(WNMin(WNMax(_number, _min), _max));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNDegToRadGeneric(const Type& _number) {
            return(static_cast<Type>(static_cast<WN_FLOAT64>(_number) * (WN_PI / 180.0)));
        }

        template <typename Type>
        WN_FORCE_INLINE Type __WNRadToDegGeneric(const Type& _number) {
            return(static_cast<Type>(static_cast<WN_FLOAT64>(_number) * (180.0 / WN_PI)));
        }
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNAbs(const Type& _number) {
        return(__WNInternal::__WNAbsGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNMod(const Type _dividend, const Type _divisor) {
        return(__WNInternal::__WNModGeneric(_dividend, _divisor));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNRound(const Type& _number) {
        return(__WNInternal::__WNRoundGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNCeil(const Type& _number) {
        return(__WNInternal::__WNCeilGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNFloor(const Type& _number) {
        return(__WNInternal::__WNFloorGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNTrunc(const Type& _number) {
        return(__WNInternal::__WNTruncGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNSqrt(const Type& _number) {
        return(__WNInternal::__WNSqrtGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNInvSqrt(const Type& _number) {
        return(__WNInternal::__WNInvSqrtGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNPow(const Type& _base, const Type& _exponent) {
        return(__WNInternal::__WNPowGeneric(_base, _exponent));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNSin(const Type& _number) {
        return(__WNInternal::__WNSinGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNCos(const Type& _number) {
        return(__WNInternal::__WNCosGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNTan(const Type& _number) {
        return(__WNInternal::__WNTanGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNASin(const Type& _number) {
        return(__WNInternal::__WNASinGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNACos(const Type& _number) {
        return(__WNInternal::__WNACosGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNATan(const Type& _number) {
        return(__WNInternal::__WNATanGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNATan(const Type _ycood, const Type& _xcoord) {
        return(__WNInternal::__WNATanGeneric(_ycood, _xcoord));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNExp(const Type& _number) {
        return(__WNInternal::__WNExpGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNExp2(const Type& _number) {
        return(__WNInternal::__WNExp2Generic(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNLn(const Type& _number) {
        return(__WNInternal::__WNLnGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNLog2(const Type& _number) {
        return(__WNInternal::__WNLog2Generic(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNLog10(const Type& _number) {
        return(__WNInternal::__WNLog10Generic(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNMin(const Type& _first, const Type& _second) {
        return(__WNInternal::__WNMinGeneric(_first, _second));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNMax(const Type& _first, const Type& _second) {
        return(__WNInternal::__WNMaxGeneric(_first, _second));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNClamp(const Type& _number, const Type& _min, const Type& _max) {
        return(__WNInternal::__WNClampGeneric(_number, _min, _max));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNDegToRad(const Type& _number) {
        return(__WNInternal::__WNDegToRadGeneric(_number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type WNRadToDeg(const Type& _number) {
        return(__WNInternal::__WNRadToDegGeneric(_number));
    }
}

#endif // __WN_MATH_BASIC_INL__