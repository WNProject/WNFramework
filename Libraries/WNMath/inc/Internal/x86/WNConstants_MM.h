////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__
#define __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__

#define __WN_MM_PS_CONST_1(_name, _value) static const WN_FLOAT32 __WN##_name##_MM[2] = {_value, _value}
#define __WN_MM_PS_CONST_2(_name, _value1, _value2) static const WN_FLOAT32 __WN##_name##_MM[2] = {_value1, _value2}

#define __WN_MM_PI32_CONST_1(_name, _value) static const WN_INT32 __WN##_name##_MM[2] = {static_cast<WN_INT32>(_value), static_cast<WN_INT32>(_value)}
#define __WN_MM_PI32_CONST_2(_name, _value1, _value2) static const WN_INT32 __WN##_name##_MM[2] = {static_cast<WN_INT32>(_value1), static_cast<WN_INT32>(_value2)}

#define __WN_MM_GET_PS_CONST(_name) (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI8_CONST(_name) (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI16_CONST(_name) (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_PI32_CONST(_name) (*reinterpret_cast<const __m64*>(__WN##_name##_MM))
#define __WN_MM_GET_SI64_CONST(_name) (*reinterpret_cast<const __m64*>(__WN##_name##_MM))

namespace WNMath {
    namespace __WNInternal {
        __WN_MM_PI32_CONST_1(SignMask11x32, 0x80000000);
        __WN_MM_PI32_CONST_2(ElementMask10x32, 0xFFFFFFFF, 0x00000000);

        __WN_MM_PS_CONST_1(One11x32, 1.0f);
    }
};

#endif // __WN_MATH_INTERNAL_X86_CONSTANTS_MM_H__