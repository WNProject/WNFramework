// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Types -----------------------------------------------------------------------------------------------------------------------
//      WN_VOID
//      WN_BYTE
//      WN_BOOL
//      WN_CHAR
//      WN_WCHAR
//      WN_INT8
//      WN_INT16
//      WN_INT32
//      WN_INT64
//      WN_UINT8
//      WN_UINT16
//      WN_UINT32
//      WN_UINT64
//      WN_FLOAT32
//      WN_FLOAT64
//      WN_POINTER
//      WN_SIZE_T
//      WN_ATOM_T

// Values ----------------------------------------------------------------------------------------------------------------------
//      WN_TRUE
//      WN_FALSE
//      WN_NULL
//      WN_FIXED8_MAX
//      WN_FIXED8_MIN
//      WN_FIXED16_MAX
//      WN_FIXED16_MIN
//      WN_FIXED32_MAX
//      WN_FIXED32_MIN
//      WN_FIXED64_MAX
//      WN_FIXED64_MIN
//      WN_FLOAT8_EPSILON
//      WN_FLOAT8_MAX
//      WN_FLOAT8_MIN
//      WN_FLOAT16_EPSILON
//      WN_FLOAT16_MAX
//      WN_FLOAT16_MIN
//      WN_FLOAT32_EPSILON
//      WN_FLOAT32_MAX
//      WN_FLOAT32_MIN
//      WN_FLOAT64_EPSILON
//      WN_FLOAT64_MAX
//      WN_FLOAT64_MIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_TYPES_H__
#define __WN_CORE_TYPES_H__

#include "WNCore/inc/WNBase.h"

#ifdef _WN_MSVC
    typedef unsigned __int8 WN_BYTE;

    typedef __int8 WN_INT8;
    typedef __int16 WN_INT16;
    typedef __int32 WN_INT32;
    typedef __int64 WN_INT64;

    typedef unsigned __int8 WN_UINT8;
    typedef unsigned __int16 WN_UINT16;
    typedef unsigned __int32 WN_UINT32;
    typedef unsigned __int64 WN_UINT64;

    #define __WN_HAS_WCHAR_CUSTOM

    typedef __wchar_t WN_WCHAR;

    #if defined _WN_X86 && defined _WN_64_BIT
        #define __WN_HAS_ATOM_T_CUSTOM

        typedef __int64 WN_ATOM_T;
    #endif

    #ifdef __WN_HAS_NULLPTR_CUSTOM
        #define WN_NULL __nullptr
    #endif
#elif defined _WN_GCC || defined _WN_CLANG
    #include <stdint.h>

    typedef uint8_t WN_BYTE;

    typedef int8_t WN_INT8;
    typedef int16_t WN_INT16;
    typedef int32_t WN_INT32;
    typedef int64_t WN_INT64;

    typedef uint8_t WN_UINT8;
    typedef uint16_t WN_UINT16;
    typedef uint32_t WN_UINT32;
    typedef uint64_t WN_UINT64;

    #ifdef __WN_HAS_NULLPTR_CUSTOM
        #define WN_NULL __null
    #endif
#endif

#include <stddef.h>

typedef void WN_VOID;

typedef char WN_CHAR;

typedef bool WN_BOOL;

typedef size_t WN_SIZE_T;

#ifdef _WN_64_BIT 
    typedef long long WN_SIGNED_T;
#else
    typedef long WN_SIGNED_T;
#endif

typedef WN_BYTE* WN_POINTER;

#define WN_TRUE true
#define WN_FALSE false

#include <float.h>

typedef float WN_FLOAT32;
typedef double WN_FLOAT64;

#define WN_FLOAT32_EPSILON FLT_EPSILON
#define WN_FLOAT32_MAX FLT_MAX
#define WN_FLOAT32_MIN FLT_MIN

#define WN_FLOAT64_EPSILON DBL_EPSILON
#define WN_FLOAT64_MAX DBL_MAX
#define WN_FLOAT64_MIN DBL_MIN

#ifndef __WN_HAS_WCHAR_CUSTOM
    typedef wchar_t WN_WCHAR;
#endif

#ifndef __WN_HAS_ATOM_T_CUSTOM
    #ifdef _WN_32_BIT
        typedef long int WN_ATOM_T;
    #elif defined _WN_64_BIT
        typedef long long WN_ATOM_T;
    #endif
#endif

#ifndef WN_NULL
    #if __WN_CPLUSPLUS >= 201103L || defined __WN_HAS_NULLPTR
        #define WN_NULL nullptr
    #else
        const
        class {
        public:
            template<class Type>
            operator Type * () const { return 0; }
            template<class Class, class Type>
            operator Type Class::* () const { return 0; }

        private:
            WN_VOID operator & () const;
        } __WNNullPtr = {};

        #define WN_NULL __WNNullPtr
    #endif
#endif

template <typename Type>
WN_BOOL WNIsValid(Type _value);

template <typename Type>
WN_BOOL WNIsValid(Type* _value);

template <> WN_BOOL WNIsValid(WN_UINT8 _value);
template <> WN_BOOL WNIsValid(WN_UINT16 _value);
template <> WN_BOOL WNIsValid(WN_UINT32 _value);
template <> WN_BOOL WNIsValid(WN_UINT64 _value);
template <> WN_BOOL WNIsValid(WN_FLOAT32 _value);
template <> WN_BOOL WNIsValid(WN_FLOAT64 _value);

#include "WNCore/inc/Internal/WNTypes.inl"

#endif // __WN_CORE_TYPES_H__
