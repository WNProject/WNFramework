#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__
#define __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__

#ifdef _WN_LINUX
    #include <stdlib.h>

    #ifdef _WN_X86
        #ifndef _WN_64_BIT
            #define WN_OSCALL_BEGIN
            #define WN_OSCALL_END __attribute__((cdecl))
        #endif
    #endif
#endif

#if defined _WN_LINUX || defined _WN_ANDROID
    #define _WN_POSIX
#endif

#define WN_UNUSED_ARGUMENT(_argument) (static_cast<void>(sizeof((_argument), 0)))

#if defined _WN_LINUX
    #include <signal.h>

    #define WN_DEBUG_BREAK() raise(SIGTRAP)
#elif defined _WN_GCC || defined _WN_CLANG
    #ifdef _WN_X86
        #define WN_DEBUG_BREAK() __asm__ __volatile__ ("int $3")
    #endif
#endif

#endif // __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__