// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// Types -----------------------------------------------------------------------------------------------------------------------
//      wn_void
//      wn_byte
//      wn_bool
//      wn_char
//      wn_wchar
//      wn_int8
//      wn_int16
//      wn_int32
//      wn_int64
//      wn_uint8
//      wn_uint16
//      wn_uint32
//      wn_uint64
//      wn_float32
//      wn_float64
//      wn_atom_t
//      wn_size_t
//      wn_signed_t
//      wn_pointer_t
//      wn_nullptr_t

// Values ----------------------------------------------------------------------------------------------------------------------
//      wn_true
//      wn_false
//      wn_nullptr

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_TYPES_H__
#define __WN_CORE_TYPES_H__

#include "WNCore/inc/WNBase.h"

#include <stddef.h>
#include <stdint.h>
#include <float.h>

#ifdef _WN_MSVC
    #define __WN_HAS_CUSTOM_WCHAR

    typedef __wchar_t wn_wchar;

    #if defined _WN_X86 && defined _WN_64_BIT
        #define __WN_HAS_CUSTOM_ATOM_T

        typedef __int64 wn_atom_t;
    #endif

    #ifdef __WN_HAS_CUSTOM_NULLPTR
        #define wn_nullptr __nullptr
    #endif
#elif defined _WN_GCC || defined _WN_CLANG
    #ifdef __WN_HAS_CUSTOM_NULLPTR
        #define wn_nullptr __null
    #endif
#endif

typedef void wn_void;
typedef char wn_char;

#ifndef __WN_HAS_CUSTOM_WCHAR
    typedef wchar_t wn_wchar;
#endif

typedef bool wn_bool;
typedef int8_t wn_int8;
typedef int16_t wn_int16;
typedef int32_t wn_int32;
typedef int64_t wn_int64;
typedef uint8_t wn_uint8;
typedef uint16_t wn_uint16;
typedef uint32_t wn_uint32;
typedef uint64_t wn_uint64;
typedef float wn_float32;
typedef double wn_float64;
typedef wn_uint8 wn_byte;

typedef wn_byte* wn_pointer_t;
typedef size_t wn_size_t;

#ifdef _WN_64_BIT
    typedef wn_int64 wn_signed_t;

    #ifndef __WN_HAS_CUSTOM_ATOM_T
        typedef long long wn_atom_t;
    #endif
#else
    typedef wn_int32 wn_signed_t;

    #ifndef __WN_HAS_CUSTOM_ATOM_T
        typedef long int wn_atom_t;
    #endif
#endif

#define wn_true true
#define wn_false false

#ifndef wn_nullptr
    #if __WN_CPP >= 201103L || defined __WN_HAS_CPP11_NULLPTR
        #define wn_nullptr nullptr
    #else
        namespace wn {
            namespace core {
                namespace internal {
                    const class{
                    public:
                        template <typename _Type>
                        WN_FORCE_INLINE operator _Type* () const {
                            return(0);
                        }

                        template <typename _ClassType, typename _Type>
                        WN_FORCE_INLINE operator _Type _ClassType::* () const {
                            return(0);
                        }

                        wn_void operator & () const = delete;
                    } null_pointer = {};
                }
            }
        }

        #define wn_nullptr wn::core::internal::null_pointer
    #endif
#endif

typedef decltype(wn_nullptr) wn_nullptr_t;

#endif // __WN_CORE_TYPES_H__
