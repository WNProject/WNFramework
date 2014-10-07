////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "WNExtendedTypes.h"

namespace WNCore {
    template <typename Type>
    class WNAlignment {
        struct __WNAlignmentHelper {
            const WN_BYTE mPadding;
            const Type mType;
        };

    public:
        enum {
            Value = sizeof(__WNAlignmentHelper) - sizeof(Type)
        };
    };

    template <typename Type>
    struct WNIsInteger {
        enum {
            Value = WN_FALSE
        };
    };

    template <>
    struct WNIsInteger<WN_UINT8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_UINT16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_UINT32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_UINT64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_INT8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_INT16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_INT32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsInteger<WN_INT64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <typename Type>
    struct WNIsFixedPoint {
        enum {
            Value = WN_FALSE
        };
    };

    template <>
    struct WNIsFixedPoint<WN_FIXED8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFixedPoint<WN_FIXED16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFixedPoint<WN_FIXED32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFixedPoint<WN_FIXED64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <typename Type>
    struct WNIsFloatingPoint {
        enum {
            Value = WN_FALSE
        };
    };

    template <>
    struct WNIsFloatingPoint<WN_FLOAT8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFloatingPoint<WN_FLOAT16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFloatingPoint<WN_FLOAT32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsFloatingPoint<WN_FLOAT64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <typename Type>
    struct WNIsReal {
        enum {
            Value = WNIsFloatingPoint<Type>::Value || WNIsFixedPoint<Type>::Value
        };
    };

    template <typename Type>
    struct WNIsSigned {
        enum {
            Value = WN_FALSE
        };
    };

    template <>
    struct WNIsSigned<WN_INT8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_INT16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_INT32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_INT64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FLOAT8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FLOAT16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FLOAT32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FLOAT64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FIXED8> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FIXED16> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FIXED32> {
        enum {
            Value = WN_TRUE
        };
    };

    template <>
    struct WNIsSigned<WN_FIXED64> {
        enum {
            Value = WN_TRUE
        };
    };

    template <typename Type>
    struct WNIsUnsigned {
        enum {
            Value = !WNIsSigned<Type>::Value
        };
    };

    template <WN_BOOL, typename = WN_VOID>
    struct WNEnableWhen {};

    template <typename Type>
    struct WNEnableWhen<WN_TRUE, Type> {
        typedef Type Value;
    };
}

#endif // __WN_CORE_TYPE_TRAITS_H__