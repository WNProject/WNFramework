////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpIsZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                return((_mm_movemask_ps(_mm_cmpneq_ss(_mm_setzero_ps(), _elements.mXMMValues[0])) & 0x1) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpIsZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpIsZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                return((_mm_movemask_ps(_mm_cmpneq_ps(_mm_setzero_ps(), _elements.mXMMValues[0])) & 0x3) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpIsZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpIsZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                return((_mm_movemask_ps(_mm_cmpneq_ps(_mm_setzero_ps(), _elements.mXMMValues[0])) & 0x7) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpIsZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpIsZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                return(_mm_movemask_ps(_mm_cmpneq_ps(_mm_setzero_ps(), _elements.mXMMValues[0])) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpIsZero() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_SSE_H__