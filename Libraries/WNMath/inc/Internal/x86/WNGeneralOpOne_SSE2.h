////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setr_epi32(1, 1, 0, 0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setr_epi32(1, 1, 1, 0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set1_epi32(1);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set1_epi64x(1);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set1_epi64x(1);
                _elements.mValues[Dimension - 1] = 1;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                const __m128i one = _mm_set1_epi64x(1);

                _elements.mXMMValues[0] = one;
                _elements.mXMMValues[1] = one;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set_sd(1.0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set1_pd(1.0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpOne_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_H__