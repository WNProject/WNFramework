#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, MultiplySubtractVectorTest) {
    WNVector3<WN_UINT8> vector1 = WNVector3<WN_UINT8>(1, 2, 3);
    WNVector3<WN_UINT8> vector2 = WNVector3<WN_UINT8>(1, 2, 3);
    WNVector3<WN_UINT8> vector3 = WNVector3<WN_UINT8>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationUINT16, MultiplySubtractVectorTest) {
    WNVector3<WN_UINT16> vector1 = WNVector3<WN_UINT16>(1, 2, 3);
    WNVector3<WN_UINT16> vector2 = WNVector3<WN_UINT16>(1, 2, 3);
    WNVector3<WN_UINT16> vector3 = WNVector3<WN_UINT16>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationUINT32, MultiplySubtractVectorTest) {
    WNVector3<WN_UINT32> vector1 = WNVector3<WN_UINT32>(1, 2, 3);
    WNVector3<WN_UINT32> vector2 = WNVector3<WN_UINT32>(1, 2, 3);
    WNVector3<WN_UINT32> vector3 = WNVector3<WN_UINT32>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationUINT64, MultiplySubtractVectorTest) {
    WNVector3<WN_UINT64> vector1 = WNVector3<WN_UINT64>(1, 2, 3);
    WNVector3<WN_UINT64> vector2 = WNVector3<WN_UINT64>(1, 2, 3);
    WNVector3<WN_UINT64> vector3 = WNVector3<WN_UINT64>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationINT8, MultiplySubtractVectorTest) {
    WNVector3<WN_INT8> vector1 = WNVector3<WN_INT8>(1, 2, 3);
    WNVector3<WN_INT8> vector2 = WNVector3<WN_INT8>(1, 2, 3);
    WNVector3<WN_INT8> vector3 = WNVector3<WN_INT8>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationINT16, MultiplySubtractVectorTest) {
    WNVector3<WN_INT16> vector1 = WNVector3<WN_INT16>(1, 2, 3);
    WNVector3<WN_INT16> vector2 = WNVector3<WN_INT16>(1, 2, 3);
    WNVector3<WN_INT16> vector3 = WNVector3<WN_INT16>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationINT32, MultiplySubtractVectorTest) {
    WNVector3<WN_INT32> vector1 = WNVector3<WN_INT32>(1, 2, 3);
    WNVector3<WN_INT32> vector2 = WNVector3<WN_INT32>(1, 2, 3);
    WNVector3<WN_INT32> vector3 = WNVector3<WN_INT32>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationINT64, MultiplySubtractVectorTest) {
    WNVector3<WN_INT64> vector1 = WNVector3<WN_INT64>(1, 2, 3);
    WNVector3<WN_INT64> vector2 = WNVector3<WN_INT64>(1, 2, 3);
    WNVector3<WN_INT64> vector3 = WNVector3<WN_INT64>(1, 4, 9);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_EQ(vector1.GetX(), 0);
    ASSERT_EQ(vector1.GetY(), 0);
    ASSERT_EQ(vector1.GetZ(), 0);
}

TEST(WNVector3ValidationFLOAT16, MultiplySubtractVectorTest) {
    WNVector3<WN_FLOAT16> vector1 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                          static_cast<WN_FLOAT16>(3.0f));
    WNVector3<WN_FLOAT16> vector2 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                          static_cast<WN_FLOAT16>(3.0f));
    WNVector3<WN_FLOAT16> vector3 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(4.0f),
                                                          static_cast<WN_FLOAT16>(9.0f));

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(0.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(0.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetZ() - static_cast<WN_FLOAT16>(0.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT32, MultiplySubtractVectorTest) {
    WNVector3<WN_FLOAT32> vector1 = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);
    WNVector3<WN_FLOAT32> vector2 = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);
    WNVector3<WN_FLOAT32> vector3 = WNVector3<WN_FLOAT32>(1.0f, 4.0f, 9.0f);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 0.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 0.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 0.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, MultiplySubtractVectorTest) {
    WNVector3<WN_FLOAT64> vector1 = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);
    WNVector3<WN_FLOAT64> vector2 = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);
    WNVector3<WN_FLOAT64> vector3 = WNVector3<WN_FLOAT64>(1.0, 4.0, 9.0);

    vector1.MultiplySubtract(vector2, vector3);

    ASSERT_LT(abs(vector1.GetX() - 0.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 0.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 0.0), ALLOWED_PERCENTAGE_ERROR);
}
