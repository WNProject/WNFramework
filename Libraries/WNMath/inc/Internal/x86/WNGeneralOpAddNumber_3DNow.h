// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mMMValues[0] = _m_pfadd(_elements.mMMValues[0], _m_from_float(_number));

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mMMValues[0] = _m_pfadd(_elements.mMMValues[0], _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number)));

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mMMValues[1] = _m_pfadd(_elements.mMMValues[1], _m_from_float(_number));
                _elements.mMMValues[0] = _m_pfadd(_elements.mMMValues[0], _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number)));

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };


        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));

                _elements.mMMValues[0] = _m_pfadd(_elements.mMMValues[0], number);
                _elements.mMMValues[1] = _m_pfadd(_elements.mMMValues[1], number);

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PS<Dimension>::Value; ++i) {
                    _elements.mMMValues[i] = _m_pfadd(_elements.mMMValues[i], number);
                }

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m64 number = _mm_set1_pi32(*reinterpret_cast<const WN_INT32*>(&_number));
                const WN_UINT32 count = __WNNumberRegisters_MM_PS<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < count; ++i) {
                    _elements.mMMValues[i] = _m_pfadd(_elements.mMMValues[i], number);
                }

                _elements.mMMValues[count] = _m_pfadd(_elements.mMMValues[count], _m_from_float(_number));

                _m_femms();

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_3DNOW_H__