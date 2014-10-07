////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const WN_FLOAT32 one = 1.0f;
                const __m64 number = _mm_setr_pi32(*reinterpret_cast<const WN_INT32*>(&_number), *reinterpret_cast<const WN_INT32*>(&one));
                __m64 result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements.mMMValues[0] = _m_pfmul(_elements.mMMValues[0], result);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));
                __m64 result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(_number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements.mMMValues[0] = _m_pfmul(_elements.mMMValues[0], result);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const WN_FLOAT32 one = 1.0f;
                const WN_INT32 number = *reinterpret_cast<const WN_INT32*>(&_number);
                const __m64 number1 = _mm_set1_pi32(number);
                const __m64 number2 = _mm_setr_pi32(number, *reinterpret_cast<const WN_INT32*>(&one));
                __m64 result1 = _m_pfrcp(number1);
                __m64 result2 = _m_pfrcp(number2);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine1 = _m_pfrcpit1(number1, result1);
                    const __m64 refine2 = _m_pfrcpit1(number2, result2);

                    result1 = _m_pfrcpit2(refine1, result1);
                    result2 = _m_pfrcpit2(refine2, result2);
                #endif

                _elements.mMMValues[0] = _m_pfmul(_elements.mMMValues[0], result1);
                _elements.mMMValues[1] = _m_pfmul(_elements.mMMValues[1], result2);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };


        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));
                __m64 result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements.mMMValues[0] = _m_pfmul(_elements.mMMValues[0], result);
                _elements.mMMValues[1] = _m_pfmul(_elements.mMMValues[1], result);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));
                __m64 result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PS<Dimension>::Value; ++i) {
                    _elements.mMMValues[i] = _m_pfmul(_elements.mMMValues[i], result);
                }

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const WN_UINT32 count = __WNNumberRegisters_MM_PS<Dimension>::Value - 1;
                const WN_INT32 inumber = *reinterpret_cast<const WN_INT32*>(&_number);
                __m64 number = _mm_set1_pi32(inumber);
                __m64 result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                for (WN_UINT32 i = 0; i < count; ++i) {
                    _elements.mMMValues[i] = _m_pfmul(_elements.mMMValues[i], result);
                }

                const WN_FLOAT32 one = 1.0f;

                number = _mm_setr_pi32(inumber, *reinterpret_cast<const WN_INT32*>(&one));
                result = _m_pfrcp(number);

                #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                    const __m64 refine = _m_pfrcpit1(number, result);

                    result = _m_pfrcpit2(refine, result);
                #endif

                _elements.mMMValues[count] = _m_pfmul(_elements.mMMValues[count], result);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_3DNOW_H__