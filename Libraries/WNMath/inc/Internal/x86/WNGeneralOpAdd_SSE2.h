////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_epi32(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_epi32(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                const WN_UINT32 lastElement = Dimension - 1;

                _elements1.mValues[lastElement] += _elements2.mValues[lastElement];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_epi32(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements1, const __WNElements<WN_INT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_epi64(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements1, const __WNElements<WN_INT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_epi64(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                _elements1.mValues[2] += _elements2.mValues[2];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements1, const __WNElements<WN_INT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_epi64(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);
                _elements1.mXMMValues[1] = _mm_add_epi64(_elements1.mXMMValues[1], _elements2.mXMMValues[1]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements1, const __WNElements<WN_INT64, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI64<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_epi64(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements1, const __WNElements<WN_INT64, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI64<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_epi64(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                const WN_UINT32 lastElement = Dimension - 1;

                _elements1.mValues[lastElement] += _elements2.mValues[lastElement];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_sd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_add_pd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpAdd_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE2_H__