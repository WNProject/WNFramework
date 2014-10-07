#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationFLOAT16, GetNormalizedTest) {
    const WNVector3<WN_FLOAT16> vector1 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                                static_cast<WN_FLOAT16>(3.0f));
    const WNVector3<WN_FLOAT16> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetX() - static_cast<WN_FLOAT16>(0.26733398f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetY() - static_cast<WN_FLOAT16>(0.53466797f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetZ() - static_cast<WN_FLOAT16>(0.80175781f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT32, GetNormalizedTest) {
    const WNVector3<WN_FLOAT32> vector1 = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);
    const WNVector3<WN_FLOAT32> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.26726124f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.53452247f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 0.80178368f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, GetNormalizedTest) {
    const WNVector3<WN_FLOAT64> vector1 = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);
    const WNVector3<WN_FLOAT64> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.26726124191242440), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.53452248382484879), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 0.80178372573727319), ALLOWED_PERCENTAGE_ERROR);
}