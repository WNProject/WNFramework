////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_div_sd(_elements.mXMMValues[0], _mm_load_sd(&_number));

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_div_pd(_elements.mXMMValues[0], _mm_set1_pd(_number));

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_H__