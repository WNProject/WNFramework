// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                _elements1.mMMValues[0] = _m_pfsub(_elements1.mMMValues[0], _elements2.mMMValues[0]);

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                _elements1.mMMValues[0] = _m_pfsub(_elements1.mMMValues[0], _elements2.mMMValues[0]);
                _elements1.mMMValues[1] = _m_pfsub(_elements1.mMMValues[1], _elements2.mMMValues[1]);

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_MM_PS<Dimension>::Value; ++i) {
                    _elements1.mMMValues[i] = _m_pfsub(_elements1.mMMValues[i], _elements2.mMMValues[i]);
                }

                _m_femms();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_3DNOW_H__