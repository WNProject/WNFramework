////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_MMX_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_MMX_H__

#ifndef __WN_MMX_AVAILABLE
    #error "This header requires a minimum of MMX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(8) __WNElements<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2)>::Value> {
            union {
                __m64 mMMValues[__WNNumberRegisters_MM_PI32<Dimension>::Value];
                WN_INT32 mValues[Dimension];
            };
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_MMX_H__