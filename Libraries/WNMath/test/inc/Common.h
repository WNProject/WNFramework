#ifndef __WN_USE_PRECOMPILED_HEADER
    #pragma once
#endif

#ifndef __WN_MATH_TESTS_COMMON_H__
#define __WN_MATH_TESTS_COMMON_H__

#include "WNCore/inc/WNBase.h"

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

#define NUMBER_VALUES 10000
#define ALLOWED_PERCENTAGE_ERROR 0.01
#define VECTOR_X_DIMENSION 10
#define MATRIX_X_ORDER 10
#define MATRIX_X_DIMENSION (MATRIX_X_ORDER * MATRIX_X_ORDER)

#include "WNCore/inc/WNExtendedTypes.h"
#include "WNPlatform/inc/WNTime.h"
#include "WNMath/inc/WNMath.h"

#include <math.h>
#include <float.h>

#endif // __WN_MATH_TESTS_COMMON_H__
