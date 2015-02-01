#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNMath/inc/WNBasic.h"

    #include <math.h>
    #include <float.h>
#endif

using namespace WNMath;

TEST(WNFloorFLOAT32, ZeroTest) {
    const WN_FLOAT32 result = WNFloor(0.0f);

    ASSERT_EQ(result, 0.0f);
}

TEST(WNFloorFLOAT32, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(i);
        const WN_FLOAT32 custom = WNFloor(number);
        const WN_FLOAT32 standard = floor(number);

        if (i != 0) {
            if (custom != standard) {
                const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

                ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
            }
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / static_cast<WN_FLOAT32>(RAND_MAX);
        const WN_FLOAT32 custom = WNFloor(number);
        const WN_FLOAT32 standard = floor(number);

        if (custom != standard) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT32 number = static_cast<WN_FLOAT32>(rand()) / (static_cast<WN_FLOAT32>(RAND_MAX) / FLT_MAX);
        const WN_FLOAT32 custom = WNFloor(number);
        const WN_FLOAT32 standard = floor(number);

        if (custom != standard) {
            const WN_FLOAT32 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}

TEST(WNFloorFLOAT64, ZeroTest) {
    const WN_FLOAT64 result = WNFloor(0.0);

    ASSERT_EQ(result, 0.0);
}

TEST(WNFloorFLOAT64, ValueTest) {
    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(i);
        const WN_FLOAT64 custom = WNFloor(number);
        const WN_FLOAT64 standard = floor(number);

        if (i != 0) {
            if (custom != standard) {
                const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

                ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
            }
        } else {
            ASSERT_EQ(custom, standard);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / static_cast<WN_FLOAT64>(RAND_MAX);
        const WN_FLOAT64 custom = WNFloor(number);
        const WN_FLOAT64 standard = floor(number);

        if (custom != standard) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        }
    }

    for (WN_UINT32 i = 0; i < NUMBER_VALUES; ++i) {
        const WN_FLOAT64 number = static_cast<WN_FLOAT64>(rand()) / (static_cast<WN_FLOAT64>(RAND_MAX) / DBL_MAX);
        const WN_FLOAT64 custom = WNFloor(number);
        const WN_FLOAT64 standard = floor(number);

        if (custom != standard) {
            const WN_FLOAT64 percentageError = abs((custom - standard) / standard);

            ASSERT_LT(percentageError, ALLOWED_PERCENTAGE_ERROR);
        } else {
            ASSERT_EQ(custom, standard);
        }
    }
}
