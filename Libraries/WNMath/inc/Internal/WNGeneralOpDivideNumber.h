////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_DIVIDE_NUMBER_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_DIVIDE_NUMBER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralHelperDivideNumber {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] /= _number;
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                _elements.mValues[0] /= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2 && !WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements, const Type& _number) {
                _elements.mValues[0] /= _number;
                _elements.mValues[1] /= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2 && WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                const Type inverseNumber = static_cast<Type>(1) / _number;

                _elements.mValues[0] *= inverseNumber;
                _elements.mValues[1] *= inverseNumber;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 && !WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                _elements.mValues[0] /= _number;
                _elements.mValues[1] /= _number;
                _elements.mValues[2] /= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 && WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                const Type inverseNumber = static_cast<Type>(1) / _number;

                _elements.mValues[0] *= inverseNumber;
                _elements.mValues[1] *= inverseNumber;
                _elements.mValues[2] *= inverseNumber;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4 && !WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                _elements.mValues[0] /= _number;
                _elements.mValues[1] /= _number;
                _elements.mValues[2] /= _number;
                _elements.mValues[3] /= _number;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4 && WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                const Type inverseNumber = static_cast<Type>(1) / _number;

                _elements.mValues[0] *= inverseNumber;
                _elements.mValues[1] *= inverseNumber;
                _elements.mValues[2] *= inverseNumber;
                _elements.mValues[3] *= inverseNumber;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperDivideNumber<Type, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && WNCore::WNIsReal<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                const Type inverseNumber = static_cast<Type>(1) / _number;

                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] *= inverseNumber;
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperDivideNumber() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpDivideNumber {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                return(__WNGeneralHelperDivideNumber<Type, Dimension>::Execute(_elements, _number));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpDivideNumber() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_DIVIDE_NUMBER_H__