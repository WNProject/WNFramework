#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNMath/inc/WNBasic.h"

    #include <math.h>
    #include <float.h>
#endif

using namespace WNMath;

TEST(WNInvSqrtFLOAT32, ZeroTest) {
    const WN_FLOAT32 custom = WNInvSqrt(0.0f);
    const WN_FLOAT32 standard = 1.0f / sqrt(0.0f);

    ASSERT_EQ(custom, standard);
}

TEST(WNInvSqrtFLOAT32, NegativeTest) {
    const WN_FLOAT32 indeterminate = WNInvSqrt(-1.0f);

    ASSERT_NE(indeterminate, indeterminate);
}

TEST(WNInvSqrtFLOAT32, BoundsTest) {
    const WN_FLOAT32 customMin = WNInvSqrt(WN_FLOAT32_MIN);
    const WN_FLOAT32 standardMin = 1.0f / sqrt(FLT_MIN);
    const WN_FLOAT32 percentageErrorMin = abs((customMin - standardMin) / standardMin);

    ASSERT_LT(percentageErrorMin, ALLOWED_PERCENTAGE_ERROR);

    const WN_FLOAT32 customMax = WNInvSqrt(WN_FLOAT32_MAX);
    const WN_FLOAT32 standardMax = 1.0f / sqrt(FLT_MAX);
    const WN_FLOAT32 percentageErrorMax = abs((customMax - standardMax) / standardMax);

    ASSERT_LT(percentageErrorMax, ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNInvSqrtFLOAT32, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(i);
        const WN_FLOAT32 custom = WNInvSqrt(number);
        const WN_FLOAT32 standard = 1.0f / sqrt(number);

        if (i > 0) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / static_cast<WN_FLOAT32>(RAND_MAX);
        const WN_FLOAT32 custom = WNInvSqrt(number);
        const WN_FLOAT32 standard = 1.0f / sqrt(number);

        if (number > 0.0f) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / (static_cast<WN_FLOAT32>(RAND_MAX) / FLT_MAX);
        const WN_FLOAT32 custom = WNInvSqrt(number);
        const WN_FLOAT32 standard = 1.0f / sqrt(number);

        if (number > 0.0f) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}

TEST(WNInvSqrtFLOAT64, ZeroTest) {
    const WN_FLOAT64 custom = WNInvSqrt(0.0);
    const WN_FLOAT64 standard = 1.0 / sqrt(0.0);

    ASSERT_EQ(custom, standard);
}

TEST(WNInvSqrtFLOAT64, NegativeTest) {
    const WN_FLOAT64 indeterminate = WNInvSqrt(-1.0);

    ASSERT_NE(indeterminate, indeterminate);
}

TEST(WNInvSqrtFLOAT64, BoundsTest) {
    const WN_FLOAT64 customMin = WNInvSqrt(WN_FLOAT64_MIN);
    const WN_FLOAT64 standardMin = 1.0 / sqrt(DBL_MIN);
    const WN_FLOAT64 percentageErrorMin = abs((customMin - standardMin) / standardMin);

    ASSERT_LT(percentageErrorMin, ALLOWED_PERCENTAGE_ERROR);

    const WN_FLOAT64 customMax = WNInvSqrt(WN_FLOAT64_MAX);
    const WN_FLOAT64 standardMax = 1.0 / sqrt(DBL_MAX);
    const WN_FLOAT64 percentageErrorMax = abs((customMax - standardMax) / standardMax);

    ASSERT_LT(percentageErrorMax, ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNInvSqrtFLOAT64, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(i);
        const WN_FLOAT64 custom = WNInvSqrt(number);
        const WN_FLOAT64 standard = 1.0 / sqrt(number);

        if (i > 0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / static_cast<WN_FLOAT64>(RAND_MAX);
        const WN_FLOAT64 custom = WNInvSqrt(number);
        const WN_FLOAT64 standard = 1.0 / sqrt(number);

        if (number > 0.0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / (static_cast<WN_FLOAT64>(RAND_MAX) / FLT_MAX);
        const WN_FLOAT64 custom = WNInvSqrt(number);
        const WN_FLOAT64 standard = 1.0 / sqrt(number);

        if (number > 0.0) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}