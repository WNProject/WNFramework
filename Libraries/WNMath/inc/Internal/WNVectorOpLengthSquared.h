////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_OP_LENGTH_SQUARED_H__
#define __WN_MATH_INTERNAL_VECTOR_OP_LENGTH_SQUARED_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNVectorHelperLengthSquared {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, Dimension>& _elements) {
                Type lengthSquared = static_cast<Type>(0);

                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    lengthSquared += (_elements.mValues[i] * _elements.mValues[i]);
                }

                return(lengthSquared);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperLengthSquared() {}
        };

        template <typename Type>
        class __WNVectorHelperLengthSquared<Type, 1> {
        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, 1>& _elements) {
                return((_elements.mValues[0] * _elements.mValues[0]));
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperLengthSquared() {}
        };

        template <typename Type>
        class __WNVectorHelperLengthSquared<Type, 2> {
        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, 2>& _elements) {
                return((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]));
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperLengthSquared() {}
        };

        template <typename Type>
        class __WNVectorHelperLengthSquared<Type, 3> {
        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, 3>& _elements) {
                return((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]) +
                       (_elements.mValues[2] * _elements.mValues[2]));
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperLengthSquared() {}
        };

        template <typename Type>
        class __WNVectorHelperLengthSquared<Type, 4> {
        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, 4>& _elements) {
                return((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]) +
                       (_elements.mValues[2] * _elements.mValues[2]) + (_elements.mValues[3] * _elements.mValues[3]));
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperLengthSquared() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNVectorOpLengthSquared {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE Type Execute(const __WNElements<Type, Dimension>& _elements) {
                return(__WNVectorHelperLengthSquared<Type, Dimension>::Execute(_elements));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLengthSquared() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_VECTOR_OP_LENGTH_SQUARED_H__