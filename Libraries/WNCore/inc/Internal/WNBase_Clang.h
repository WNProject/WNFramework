////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_CLANG_H__
#define __WN_CORE_INTERNAL_BASE_CLANG_H__

#if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 0)
    #error "Compiler not supported: Clang 3.1 or higher must be used"
#endif

#ifdef __clang_patchlevel__
    #define _WN_CLANG (__clang_major__ * 100000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #define _WN_CLANG_MAJOR __clang_major__
    #define _WN_CLANG_MINOR __clang_minor__
    #define _WN_CLANG_BUILD __clang_patchlevel__
#else
    #define _WN_CLANG (__clang_major__ * 100000 + __clang_minor__ * 100)
    #define _WN_CLANG_MAJOR __clang_major__
    #define _WN_CLANG_MINOR __clang_minor__
    #define _WN_CLANG_BUILD 0
#endif

#ifndef __has_feature
    #define __has_feature(_x) 0
#endif

#ifndef __has_extension
    #define __has_extension __has_feature
#endif

#if __has_feature(cxx_nullptr) || __has_extension(cxx_nullptr)
    #define __WN_HAS_NULLPTR
#endif

#if __has_feature(cxx_rvalue_references) || __has_extension(cxx_rvalue_references)
    #define __WN_HAS_RVALUE_REFERENCES
#endif

#if __has_feature(cxx_static_assert) || __has_extension(cxx_static_assert)
    #define __WN_HAS_STATIC_ASSERT
#endif

#if __has_feature(cxx_variadic_templates) || __has_extension(cxx_variadic_templates)
    #define __WN_HAS_VARIADIC_TEMPLATES
#endif

// Work around a header bug with CLANG/ see http://lists.cs.uiuc.edu/pipermail/cfe-dev/2011-February/013207.html
namespace std { class type_info; }

#include "WNCore/inc/Internal/WNBase_Clang_GCC.h"

#endif // __WN_CORE_INTERNAL_BASE_CLANG_H__