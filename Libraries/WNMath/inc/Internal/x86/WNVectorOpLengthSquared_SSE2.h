////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE2_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLengthSquared<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                return(_mm_cvtsd_f64(_mm_mul_sd(_elements.mXMMValues[0], _elements.mXMMValues[0])));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLengthSquared() {}
        };
    }
}

#ifndef __WN_SSE3_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared_SSE2-NoSSE3.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE2_H__