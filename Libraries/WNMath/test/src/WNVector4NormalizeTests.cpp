#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationFLOAT16, NormalizeTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(1.0f, 2.0f, 3.0f, 4.0f);

    vector.Normalize();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(0.18257418f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(0.36514837f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetZ() - static_cast<WN_FLOAT16>(0.54772258f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetW() - static_cast<WN_FLOAT16>(0.73029673f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, NormalizeTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    vector.Normalize();

    ASSERT_LT(abs(vector.GetX() - 0.18242148f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 0.36484295f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 0.54726446f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetW() - 0.72968590f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, NormalizeTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    vector.Normalize();

    ASSERT_LT(abs(vector.GetX() - 0.18257418583505536), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 0.36514837167011072), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 0.54772255750516607), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetW() - 0.73029674334022143), ALLOWED_PERCENTAGE_ERROR);
}