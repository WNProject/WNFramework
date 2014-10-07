////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_SUBTRACT_NUMBER_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_SUBTRACT_NUMBER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperSubtractNumber {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] -= _number;
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperSubtractNumber() {}
        };

        template <typename Type>
        class __WNGeneralHelperSubtractNumber<Type, 1> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements, const Type& _number) {
                _elements.mValues[0] -= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperSubtractNumber() {}
        };

        template <typename Type>
        class __WNGeneralHelperSubtractNumber<Type, 2> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements, const Type& _number) {
                _elements.mValues[0] -= _number;
                _elements.mValues[1] -= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperSubtractNumber() {}
        };

        template <typename Type>
        class __WNGeneralHelperSubtractNumber<Type, 3> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements, const Type& _number) {
                _elements.mValues[0] -= _number;
                _elements.mValues[1] -= _number;
                _elements.mValues[2] -= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperSubtractNumber() {}
        };

        template <typename Type>
        class __WNGeneralHelperSubtractNumber<Type, 4> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements, const Type& _number) {
                _elements.mValues[0] -= _number;
                _elements.mValues[1] -= _number;
                _elements.mValues[2] -= _number;
                _elements.mValues[3] -= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperSubtractNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpSubtractNumber {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                return(__WNGeneralHelperSubtractNumber<Type, Dimension>::Execute(_elements, _number));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtractNumber() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpSubtractNumber.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_SUBTRACT_NUMBER_H__