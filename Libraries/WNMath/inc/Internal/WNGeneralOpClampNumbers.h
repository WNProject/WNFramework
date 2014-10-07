////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_NUMBERS_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_NUMBERS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperClampNumbers {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _minimum, const Type& _maximum) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] = WNClamp(_elements.mValues[i], _minimum, _maximum);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClampNumbers() {}
        };

        template <typename Type>
        class __WNGeneralHelperClampNumbers<Type, 1> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements, const Type& _minimum, const Type& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum, _maximum);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClampNumbers() {}
        };

        template <typename Type>
        class __WNGeneralHelperClampNumbers<Type, 2> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements, const Type& _minimum, const Type& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum, _maximum);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum, _maximum);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClampNumbers() {}
        };

        template <typename Type>
        class __WNGeneralHelperClampNumbers<Type, 3> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements, const Type& _minimum, const Type& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum, _maximum);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum, _maximum);
                _elements.mValues[2] = WNClamp(_elements.mValues[2], _minimum, _maximum);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClampNumbers() {}
        };

        template <typename Type>
        class __WNGeneralHelperClampNumbers<Type, 4> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements, const Type& _minimum, const Type& _maximum) {
                _elements.mValues[0] = WNClamp(_elements.mValues[0], _minimum, _maximum);
                _elements.mValues[1] = WNClamp(_elements.mValues[1], _minimum, _maximum);
                _elements.mValues[2] = WNClamp(_elements.mValues[2], _minimum, _maximum);
                _elements.mValues[3] = WNClamp(_elements.mValues[3], _minimum, _maximum);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperClampNumbers() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpClampNumbers {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _minimum, const Type& _maximum) {
                return(__WNGeneralHelperClampNumbers<Type, Dimension>::Execute(_elements, _minimum, _maximum));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpClampNumbers() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_GENERAL_OP_CLAMP_NUMBERS_H__