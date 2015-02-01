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
    namespace internal {
        WN_FORCE_INLINE wn_void __WNSwapByte(wn_byte* _start, wn_byte* _end) {
            *_start ^= *_end;
            *_end ^= *_start;
            *_start ^= *_end;
        }

        template <typename type, wn_size_t width>
        struct __WNSwapBytes {
            static WN_FORCE_INLINE type&  Execute(type& _value) {
                static_assert(WN_STATIC_ASSERT_DEPENDENT_FAIL<type>::Value, "You cannot convert this type from big endian"); 

                return(_value);
            }
        };

        template <typename type>
        struct __WNSwapBytes<type, 1> {
            static WN_FORCE_INLINE type&  Execute(type& _value) {
                static_assert(WN_STATIC_ASSERT_DEPENDENT_FAIL<type>::Value, "You should not be trying to endian swap a byte"); 

                return(_value);
            }
        };

        template <typename type>
        struct __WNSwapBytes<type, 2> {
            static WN_FORCE_INLINE type& Execute(type& _value) {
                _value = (_value >> 8) | ((_value & 0xFF) << 8);

                return(_value);
            }
        };

        template <typename type>
        struct __WNSwapBytes<type, 4> {
            static WN_FORCE_INLINE type& Execute(type& _value) {
                _value = ((((_value) & 0xFF000000) >> 24) |
                          (((_value) & 0x00FF0000) >> 8) |
                          (((_value) & 0x0000FF00) << 8) |
                          (((_value) & 0x000000FF) << 24));

                return(_value);
            }
        };

        template <typename type>
        struct __WNSwapBytes<type, 8> {
            static WN_FORCE_INLINE type& Execute(type& _value) {
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
        struct __WNSwapBytes<wn_float32, 4> {
            static WN_FORCE_INLINE wn_float32& Execute(wn_float32& _value) {
                wn_byte* bytes = reinterpret_cast<wn_byte*>(&_value);

                __WNSwapByte(&bytes[0], &bytes[3]);
                __WNSwapByte(&bytes[1], &bytes[2]);

                return(_value);
            }
        };

        template <>
        struct __WNSwapBytes<wn_float64, 8> {
            static WN_FORCE_INLINE wn_float64& Execute(wn_float64& _value) {
                wn_byte* bytes = reinterpret_cast<wn_byte*>(&_value);

                __WNSwapByte(&bytes[0], &bytes[7]);
                __WNSwapByte(&bytes[1], &bytes[6]);
                __WNSwapByte(&bytes[2], &bytes[5]);
                __WNSwapByte(&bytes[3], &bytes[4]);

                return(_value);
            }
        };
    }

    #ifdef _WN_ENDIAN_BIG
        template <typename type>
        WN_FORCE_INLINE type& WNToBigEndian(type& _value) {
            return(_value);
        }

        template <typename type>
        WN_FORCE_INLINE type& WNFromBigEndian(type& _value) {
            return(_value);
        }

        template <typename type>
        WN_FORCE_INLINE type& WNToLittleEndian(type& _value) {
            return(internal::__WNSwapBytes<type, sizeof(type)>::Execute(_value));
        }

        template <typename type>
        WN_FORCE_INLINE type& WNFromLittleEndian(type& _value) {
            return(internal::__WNSwapBytes<type, sizeof(type)>::Execute(_value));
        }
    #else
        template <typename type>
        WN_FORCE_INLINE type& WNToBigEndian(type& _value) {
            return(internal::__WNSwapBytes<type, sizeof(type)>::Execute(_value));
        }

        template <typename type>
        WN_FORCE_INLINE type& WNFromBigEndian(type& _value) {
            return(internal::__WNSwapBytes<type, sizeof(type)>::Execute(_value));
        }

        template <typename type>
        WN_FORCE_INLINE type& WNToLittleEndian(type& _value) {
            return(_value);
        }

        template <typename type>
        WN_FORCE_INLINE type& WNFromLittleEndian(type& _value) {
            return(_value);
        }
    #endif
}

#endif // __WN_CORE_INTERNAL_ENDIAN_INL__
