#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNMath/inc/WNBasic.h"

    #include <math.h>
    #include <float.h>
#endif

using namespace WNMath;

TEST(WNSqrtFLOAT32, ZeroTest) {
    const WN_FLOAT32 result = WNSqrt(0.0f);

    ASSERT_EQ(result, 0.0f);
}

TEST(WNSqrtFLOAT32, NegativeTest) {
    const WN_FLOAT32 indeterminate = WNSqrt(-1.0f);

    ASSERT_NE(indeterminate, indeterminate);
}

TEST(WNSqrtFLOAT32, BoundsTest) {
    const WN_FLOAT32 customMin = WNSqrt(WN_FLOAT32_MIN);
    const WN_FLOAT32 standardMin = sqrt(FLT_MIN);
    const WN_FLOAT32 percentageErrorMin = abs((customMin - standardMin) / standardMin);

    ASSERT_LT(percentageErrorMin, ALLOWED_PERCENTAGE_ERROR);

    const WN_FLOAT32 customMax = WNSqrt(WN_FLOAT32_MAX);
    const WN_FLOAT32 standardMax = sqrt(FLT_MAX);
    const WN_FLOAT32 percentageErrorMax = abs((customMax - standardMax) / standardMax);

    ASSERT_LT(percentageErrorMax, ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNSqrtFLOAT32, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(i);
        const WN_FLOAT32 custom = WNSqrt(number);
        const WN_FLOAT32 standard = sqrt(number);

        if (i > 0) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / static_cast<WN_FLOAT32>(RAND_MAX);
        const WN_FLOAT32 custom = WNSqrt(number);
        const WN_FLOAT32 standard = sqrt(number);

        if (number > 0.0f) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / (static_cast<WN_FLOAT32>(RAND_MAX) / FLT_MAX);
        const WN_FLOAT32 custom = WNSqrt(number);
        const WN_FLOAT32 standard = sqrt(number);

        if (number > 0.0f) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}

TEST(WNSqrtFLOAT64, ZeroTest) {
    const WN_FLOAT64 result = WNSqrt(0.0);

    ASSERT_EQ(result, 0.0);
}

TEST(WNSqrtFLOAT64, NegativeTest) {
    const WN_FLOAT64 indeterminate = WNSqrt(-1.0);

    ASSERT_NE(indeterminate, indeterminate);
}

TEST(WNSqrtFLOAT64, BoundsTest) {
    const WN_FLOAT64 customMin = WNSqrt(WN_FLOAT64_MIN);
    const WN_FLOAT64 standardMin = sqrt(DBL_MIN);
    const WN_FLOAT64 percentageErrorMin = abs((customMin - standardMin) / standardMin);

    ASSERT_LT(percentageErrorMin, ALLOWED_PERCENTAGE_ERROR);

    const WN_FLOAT64 customMax = WNSqrt(WN_FLOAT64_MAX);
    const WN_FLOAT64 standardMax = sqrt(DBL_MAX);
    const WN_FLOAT64 percentageErrorMax = abs((customMax - standardMax) / standardMax);

    ASSERT_LT(percentageErrorMax, ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNSqrtFLOAT64, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(i);
        const WN_FLOAT64 custom = WNSqrt(number);
        const WN_FLOAT64 standard = sqrt(number);

        if (i > 0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / static_cast<WN_FLOAT64>(RAND_MAX);
        const WN_FLOAT64 custom = WNSqrt(number);
        const WN_FLOAT64 standard = sqrt(number);

        if (number > 0.0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / (static_cast<WN_FLOAT64>(RAND_MAX) / FLT_MAX);
        const WN_FLOAT64 custom = WNSqrt(number);
        const WN_FLOAT64 standard = sqrt(number);

        if (number > 0.0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}
