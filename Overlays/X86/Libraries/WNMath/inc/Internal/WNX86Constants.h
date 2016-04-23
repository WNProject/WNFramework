// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__
#define __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__

#define __WN_MM_PS_CONST_1(_name, _value)                                      \
  static const float __WN##_name##_MM[2] = {_value, _value}
#define __WN_MM_PS_CONST_2(_name, _value1, _value2)                            \
  static const float __WN##_name##_MM[2] = {_value1, _value2}

#define __WN_MM_PI32_CONST_1(_name, _value)                                    \
  static const int32_t __WN##_name##_MM[2] = {                                 \
      static_cast<int32_t>(_value), static_cast<int32_t>(_value)}
#define __WN_MM_PI32_CONST_2(_name, _value1, _value2)                          \
  static const int32_t __WN##_name##_MM[2] = {                                 \
      static_cast<int32_t>(_value1), static_cast<int32_t>(_value2)}

#define __WN_MM_GET_PS_CONST(_name)                                            \
  (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI8_CONST(_name)                                           \
  (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI16_CONST(_name)                                          \
  (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI32_CONST(_name)                                          \
  (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_SI64_CONST(_name)                                          \
  (*reinterpret_cast<const __m64*>(__WN##_name##_MM))

namespace wn {
namespace internal {
__WN_MM_PI32_CONST_1(SignMask11x32, 0x80000000);
__WN_MM_PI32_CONST_2(ElementMask10x32, 0xFFFFFFFF, 0x00000000);

__WN_MM_PS_CONST_1(One11x32, 1.0f);
}
};

#define __WN_XMM_PS_CONST_1(_name, _value)                                     \
  static const WN_ALIGN(16) float __WN##_name##_XMM[4] = {                     \
      _value, _value, _value, _value}
#define __WN_XMM_PS_CONST_4(_name, _value1, _value2, _value3, _value4)         \
  static const WN_ALIGN(16) float __WN##_name##_XMM[4] = {                     \
      _value1, _value2, _value3, _value4}
#define __WN_XMM_PD_CONST_1(_name, _value)                                     \
  static const WN_ALIGN(16) double __WN##_name##_XMM[2] = {_value, _value}
#define __WN_XMM_PD_CONST_2(_name, _value1, _value2)                           \
  static const WN_ALIGN(16) double __WN##_name##_XMM[2] = {_value1, _value2}

#define __WN_XMM_PI32_CONST_1(_name, _value)                                   \
  static const WN_ALIGN(16) int32_t __WN##_name##_XMM[4] = {                   \
      static_cast<int32_t>(_value), static_cast<int32_t>(_value),              \
      static_cast<int32_t>(_value), static_cast<int32_t>(_value)}
#define __WN_XMM_PI32_CONST_4(_name, _value1, _value2, _value3, _value4)       \
  static const WN_ALIGN(16) int32_t __WN##_name##_XMM[4] = {                   \
      static_cast<int32_t>(_value1), static_cast<int32_t>(_value2),            \
      static_cast<int32_t>(_value3), static_cast<int32_t>(_value4)}
#define __WN_XMM_PI64_CONST_1(_name, _value)                                   \
  static const WN_ALIGN(16) int64_t __WN##_name##_XMM[2] = {                   \
      static_cast<int64_t>(_value), static_cast<int64_t>(_value)}
#define __WN_XMM_PI64_CONST_2(_name, _value1, _value2)                         \
  static const WN_ALIGN(16) int64_t __WN##_name##_XMM[2] = {                   \
      static_cast<int64_t>(_value1), static_cast<int64_t>(_value2)}

#define __WN_XMM_GET_PS_CONST(_name)                                           \
  (*reinterpret_cast<const __m128*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PD_CONST(_name)                                           \
  (*reinterpret_cast<const __m128d*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI8_CONST(_name)                                          \
  (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI16_CONST(_name)                                         \
  (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI32_CONST(_name)                                         \
  (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI64_CONST(_name)                                         \
  (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_SI128_CONST(_name)                                        \
  (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))

namespace wn {
namespace internal {
__WN_XMM_PI32_CONST_1(SignMask1111x32, 0x80000000);
__WN_XMM_PI32_CONST_4(
    ElementMask1000x32, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000);
__WN_XMM_PI32_CONST_4(
    ElementMask1100x32, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000);
__WN_XMM_PI32_CONST_4(
    ElementMask1110x32, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000);

__WN_XMM_PI64_CONST_1(SignMask11x64, 0x8000000000000000);
__WN_XMM_PI64_CONST_2(ElementMask10x64, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000);
}
};

#endif  // __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__