////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_UTILITIES_XMM_H__
#define __WN_MATH_INTERNAL_X86_UTILITIES_XMM_H__

#define __WN_XMM_CAST_PS_TO_SI128(_value) *reinterpret_cast<__m128i*>(&_value)

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_XMM_PI32 {
            enum {
                Value = (Dimension % 4) == 1 ? (Dimension / 4) : ((Dimension + 3) / 4)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_XMM_PI64 {
            enum {
                Value = (Dimension % 2) == 1 ? (Dimension / 2) : ((Dimension + 1) / 2)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_XMM_PS {
            enum {
                Value = ((Dimension + 3) / 4)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_XMM_PD {
            enum {
                Value = ((Dimension + 1) / 2)
            };
        };
    }
};

#endif // __WN_MATH_INTERNAL_X86_UTILITIES_XMM_H__