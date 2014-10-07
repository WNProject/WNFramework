////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_ADD_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_ADD_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperAdd {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements1, const __WNElements<Type, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements1.mValues[i] += _elements2.mValues[i];
                }

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperAdd() {}
        };

        template <typename Type>
        class __WNGeneralHelperAdd<Type, 1> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements1, const __WNElements<Type, 1>& _elements2) {
                _elements1.mValues[0] += _elements2.mValues[0];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperAdd() {}
        };

        template <typename Type>
        class __WNGeneralHelperAdd<Type, 2> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements1, const __WNElements<Type, 2>& _elements2) {
                _elements1.mValues[0] += _elements2.mValues[0];
                _elements1.mValues[1] += _elements2.mValues[1];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperAdd() {}
        };

        template <typename Type>
        class __WNGeneralHelperAdd<Type, 3> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements1, const __WNElements<Type, 3>& _elements2) {
                _elements1.mValues[0] += _elements2.mValues[0];
                _elements1.mValues[1] += _elements2.mValues[1];
                _elements1.mValues[2] += _elements2.mValues[2];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperAdd() {}
        };

        template <typename Type>
        class __WNGeneralHelperAdd<Type, 4> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements1, const __WNElements<Type, 4>& _elements2) {
                _elements1.mValues[0] += _elements2.mValues[0];
                _elements1.mValues[1] += _elements2.mValues[1];
                _elements1.mValues[2] += _elements2.mValues[2];
                _elements1.mValues[3] += _elements2.mValues[3];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperAdd() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpAdd {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements1, const __WNElements<Type, Dimension>& _elements2) {
                return(__WNGeneralHelperAdd<Type, Dimension>::Execute(_elements1, _elements2));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpAdd() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpAdd.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_ADD_H__