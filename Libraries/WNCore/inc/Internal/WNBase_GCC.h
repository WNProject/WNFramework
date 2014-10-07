////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_GCC_H__
#define __WN_CORE_INTERNAL_BASE_GCC_H__

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 4)
    #error "Compiler not supported: GNU C 4.4.0 or higher must be used"
#endif

#ifdef __GNUC_PATCHLEVEL__
    #define _WN_GCC (__GNUC__ * 100000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #define _WN_GCC_MAJOR __GNUC__
    #define _WN_GCC_MINOR __GNUC_MINOR__
    #define _WN_GCC_BUILD __GNUC_PATCHLEVEL__
#else
    #define _WN_GCC (__GNUC__ * 100000 + __GNUC_MINOR__ * 100)
    #define _WN_GCC_MAJOR __GNUC__
    #define _WN_GCC_MINOR __GNUC_MINOR__
    #define _WN_GCC_BUILD 0
#endif

#if defined __GXX_EXPERIMENTAL_CXX0X__ || __cplusplus >= 201100L
    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
        #define __WN_HAS_STATIC_ASSERT
        #define __WN_HAS_RVALUE_REFERENCES
        #define __WN_HAS_VARIADIC_TEMPLATES
    #endif

    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
        #define __WN_HAS_NULLPTR
    #endif

    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
        #define __WN_HAS_THREAD_LOCAL
    #endif
#endif

#ifdef __GNUG__
    #define __WN_HAS_NULLPTR_CUSTOM
#endif

#include "WNCore/inc/Internal/WNBase_Clang_GCC.h"

#endif // __WN_CORE_INTERNAL_BASE_GCC_H__