#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__
#define __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__

#define WN_INLINE __inline__

#ifdef _WN_DEBUG
    #define WN_FORCE_INLINE __inline__
#else
    #define WN_FORCE_INLINE __inline__ __attribute__((always_inline))
#endif

#define WN_NO_INLINE __attribute__((noinline))

#define WN_RESTRICT __restrict__

#ifndef __WN_HAS_THREAD_LOCAL
    #define WN_THREAD_LOCAL __thread
#endif

#define WN_ALIGN(_x) __attribute__((aligned (_x)))

#define WN_WEAK_LINK __attribute__((weak))

#if defined __x86_64 || defined __x86_64__ || defined __amd64 || defined __amd64__
    #define _WN_X86
    #define _WN_64_BIT
#elif defined i386 || defined __i386 || defined __i386__
    #define _WN_X86
    #define _WN_32_BIT
#elif defined arm || defined __arm__
    #if defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ || defined __ARM_ARCH_7R__ || \
        defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7EM__ || defined _ARM_ARCH_7
        #define _WN_ARM 7
    #elif defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ || defined __ARM_ARCH_6K__ || \
            defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__ || defined __ARM_ARCH_6T2__ || \
            defined __ARM_ARCH_6M__ || defined _ARM_ARCH_6
        #define _WN_ARM 6
    #elif defined __ARM_ARCH_5__ || defined __ARM_ARCH_5T__ || defined __ARM_ARCH_5E__ || \
            defined __ARM_ARCH_5TE__ || defined __ARM_ARCH_5TEJ__ || defined _ARM_ARCH_5
        #define _WN_ARM 5
    #elif defined __ARM_ARCH_4__ || defined __ARM_ARCH_4T__ || defined _ARM_ARCH_4
        #define _WN_ARM 4
    #endif

    #define _WN_32_BIT
#elif defined mips || defined __mips__
    #define _WN_MIPS
    #define _WN_32_BIT
    #define _WN_ARCH_NAME "mips"
#endif

#if defined __ANDROID || defined ANDROID
    #define _WN_ANDROID
    #define _WN_PLATFORM_NAME "Android"
#elif defined linux || defined __linux || defined __linux__
    #define _WN_LINUX
    #define _WN_PLATFORM_NAME "Linux"

    #include <stdlib.h>

    #ifdef _WN_X86
        #ifndef _WN_64_BIT
            #define WN_OSCALL_BEGIN
            #define WN_OSCALL_END __attribute__((cdecl))
        #endif
    #endif
#endif

#ifdef __BIG_ENDIAN__
    #define _WN_ENDIAN_BIG
#elif defined __LITTLE_ENDIAN__
    #define _WN_ENDIAN_LITTLE
#else
    #define _WN_ENDIAN_UNKNOWN
#endif

#if defined _WN_LINUX || defined _WN_ANDROID
    #define _WN_POSIX
#endif

#define WN_UNUSED_ARGUMENT(_argument) (static_cast<wn_void>(sizeof((_argument), 0)))

#if defined _WN_LINUX
    #include <signal.h>

    #define WN_DEBUG_BREAK() raise(SIGTRAP)
#elif defined _WN_GCC || defined _WN_CLANG
    #ifdef _WN_X86
        #define WN_DEBUG_BREAK() __asm__ __volatile__ ("int $3")
    #endif
#endif

#endif // __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__