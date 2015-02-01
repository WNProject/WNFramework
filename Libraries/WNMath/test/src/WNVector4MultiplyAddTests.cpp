#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, MultiplyAddTest) {
    WNVector4<WN_UINT8> vector = WNVector4<WN_UINT8>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationUINT16, MultiplyAddTest) {
    WNVector4<WN_UINT16> vector = WNVector4<WN_UINT16>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationUINT32, MultiplyAddTest) {
    WNVector4<WN_UINT32> vector = WNVector4<WN_UINT32>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationUINT64, MultiplyAddTest) {
    WNVector4<WN_UINT64> vector = WNVector4<WN_UINT64>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationINT8, MultiplyAddTest) {
    WNVector4<WN_INT8> vector = WNVector4<WN_INT8>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationINT16, MultiplyAddTest) {
    WNVector4<WN_INT16> vector = WNVector4<WN_INT16>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationINT32, MultiplyAddTest) {
    WNVector4<WN_INT32> vector = WNVector4<WN_INT32>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationINT64, MultiplyAddTest) {
    WNVector4<WN_INT64> vector = WNVector4<WN_INT64>(1, 2, 3, 4);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
    ASSERT_EQ(vector.GetW(), 11);
}

TEST(WNVector4ValidationFLOAT16, MultiplyAddTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                         static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));

    vector.MultiplyAdd(static_cast<WN_FLOAT16>(2.0f), static_cast<WN_FLOAT16>(3.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(5.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(7.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetZ() - static_cast<WN_FLOAT16>(9.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetW() - static_cast<WN_FLOAT16>(11.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, MultiplyAddTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    vector.MultiplyAdd(2.0f, 3.0f);

    ASSERT_LT(abs(vector.GetX() - 5.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 7.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 9.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetW() - 11.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, MultiplyAddTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    vector.MultiplyAdd(2.0, 3.0);

    ASSERT_LT(abs(vector.GetX() - 5.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 7.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 9.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetW() - 11.0), ALLOWED_PERCENTAGE_ERROR);
}
