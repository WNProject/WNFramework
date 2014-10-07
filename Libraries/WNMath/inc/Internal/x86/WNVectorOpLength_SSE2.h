////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d squared = _mm_mul_pd(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128d lengthSquared = _mm_add_sd(squared, _mm_shuffle_pd(squared, squared, _MM_SHUFFLE2(0, 1)));
                const __m128d length = _mm_sqrt_sd(_mm_setzero_pd(), lengthSquared);

                return(_mm_cvtsd_f64(length));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLength_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_H__