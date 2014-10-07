////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_UTILITIES_YMM_H__
#define __WN_MATH_INTERNAL_X86_UTILITIES_YMM_H__

#ifndef __WN_YMM_ZEROUPPER
    #ifndef __WN_VEX_AVAILABLE
        #define __WN_YMM_ZEROUPPER() _mm256_zeroupper()
    #else
        #define __WN_YMM_ZEROUPPER()
    #endif
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_YMM_PI32 {
            enum {
                Value = (Dimension % 8) >= 5 ? ((Dimension + 7) / 8) : (Dimension / 8)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_YMM_PI64 {
            enum {
                Value = (Dimension % 4) >= 3 ? ((Dimension + 3) / 4) : (Dimension / 4)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_YMM_PS {
            enum {
                Value = (Dimension % 8) >= 5 ? ((Dimension + 7) / 8) : (Dimension / 8)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_YMM_PD {
            enum {
                Value = (Dimension % 4) >= 3 ? ((Dimension + 3) / 4) : (Dimension / 4)
            };
        };
    }
};

#endif // __WN_MATH_INTERNAL_X86_UTILITIES_YMM_H__