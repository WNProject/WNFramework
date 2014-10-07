////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                __m64 result = _m_pfrcp(_elements2.mMMValues[0]);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(_elements2.mMMValues[0], result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements1.mMMValues[0] = _mm_and_si64(_m_pfmul(_elements1.mMMValues[0], result), __WN_MM_GET_PS_CONST(ElementMask1000x32));

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                __m64 result = _m_pfrcp(_elements2.mMMValues[0]);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(_elements2.mMMValues[0], result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements1.mMMValues[0] = _m_pfmul(_elements1.mMMValues[0], result);

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                __m64 result1 = _m_pfrcp(_elements2.mMMValues[0]);
                __m64 result2 = _m_pfrcp(_elements2.mMMValues[1]);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine1 = _m_pfrcpit1(_elements2.mMMValues[0], result1);
                    const __m64 refine2 = _m_pfrcpit1(_elements2.mMMValues[1], result2);

                    result1 = _m_pfrcpit2(refine1, result1);
                    result2 = _m_pfrcpit2(refine2, result2);
                #endif

                _elements1.mMMValues[0] = _m_pfmul(_elements1.mMMValues[0], result1);
                _elements1.mMMValues[1] = _mm_and_si64(_m_pfmul(_elements1.mMMValues[1], result2), __WN_MM_GET_PS_CONST(ElementMask10x32));

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                __m64 result1 = _m_pfrcp(_elements2.mMMValues[0]);
                __m64 result2 = _m_pfrcp(_elements2.mMMValues[1]);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine1 = _m_pfrcpit1(_elements2.mMMValues[0], result1);
                    const __m64 refine2 = _m_pfrcpit1(_elements2.mMMValues[1], result2);

                    result1 = _m_pfrcpit2(refine1, result1);
                    result2 = _m_pfrcpit2(refine2, result2);
                #endif

                _elements1.mMMValues[0] = _m_pfmul(_elements1.mMMValues[0], result1);
                _elements1.mMMValues[1] = _m_pfmul(_elements1.mMMValues[1], result2);

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PS<Dimension>::Value; ++i) {
                    __m64 result = _m_pfrcp(_elements2.mMMValues[i]);

                    #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m64 refine1 = _m_pfrcpit1(_elements2.mMMValues[i], result);

                        result = _m_pfrcpit2(refine1, result);
                    #endif

                    _elements1.mMMValues[i] = _m_pfmul(_elements1.mMMValues[i], result);
                }

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivide<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                const WN_UINT32 count = __WNNumberRegisters_MM_PS<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < count; ++i) {
                    __m64 result = _m_pfrcp(_elements2.mMMValues[i]);

                    #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m64 refine1 = _m_pfrcpit1(_elements2.mMMValues[i], result);

                        result = _m_pfrcpit2(refine1, result);
                    #endif

                    _elements1.mMMValues[i] = _m_pfmul(_elements1.mMMValues[i], result);
                }

                __m64 result = _m_pfrcp(_elements2.mMMValues[count]);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(_elements2.mMMValues[count], result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements1.mMMValues[count] = _mm_and_si64(_m_pfmul(_elements1.mMMValues[count], result), __WN_MM_GET_PS_CONST(ElementMask10x32));

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpDivide() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_3DNOW_H__