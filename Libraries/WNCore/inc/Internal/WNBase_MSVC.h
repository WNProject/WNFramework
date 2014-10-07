////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_MSVC_H__
#define __WN_CORE_INTERNAL_BASE_MSVC_H__

#if _MSC_VER < 1600
    #error "Compiler not supported: MSVC 10 (Visual Studio 2010) or higher must be used"
#endif

#ifdef _MSC_FULL_VER
    #define _WN_MSVC _MSC_FULL_VER
    #define _WN_MSVC_MAJOR (_MSC_VER / 100)
    #define _WN_MSVC_MINOR (_MSC_VER % 100)
    #define _WN_MSVC_BUILD (_MSC_FULL_VER % 100000)
#else
    #define _WN_MSVC _MSC_VER * 100000
    #define _WN_MSVC_MAJOR (_MSC_VER / 100)
    #define _WN_MSVC_MINOR (_MSC_VER % 100)
    #define _WN_MSVC_BUILD 0
#endif

#define WN_INLINE __inline
#define WN_FORCE_INLINE __forceinline
#define WN_NO_INLINE __declspec(noinline)
#define WN_RESTRICT __restrict
#define WN_THREAD_LOCAL __declspec(thread)
#define WN_WEAK_LINK __declspec(selectany)

#define WN_ALIGN(_x) __declspec(align(_x))

#if _MSC_VER >= 1600 || (defined _MSC_EXTENSIONS && defined _NATIVE_NULLPTR_SUPPORTED)
    #define __WN_HAS_NULLPTR
    #define __WN_HAS_NULLPTR_CUSTOM
#endif

#if _MSC_VER >= 1600
    #define __WN_HAS_STATIC_ASSERT
    #define __WN_HAS_RVALUE_REFERENCES
    #define __WN_HAS_TYPE_TRAITS
#endif

#if defined _M_X64 || defined _M_AMD64
    #define _WN_X86
    #define _WN_64_BIT

    // For some reason the following don't get properly defined at editing time just at compile time.  Define them so
    // intellisense doesn't complain
    #ifndef _M_AMD64
        #define _M_AMD64

        #ifndef _AMD64_
            #define _AMD64_
        #endif
    #endif
#elif defined _M_IX86
    #define _WN_X86
    #define _WN_32_BIT
#elif defined _M_ARM
    #define _WN_ARM _M_ARM
    #define _WN_32_BIT
#endif

#if defined _WIN32 || defined _WIN64
    #define _WN_WINDOWS
    #define _WN_PLATFORM_NAME "Windows"

    #include <winsock2.h>
    #include <windows.h>

    #if !defined _WIN32_WINNT_VISTA || _WIN32_WINNT < _WIN32_WINNT_VISTA
        #error "Windows API Version not supported: Windows Vista or higher must be used"
    #endif

    #define WN_OSCALL_BEGIN WINAPI
    #define WN_OSCALL_END
#endif

#endif // __WN_CORE_INTERNAL_BASE_MSVC_H__