#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationFLOAT16, RecipricolTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(2.0f));

    vector.Recipricol();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[i] - static_cast<WN_FLOAT16>(0.5f))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVector4ValidationFLOAT32, RecipricolTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(2.0f);

    vector.Recipricol();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_LT(abs(vector[i] - 0.5f), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVector4ValidationFLOAT64, RecipricolTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(2.0);

    vector.Recipricol();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_LT(abs(vector[i] - 0.5), ALLOWED_PERCENTAGE_ERROR);
    }
}