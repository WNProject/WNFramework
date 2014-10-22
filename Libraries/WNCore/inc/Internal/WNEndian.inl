// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_ENDIAN_INL__
#define __WN_CORE_INTERNAL_ENDIAN_INL__

#ifndef __WN_CORE_ENDIAN_H__
    #error "WNEndian.inl should never be included directly. Please include WNEndian.h instead"
#endif

#include "WNCore/inc/WNAssert.h"

namespace WNCore {
    namespace __WNInternal {
        WN_FORCE_INLINE WN_VOID __WNSwapByte(WN_BYTE* _start, WN_BYTE* _end) {
            *_start ^= *_end;
            *_end ^= *_start;
            *_start ^= *_end;
        }

        template <typename Type, WN_SIZE_T width>
        struct __WNSwapBytes {
            static WN_FORCE_INLINE Type&  Execute(Type& _value) {
                WN_STATIC_ASSERT_DESC(WN_STATIC_ASSERT_DEPENDENT_FAIL<Type>::Value, "You cannot convert this type from big endian"); 

                return(_value);
            }
        };

        template <typename Type>
        struct __WNSwapBytes<Type, 1> {
            static WN_FORCE_INLINE Type&  Execute(Type& _value) {
                WN_STATIC_ASSERT_DESC(WN_STATIC_ASSERT_DEPENDENT_FAIL<Type>::Value, "You should not be trying to endian swap a byte"); 

                return(_value);
            }
        };

        template <typename Type>
        struct __WNSwapBytes<Type, 2> {
            static WN_FORCE_INLINE Type& Execute(Type& _value) {
                _value = (_value >> 8) | ((_value & 0xFF) << 8);

                return(_value);
            }
        };

        template <typename Type>
        struct __WNSwapBytes<Type, 4> {
            static WN_FORCE_INLINE Type& Execute(Type& _value) {
                _value = ((((_value) & 0xFF000000) >> 24) |
                          (((_value) & 0x00FF0000) >> 8) |
                          (((_value) & 0x0000FF00) << 8) |
                          (((_value) & 0x000000FF) << 24));

                return(_value);
            }
        };

        template <typename Type>
        struct __WNSwapBytes<Type, 8> {
            static WN_FORCE_INLINE Type& Execute(Type& _value) {
                _value = ((((_value) & 0xFF00000000000000ULL) >> 56) |
                          (((_value) & 0x00FF000000000000ULL) >> 40) |
                          (((_value) & 0x0000FF0000000000ULL) >> 24) |
                          (((_value) & 0x000000FF00000000ULL) >> 8) |
                          (((_value) & 0x00000000FF000000ULL) << 8) |
                          (((_value) & 0x0000000000FF0000ULL) << 24) |
                          (((_value) & 0x000000000000FF00ULL) << 40) |
                          (((_value) & 0x00000000000000FFULL) << 56));

                return(_value);
            }
        };

        template <>
        struct __WNSwapBytes<WN_FLOAT32, 4> {
            static WN_FORCE_INLINE WN_FLOAT32& Execute(WN_FLOAT32& _value) {
                WN_BYTE* bytes = reinterpret_cast<WN_BYTE*>(&_value);

                __WNSwapByte(&bytes[0], &bytes[3]);
                __WNSwapByte(&bytes[1], &bytes[2]);

                return(_value);
            }
        };

        template <>
        struct __WNSwapBytes<WN_FLOAT64, 8> {
            static WN_FORCE_INLINE WN_FLOAT64& Execute(WN_FLOAT64& _value) {
                WN_BYTE* bytes = reinterpret_cast<WN_BYTE*>(&_value);

                __WNSwapByte(&bytes[0], &bytes[7]);
                __WNSwapByte(&bytes[1], &bytes[6]);
                __WNSwapByte(&bytes[2], &bytes[5]);
                __WNSwapByte(&bytes[3], &bytes[4]);

                return(_value);
            }
        };
    }

    #ifdef _WN_ENDIAN_BIG
        template <typename Type>
        WN_FORCE_INLINE Type& WNToBigEndian(Type& _value) {
            return(_value);
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNFromBigEndian(Type& _value) {
            return(_value);
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNToLittleEndian(Type& _value) {
            return(__WNInternal::__WNSwapBytes<Type, sizeof(Type)>::Execute(_value));
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNFromLittleEndian(Type& _value) {
            return(__WNInternal::__WNSwapBytes<Type, sizeof(Type)>::Execute(_value));
        }
    #else
        template <typename Type>
        WN_FORCE_INLINE Type& WNToBigEndian(Type& _value) {
            return(__WNInternal::__WNSwapBytes<Type, sizeof(Type)>::Execute(_value));
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNFromBigEndian(Type& _value) {
            return(__WNInternal::__WNSwapBytes<Type, sizeof(Type)>::Execute(_value));
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNToLittleEndian(Type& _value) {
            return(_value);
        }

        template <typename Type>
        WN_FORCE_INLINE Type& WNFromLittleEndian(Type& _value) {
            return(_value);
        }
    #endif
}

#endif // __WN_CORE_INTERNAL_ENDIAN_INL__
