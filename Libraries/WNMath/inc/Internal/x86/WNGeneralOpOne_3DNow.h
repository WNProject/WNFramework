////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                _elements.mMMValues[0] = _m_from_float(1.0f);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                _elements.mMMValues[0] = __WN_MM_GET_PS_CONST(One11x32);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m64 number = _m_from_float(1.0f);

                _elements.mMMValues[0] = __WN_MM_GET_PS_CONST(One11x32);
                _elements.mMMValues[1] = number;

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m64 number = __WN_MM_GET_PS_CONST(One11x32);

                _elements.mMMValues[0] = number;
                _elements.mMMValues[1] = number;

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimesion % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m64 number = __WN_MM_GET_PS_CONST(One);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PS<Dimension>::Value; ++i) {
                    _elements.mMMValues[i] = number;
                }

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimesion % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m64 number = __WN_MM_GET_PS_CONST(One);
                const WN_UINT32 count = __WNNumberRegisters_MM_PS<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < count; ++i) {
                    _elements.mMMValues[i] = number;
                }

                _elements.mMMValues[count] = _m_from_float(1.0f);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_3DNOW_H__