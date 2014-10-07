#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationFLOAT16, GetNormalizedTest) {
    const WNVector2<WN_FLOAT16> vector1 = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));
    const WNVector2<WN_FLOAT16> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetX() - static_cast<WN_FLOAT16>(0.44750977f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetY() - static_cast<WN_FLOAT16>(0.89501953f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT32, GetNormalizedTest) {
    const WNVector2<WN_FLOAT32> vector1 = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    const WNVector2<WN_FLOAT32> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.44721359f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.89442718f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, GetNormalizedTest) {
    const WNVector2<WN_FLOAT64> vector1 = WNVector2<WN_FLOAT64>(1.0, 2.0);
    const WNVector2<WN_FLOAT64> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.44721359549995793), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.89442719099991586), ALLOWED_PERCENTAGE_ERROR);
}