#ifndef __WN_USE_PRECOMPILED_HEADER
    #pragma once
#endif

#ifndef __WN_CORE_TESTS_COMMON_H__
#define __WN_CORE_TESTS_COMMON_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
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
    #include "WNCore/inc/WNCore.h"
#endif


#endif // __WN_CORE_TESTS_COMMON_H__
