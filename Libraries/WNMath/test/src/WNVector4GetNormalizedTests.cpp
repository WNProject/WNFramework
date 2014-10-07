#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationFLOAT16, GetNormalizedTest) {
    const WNVector4<WN_FLOAT16> vector1 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                                static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));
    const WNVector4<WN_FLOAT16> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetW() - static_cast<WN_FLOAT16>(4.0f))), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetX() - static_cast<WN_FLOAT16>(0.18257418f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetY() - static_cast<WN_FLOAT16>(0.36514837f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetZ() - static_cast<WN_FLOAT16>(0.54772258f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetW() - static_cast<WN_FLOAT16>(0.73029673f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, GetNormalizedTest) {
    const WNVector4<WN_FLOAT32> vector1 = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);
    const WNVector4<WN_FLOAT32> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 4.0f), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.18242148f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.36484295f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 0.54726446f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetW() - 0.72968590f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, GetNormalizedTest) {
    const WNVector4<WN_FLOAT64> vector1 = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);
    const WNVector4<WN_FLOAT64> vector2 = vector1.GetNormalized();

    ASSERT_LT(abs(vector1.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 4.0), ALLOWED_PERCENTAGE_ERROR);

    ASSERT_LT(abs(vector2.GetX() - 0.18257418583505536), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 0.36514837167011072), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 0.54772255750516607), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetW() - 0.73029674334022143), ALLOWED_PERCENTAGE_ERROR);
}