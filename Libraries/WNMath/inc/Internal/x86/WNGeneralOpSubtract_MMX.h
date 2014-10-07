////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_MMX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_MMX_H__

#ifndef __WN_MMX_AVAILABLE
    #error "This header requires a minimum of MMX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                _elements1.mMMValues[0] = _mm_sub_pi32(_elements1.mMMValues[0], _elements2.mMMValues[0]);

                _mm_empty();

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                _elements1.mMMValues[0] = _mm_sub_pi32(_elements1.mMMValues[0], _elements2.mMMValues[0]);

                _mm_empty();

                _elements1.mValues[2] -= _elements2.mValues[2];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                _elements1.mMMValues[0] = _mm_sub_pi32(_elements1.mMMValues[0], _elements2.mMMValues[0]);
                _elements1.mMMValues[1] = _mm_sub_pi32(_elements1.mMMValues[1], _elements2.mMMValues[1]);

                _mm_empty();

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PI32<Dimension>::Value; ++i) {
                    _elements1.mMMValues[i] = _mm_sub_pi32(_elements1.mMMValues[i], _elements2.mMMValues[i]);
                }

                _mm_empty();

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PI32<Dimension>::Value; ++i) {
                    _elements1.mMMValues[i] = _mm_sub_pi32(_elements1.mMMValues[i], _elements2.mMMValues[i]);
                }

                _mm_empty();

                const WN_UINT32 lastElement = Dimension - 1;

                _elements1.mValues[lastElement] -= _elements2.mValues[lastElement];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_MMX_H__