// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_ENDIAN_H__
#define __WN_CORE_ENDIAN_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
    namespace core {
        namespace internal {
            WN_FORCE_INLINE wn_void swap_byte(wn_byte& _first, wn_byte& _second) {
                _first ^= _second;
                _second ^= _first;
                _first ^= _second;
            }

            template <typename _Type, const wn_size_t _Size = sizeof(_Type)>
            struct swap_bytes {
                static WN_FORCE_INLINE _Type execute(const _Type _value) {
                    static_assert(dependent_false<_Type>::value, "You cannot convert this _Type from big endian");

                    return(_value);
                }
            };

            template <typename _Type>
            struct swap_bytes<_Type, 1> {
                static WN_FORCE_INLINE _Type execute(const _Type _value) {
                    return(_value);
                }
            };

            template <typename _Type>
            struct swap_bytes<_Type, 2> {
                static WN_FORCE_INLINE _Type execute(const _Type _value) {
                    return((_value >> 8) | ((_value & 0xFF) << 8));
                }
            };

            template <typename _Type>
            struct swap_bytes<_Type, 4> {
                static WN_FORCE_INLINE _Type execute(const _Type _value) {
                    return(((((_value) & 0xFF000000) >> 24) |
                            (((_value) & 0x00FF0000) >> 8) |
                            (((_value) & 0x0000FF00) << 8) |
                            (((_value) & 0x000000FF) << 24)));
                }
            };

            template <typename _Type>
            struct swap_bytes<_Type, 8> {
                static WN_FORCE_INLINE _Type execute(const _Type _value) {
                    return(((((_value) & 0xFF00000000000000ULL) >> 56) |
                            (((_value) & 0x00FF000000000000ULL) >> 40) |
                            (((_value) & 0x0000FF0000000000ULL) >> 24) |
                            (((_value) & 0x000000FF00000000ULL) >> 8) |
                            (((_value) & 0x00000000FF000000ULL) << 8) |
                            (((_value) & 0x0000000000FF0000ULL) << 24) |
                            (((_value) & 0x000000000000FF00ULL) << 40) |
                            (((_value) & 0x00000000000000FFULL) << 56)));
                }
            };

            template <>
            struct swap_bytes<wn_float32, 4> {
                static WN_FORCE_INLINE wn_float32 execute(const wn_float32 _value) {
                    wn_float32 value = _value;
                    wn_byte* bytes = reinterpret_cast<wn_byte*>(&value);

                    swap_byte(bytes[0], bytes[3]);
                    swap_byte(bytes[1], bytes[2]);

                    return(value);
                }
            };

            template <>
            struct swap_bytes<wn_float64, 8> {
                static WN_FORCE_INLINE wn_float64 execute(const wn_float64 _value) {
                    wn_float64 value = _value;
                    wn_byte* bytes = reinterpret_cast<wn_byte*>(&value);

                    swap_byte(bytes[0], bytes[7]);
                    swap_byte(bytes[1], bytes[6]);
                    swap_byte(bytes[2], bytes[5]);
                    swap_byte(bytes[3], bytes[4]);

                    return(value);
                }
            };
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type to_big_endian(const _Type _value) {
            #ifdef _WN_ENDIAN_BIG
                return(_value);
            #else
                return(internal::swap_bytes<_Type>::execute(_value));
            #endif
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type from_big_endian(const _Type _value) {
            #ifdef _WN_ENDIAN_BIG
                return(_value);
            #else
                return(internal::swap_bytes<_Type>::execute(_value));
            #endif
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type to_little_endian(const _Type _value) {
            #ifdef _WN_ENDIAN_BIG
                return(internal::swap_bytes<_Type>::execute(_value));
            #else
                return(_value);
            #endif
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type from_little_endian(const _Type _value) {
            #ifdef _WN_ENDIAN_BIG
                return(internal::swap_bytes<_Type>::execute(_value));
            #else
                return(_value);
            #endif
        }
    }
}

#endif // __WN_CORE_ENDIAN_H__
