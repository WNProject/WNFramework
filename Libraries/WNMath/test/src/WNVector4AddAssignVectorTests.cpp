#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, AddAssignVectorTest) {
    WNVector4<WN_UINT8> vector1 = WNVector4<WN_UINT8>(1, 2, 3, 4);
    WNVector4<WN_UINT8> vector2 = WNVector4<WN_UINT8>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationUINT16, AddAssignVectorTest) {
    WNVector4<WN_UINT16> vector1 = WNVector4<WN_UINT16>(1, 2, 3, 4);
    WNVector4<WN_UINT16> vector2 = WNVector4<WN_UINT16>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationUINT32, AddAssignVectorTest) {
    WNVector4<WN_UINT32> vector1 = WNVector4<WN_UINT32>(1, 2, 3, 4);
    WNVector4<WN_UINT32> vector2 = WNVector4<WN_UINT32>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationUINT64, AddAssignVectorTest) {
    WNVector4<WN_UINT64> vector1 = WNVector4<WN_UINT64>(1, 2, 3, 4);
    WNVector4<WN_UINT64> vector2 = WNVector4<WN_UINT64>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationINT8, AddAssignVectorTest) {
    WNVector4<WN_INT8> vector1 = WNVector4<WN_INT8>(1, 2, 3, 4);
    WNVector4<WN_INT8> vector2 = WNVector4<WN_INT8>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationINT16, AddAssignVectorTest) {
    WNVector4<WN_INT16> vector1 = WNVector4<WN_INT16>(1, 2, 3, 4);
    WNVector4<WN_INT16> vector2 = WNVector4<WN_INT16>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationINT32, AddAssignVectorTest) {
    WNVector4<WN_INT32> vector1 = WNVector4<WN_INT32>(1, 2, 3, 4);
    WNVector4<WN_INT32> vector2 = WNVector4<WN_INT32>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationINT64, AddAssignVectorTest) {
    WNVector4<WN_INT64> vector1 = WNVector4<WN_INT64>(1, 2, 3, 4);
    WNVector4<WN_INT64> vector2 = WNVector4<WN_INT64>(1, 2, 3, 4);

    vector1 += vector2;

    ASSERT_EQ(vector1.GetX(), 2);
    ASSERT_EQ(vector1.GetY(), 4);
    ASSERT_EQ(vector1.GetZ(), 6);
    ASSERT_EQ(vector1.GetW(), 8);
}

TEST(WNVector4ValidationFLOAT16, AddAssignVectorTest) {
    WNVector4<WN_FLOAT16> vector1 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                          static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));
    WNVector4<WN_FLOAT16> vector2 = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                          static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));

    vector1 += vector2;

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetX() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetY() - static_cast<WN_FLOAT16>(4.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetZ() - static_cast<WN_FLOAT16>(6.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1.GetW() - static_cast<WN_FLOAT16>(8.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, AddAssignVectorTest) {
    WNVector4<WN_FLOAT32> vector1 = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);
    WNVector4<WN_FLOAT32> vector2 = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    vector1 += vector2;

    ASSERT_LT(abs(vector1.GetX() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 4.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 6.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 8.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, AddAssignVectorTest) {
    WNVector4<WN_FLOAT64> vector1 = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);
    WNVector4<WN_FLOAT64> vector2 = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    vector1 += vector2;

    ASSERT_LT(abs(vector1.GetX() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetY() - 4.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetZ() - 6.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector1.GetW() - 8.0), ALLOWED_PERCENTAGE_ERROR);
}