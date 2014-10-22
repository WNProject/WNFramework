// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperClamp {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const __WNElements<Type, Dimension>& _minimum, const __WNElements<Type, Dimension>& _maximum) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] = WNClamp(_elements.mValues[i], _minimum.mValues[i], _maximum.mValues[i]);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClamp() {}
        };

        template <typename Type>
        class __WNGeneralHelperClamp<Type, 1> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements, const __WNElements<Type, 1>& _minimum, const __WNElements<Type, 1>& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum.mValues[0], _maximum.mValues[0]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClamp() {}
        };

        template <typename Type>
        class __WNGeneralHelperClamp<Type, 2> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements, const __WNElements<Type, 2>& _minimum, const __WNElements<Type, 2>& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum.mValues[0], _maximum.mValues[0]);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum.mValues[1], _maximum.mValues[1]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClamp() {}
        };

        template <typename Type>
        class __WNGeneralHelperClamp<Type, 3> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements, const __WNElements<Type, 3>& _minimum, const __WNElements<Type, 3>& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum.mValues[0], _maximum.mValues[0]);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum.mValues[1], _maximum.mValues[1]);
                _elements.mValues[2] = WNClamp(_elements.mValues[2], _minimum.mValues[2], _maximum.mValues[2]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClamp() {}
        };

        template <typename Type>
        class __WNGeneralHelperClamp<Type, 4> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements, const __WNElements<Type, 4>& _minimum, const __WNElements<Type, 4>& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum.mValues[0], _maximum.mValues[0]);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum.mValues[1], _maximum.mValues[1]);
                _elements.mValues[2] = WNClamp(_elements.mValues[2], _minimum.mValues[2], _maximum.mValues[2]);
                _elements.mValues[3] = WNClamp(_elements.mValues[3], _minimum.mValues[3], _maximum.mValues[3]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClamp() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpClamp {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const __WNElements<Type, Dimension>& _minimum, const __WNElements<Type, Dimension>& _maximum) {
                return(__WNGeneralHelperClamp<Type, Dimension>::Execute(_elements, _minimum, _maximum));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpClamp() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpClamp.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_H__