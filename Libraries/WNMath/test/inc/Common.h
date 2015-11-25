#ifndef __WN_USE_PRECOMPILED_HEADER
    #pragma once
#endif

#ifndef __WN_MATH_TESTS_COMMON_H__
#define __WN_MATH_TESTS_COMMON_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/test/inc/TestDefinitions.h"

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
