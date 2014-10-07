#ifndef __WN_USE_PRECOMPILED_HEADER
    #pragma once
#endif

#ifndef __WN_MEMORY_TESTS_COMMON_H__
#define __WN_MEMORY_TESTS_COMMON_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)

    #if _WN_MSVC_MAJOR > 16
        #define _VARIADIC_MAX 10
    #endif
#endif

#ifdef _HAS_EXCEPTIONS
    #undef _HAS_EXCEPTIONS
#endif

#define _HAS_EXCEPTIONS 0

#define GTEST_HAS_RTTI 0

#include <gtest/gtest.h>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#ifdef __WN_USE_PRECOMPILED_HEADER
    #include "WNMemory/inc/WNMemory.h"
#endif

#endif // __WN_MEMORY_TESTS_COMMON_H__
