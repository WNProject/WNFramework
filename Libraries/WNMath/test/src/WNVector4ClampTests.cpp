#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, ClampTest) {
    WNVector4<WN_UINT8> vector1 = WNVector4<WN_UINT8>(50, 50, 50, 50);
    WNVector4<WN_UINT8> vector2 = WNVector4<WN_UINT8>(0, 75, 100, 50);
    WNVector4<WN_UINT8> vector3 = WNVector4<WN_UINT8>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationUINT16, ClampTest) {
    WNVector4<WN_UINT16> vector1 = WNVector4<WN_UINT16>(50, 50, 50, 50);
    WNVector4<WN_UINT16> vector2 = WNVector4<WN_UINT16>(0, 75, 100, 50);
    WNVector4<WN_UINT16> vector3 = WNVector4<WN_UINT16>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationUINT32, ClampTest) {
    WNVector4<WN_UINT32> vector1 = WNVector4<WN_UINT32>(50, 50, 50, 50);
    WNVector4<WN_UINT32> vector2 = WNVector4<WN_UINT32>(0, 75, 100, 50);
    WNVector4<WN_UINT32> vector3 = WNVector4<WN_UINT32>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationUINT64, ClampTest) {
    WNVector4<WN_UINT64> vector1 = WNVector4<WN_UINT64>(50, 50, 50, 50);
    WNVector4<WN_UINT64> vector2 = WNVector4<WN_UINT64>(0, 75, 100, 50);
    WNVector4<WN_UINT64> vector3 = WNVector4<WN_UINT64>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationINT8, ClampTest) {
    WNVector4<WN_INT8> vector1 = WNVector4<WN_INT8>(50, 50, 50, 50);
    WNVector4<WN_INT8> vector2 = WNVector4<WN_INT8>(0, 75, 100, 50);
    WNVector4<WN_INT8> vector3 = WNVector4<WN_INT8>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationINT16, ClampTest) {
    WNVector4<WN_INT16> vector1 = WNVector4<WN_INT16>(50, 50, 50, 50);
    WNVector4<WN_INT16> vector2 = WNVector4<WN_INT16>(0, 75, 100, 50);
    WNVector4<WN_INT16> vector3 = WNVector4<WN_INT16>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationINT32, ClampTest) {
    WNVector4<WN_INT32> vector1 = WNVector4<WN_INT32>(50, 50, 50, 50);
    WNVector4<WN_INT32> vector2 = WNVector4<WN_INT32>(0, 75, 100, 50);
    WNVector4<WN_INT32> vector3 = WNVector4<WN_INT32>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationINT64, ClampTest) {
    WNVector4<WN_INT64> vector1 = WNVector4<WN_INT64>(50, 50, 50, 50);
    WNVector4<WN_INT64> vector2 = WNVector4<WN_INT64>(0, 75, 100, 50);
    WNVector4<WN_INT64> vector3 = WNVector4<WN_INT64>(25, 100, 50, 100);

    vector1.Clamp(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 25);
    ASSERT_EQ(vector1.GetY(), 75);
    ASSERT_EQ(vector1.GetZ(), 50);
    ASSERT_EQ(vector1.GetW(), 50);
}

TEST(WNVector4ValidationFLOAT16, ClampTest) {
    WNVector4<WN_FLOAT16> vector1 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(50.0f), static_cast<WN_FLOAT16>(50.0f),
                                                          static_cast<WN_FLOAT16>(50.0f), static_cast<WN_FLOAT16>(50.0f));
    WNVector4<WN_FLOAT16> vector2 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(0.0f), static_cast<WN_FLOAT16>(75.0f),
                                                          static_cast<WN_FLOAT16>(100.0f), static_cast<WN_FLOAT16>(50.0f));
    WNVector4<WN_FLOAT16> vector3 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(25.0f), static_cast<WN_FLOAT16>(100.0f),
                                                          static_cast<WN_FLOAT16>(50.0f), static_cast<WN_FLOAT16>(100.0f));

    vector1.Clamp(vector2, vector3);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(25.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(75.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetZ() - static_cast<WN_FLOAT16>(50.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetW() - static_cast<WN_FLOAT16>(50.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, ClampTest) {
    WNVector4<WN_FLOAT32> vector1 = WNVector4<WN_FLOAT32>(50.0f, 50.0f, 50.0f, 50.0f);
    WNVector4<WN_FLOAT32> vector2 = WNVector4<WN_FLOAT32>(0.0f, 75.0f, 100.0f, 50.0f);
    WNVector4<WN_FLOAT32> vector3 = WNVector4<WN_FLOAT32>(25.0f, 100.0f, 50.0f, 100.0f);

    vector1.Clamp(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 25.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 75.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 50.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 50.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, ClampTest) {
    WNVector4<WN_FLOAT64> vector1 = WNVector4<WN_FLOAT64>(50.0, 50.0, 50.0, 50.0);
    WNVector4<WN_FLOAT64> vector2 = WNVector4<WN_FLOAT64>(0.0, 75.0, 100.0, 50.0);
    WNVector4<WN_FLOAT64> vector3 = WNVector4<WN_FLOAT64>(25.0, 100.0, 50.0, 100.0);

    vector1.Clamp(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 25.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 75.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 50.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 50.0), ALLOWED_PERCENTAGE_ERROR);
}
