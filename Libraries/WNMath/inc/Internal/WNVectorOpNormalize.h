////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_OP_NORMALIZE_H__
#define __WN_MATH_INTERNAL_VECTOR_OP_NORMALIZE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNVectorHelperOpNormalize {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements) {
                Type lengthSquared = static_cast<Type>(0);

                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    lengthSquared += (_elements.mValues[i] * _elements.mValues[i]);
                }

                const Type lengthInverse = WNInvSqrt(lengthSquared);

                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] *= lengthInverse;
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperOpNormalize() {}
        };

        template <typename Type>
        class __WNVectorHelperOpNormalize<Type, 1> {
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements) {
                _elements.mValues[0]  = static_cast<Type>(1);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperOpNormalize() {}
        };

        template <typename Type>
        class __WNVectorHelperOpNormalize<Type, 2> {
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements) {
                const Type lengthInverse = WNInvSqrt((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]));

                _elements.mValues[0] *= lengthInverse;
                _elements.mValues[1] *= lengthInverse;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperOpNormalize() {}
        };

        template <typename Type>
        class __WNVectorHelperOpNormalize<Type, 3> {
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements) {
                const Type lengthInverse = WNInvSqrt((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]) +
                                                     (_elements.mValues[2] * _elements.mValues[2]));

                _elements.mValues[0] *= lengthInverse;
                _elements.mValues[1] *= lengthInverse;
                _elements.mValues[2] *= lengthInverse;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperOpNormalize() {}
        };

        template <typename Type>
        class __WNVectorHelperOpNormalize<Type, 4> {
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements) {
                const Type lengthInverse = WNInvSqrt((_elements.mValues[0] * _elements.mValues[0]) + (_elements.mValues[1] * _elements.mValues[1]) +
                                                     (_elements.mValues[2] * _elements.mValues[2]) + (_elements.mValues[3] * _elements.mValues[3]));

                _elements.mValues[0] *= lengthInverse;
                _elements.mValues[1] *= lengthInverse;
                _elements.mValues[2] *= lengthInverse;
                _elements.mValues[3] *= lengthInverse;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorHelperOpNormalize() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNVectorOpNormalize {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");
            WN_STATIC_ASSERT_DESC(WNCore::WNIsReal<Type>::Value, "Operation only supports real number type.  Please use a real number type.");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements) {
                return(__WNVectorHelperOpNormalize<Type, Dimension>::Execute(_elements));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpNormalize() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNVectorOpNormalize.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_VECTOR_OP_NORMALIZE_H__