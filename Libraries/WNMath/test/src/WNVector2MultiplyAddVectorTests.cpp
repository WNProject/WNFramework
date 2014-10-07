#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, MultiplyAddVectorTest) {
    WNVector2<WN_UINT8> vector1 = WNVector2<WN_UINT8>(1, 2);
    WNVector2<WN_UINT8> vector2 = WNVector2<WN_UINT8>(1, 2);
    WNVector2<WN_UINT8> vector3 = WNVector2<WN_UINT8>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationUINT16, MultiplyAddVectorTest) {
    WNVector2<WN_UINT16> vector1 = WNVector2<WN_UINT16>(1, 2);
    WNVector2<WN_UINT16> vector2 = WNVector2<WN_UINT16>(1, 2);
    WNVector2<WN_UINT16> vector3 = WNVector2<WN_UINT16>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationUINT32, MultiplyAddVectorTest) {
    WNVector2<WN_UINT32> vector1 = WNVector2<WN_UINT32>(1, 2);
    WNVector2<WN_UINT32> vector2 = WNVector2<WN_UINT32>(1, 2);
    WNVector2<WN_UINT32> vector3 = WNVector2<WN_UINT32>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationUINT64, MultiplyAddVectorTest) {
    WNVector2<WN_UINT64> vector1 = WNVector2<WN_UINT64>(1, 2);
    WNVector2<WN_UINT64> vector2 = WNVector2<WN_UINT64>(1, 2);
    WNVector2<WN_UINT64> vector3 = WNVector2<WN_UINT64>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationINT8, MultiplyAddVectorTest) {
    WNVector2<WN_INT8> vector1 = WNVector2<WN_INT8>(1, 2);
    WNVector2<WN_INT8> vector2 = WNVector2<WN_INT8>(1, 2);
    WNVector2<WN_INT8> vector3 = WNVector2<WN_INT8>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationINT16, MultiplyAddVectorTest) {
    WNVector2<WN_INT16> vector1 = WNVector2<WN_INT16>(1, 2);
    WNVector2<WN_INT16> vector2 = WNVector2<WN_INT16>(1, 2);
    WNVector2<WN_INT16> vector3 = WNVector2<WN_INT16>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationINT32, MultiplyAddVectorTest) {
    WNVector2<WN_INT32> vector1 = WNVector2<WN_INT32>(1, 2);
    WNVector2<WN_INT32> vector2 = WNVector2<WN_INT32>(1, 2);
    WNVector2<WN_INT32> vector3 = WNVector2<WN_INT32>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationINT64, MultiplyAddVectorTest) {
    WNVector2<WN_INT64> vector1 = WNVector2<WN_INT64>(1, 2);
    WNVector2<WN_INT64> vector2 = WNVector2<WN_INT64>(1, 2);
    WNVector2<WN_INT64> vector3 = WNVector2<WN_INT64>(1, 2);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 6);
}

TEST(WNVector2ValidationFLOAT16, MultiplyAddVectorTest) {
    WNVector2<WN_FLOAT16> vector1 = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));
    WNVector2<WN_FLOAT16> vector2 = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));
    WNVector2<WN_FLOAT16> vector3 = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(6.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT32, MultiplyAddVectorTest) {
    WNVector2<WN_FLOAT32> vector1 = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    WNVector2<WN_FLOAT32> vector2 = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    WNVector2<WN_FLOAT32> vector3 = WNVector2<WN_FLOAT32>(1.0f, 2.0f);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 6.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, MultiplyAddVectorTest) {
    WNVector2<WN_FLOAT64> vector1 = WNVector2<WN_FLOAT64>(1.0, 2.0);
    WNVector2<WN_FLOAT64> vector2 = WNVector2<WN_FLOAT64>(1.0, 2.0);
    WNVector2<WN_FLOAT64> vector3 = WNVector2<WN_FLOAT64>(1.0, 2.0);

    vector1.MultiplyAdd(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 6.0), ALLOWED_PERCENTAGE_ERROR);
}
