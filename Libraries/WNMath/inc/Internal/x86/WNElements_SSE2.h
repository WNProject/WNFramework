////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_SSE2_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2)>::Value> {
            union {
                __m128i mXMMValues[__WNNumberRegisters_XMM_PI32<Dimension>::Value];
                WN_INT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2)>::Value> {
            union {
                __m128i mXMMValues[__WNNumberRegisters_XMM_PI64<Dimension>::Value];
                WN_INT64 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 1 && Dimension <= 2)>::Value> {
            union {
                __m128d mXMMValues[1];
                WN_FLOAT64 mValues[Dimension];
            };
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNElements_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_SSE2_H__