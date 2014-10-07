    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONSTANTS_XMM_H__
#define __WN_MATH_INTERNAL_X86_CONSTANTS_XMM_H__

#define __WN_XMM_PS_CONST_1(_name, _value) static const WN_ALIGN(16) WN_FLOAT32 __WN##_name##_XMM[4] = {_value, _value, _value, _value}
#define __WN_XMM_PS_CONST_4(_name, _value1, _value2, _value3, _value4) static const WN_ALIGN(16) WN_FLOAT32 __WN##_name##_XMM[4] = {_value1, _value2, _value3, _value4}
#define __WN_XMM_PD_CONST_1(_name, _value) static const WN_ALIGN(16) WN_FLOAT64 __WN##_name##_XMM[2] = {_value, _value}
#define __WN_XMM_PD_CONST_2(_name, _value1, _value2) static const WN_ALIGN(16) WN_FLOAT64 __WN##_name##_XMM[2] = {_value1, _value2}

#define __WN_XMM_PI32_CONST_1(_name, _value) static const WN_ALIGN(16) WN_INT32 __WN##_name##_XMM[4] = {static_cast<WN_INT32>(_value), static_cast<WN_INT32>(_value), static_cast<WN_INT32>(_value), static_cast<WN_INT32>(_value)}
#define __WN_XMM_PI32_CONST_4(_name, _value1, _value2, _value3, _value4) static const WN_ALIGN(16) WN_INT32 __WN##_name##_XMM[4] = {static_cast<WN_INT32>(_value1), static_cast<WN_INT32>(_value2), static_cast<WN_INT32>(_value3), static_cast<WN_INT32>(_value4)}
#define __WN_XMM_PI64_CONST_1(_name, _value) static const WN_ALIGN(16) WN_INT64 __WN##_name##_XMM[2] = {static_cast<WN_INT64>(_value), static_cast<WN_INT64>(_value)}
#define __WN_XMM_PI64_CONST_2(_name, _value1, _value2) static const WN_ALIGN(16) WN_INT64 __WN##_name##_XMM[2] = {static_cast<WN_INT64>(_value1), static_cast<WN_INT64>(_value2)}

#define __WN_XMM_GET_PS_CONST(_name) (*reinterpret_cast<const __m128*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PD_CONST(_name) (*reinterpret_cast<const __m128d*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI8_CONST(_name) (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI16_CONST(_name) (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI32_CONST(_name) (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_PI64_CONST(_name) (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))
#define __WN_XMM_GET_SI128_CONST(_name) (*reinterpret_cast<const __m128i*>(__WN##_name##_XMM))

namespace WNMath {
    namespace __WNInternal {
        __WN_XMM_PI32_CONST_1(SignMask1111x32, 0x80000000);
        __WN_XMM_PI32_CONST_4(ElementMask1000x32, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000);
        __WN_XMM_PI32_CONST_4(ElementMask1100x32, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000);
        __WN_XMM_PI32_CONST_4(ElementMask1110x32, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000);

        __WN_XMM_PI64_CONST_1(SignMask11x64, 0x8000000000000000);
        __WN_XMM_PI64_CONST_2(ElementMask10x64, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000);
    }
};

#endif // __WN_MATH_INTERNAL_X86_CONSTANTS_XMM_H__