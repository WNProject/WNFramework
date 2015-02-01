    // Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONSTANTS_YMM_H__
#define __WN_MATH_INTERNAL_X86_CONSTANTS_YMM_H__

#define __WN_YMM_PI64_CONST_1(_name, _value) static const WN_ALIGN(16) WN_INT64 __WN##_name##_YMM[2] = {static_cast<WN_INT64>(_value), static_cast<WN_INT64>(_value)}

#define __WN_YMM_GET_PS_CONST(_name) (*reinterpret_cast<const __m256*>(__WN##_name##_YMM))
#define __WN_YMM_GET_PD_CONST(_name) (*reinterpret_cast<const __m256d*>(__WN##_name##_YMM))
#define __WN_YMM_GET_PI8_CONST(_name) (*reinterpret_cast<const __m256i*>(__WN##_name##_YMM))
#define __WN_YMM_GET_PI16_CONST(_name) (*reinterpret_cast<const __m256i*>(__WN##_name##_YMM))
#define __WN_YMM_GET_PI32_CONST(_name) (*reinterpret_cast<const __m256i*>(__WN##_name##_YMM))
#define __WN_YMM_GET_PI64_CONST(_name) (*reinterpret_cast<const __m256i*>(__WN##_name##_YMM))
#define __WN_YMM_GET_SI256_CONST(_name) (*reinterpret_cast<const __m256i*>(__WN##_name##_YMM))

namespace WNMath {
    namespace __WNInternal {
        __WN_YMM_PI64_CONST_1(SignMask1111x64, 0x8000000000000000);
    }
};

#endif // __WN_MATH_INTERNAL_X86_CONSTANTS_YMM_H__