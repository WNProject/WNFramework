#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationFLOAT16, NormalizeTest) {
    WNVector2<WN_FLOAT16> vector = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));

    vector.Normalize();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(0.44750977f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(0.89501953f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT32, NormalizeTest) {
    WNVector2<WN_FLOAT32> vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);

    vector.Normalize();

    ASSERT_LT(abs(vector.GetX() - 0.44721359f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 0.89442718f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, NormalizeTest) {
    WNVector2<WN_FLOAT64> vector = WNVector2<WN_FLOAT64>(1.0, 2.0);

    vector.Normalize();

    ASSERT_LT(abs(vector.GetX() - 0.44721359549995793), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 0.89442719099991586), ALLOWED_PERCENTAGE_ERROR);
}