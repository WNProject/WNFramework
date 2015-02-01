#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, MultiplyAddTest) {
    WNVector3<WN_UINT8> vector = WNVector3<WN_UINT8>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationUINT16, MultiplyAddTest) {
    WNVector3<WN_UINT16> vector = WNVector3<WN_UINT16>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationUINT32, MultiplyAddTest) {
    WNVector3<WN_UINT32> vector = WNVector3<WN_UINT32>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationUINT64, MultiplyAddTest) {
    WNVector3<WN_UINT64> vector = WNVector3<WN_UINT64>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationINT8, MultiplyAddTest) {
    WNVector3<WN_INT8> vector = WNVector3<WN_INT8>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationINT16, MultiplyAddTest) {
    WNVector3<WN_INT16> vector = WNVector3<WN_INT16>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationINT32, MultiplyAddTest) {
    WNVector3<WN_INT32> vector = WNVector3<WN_INT32>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationINT64, MultiplyAddTest) {
    WNVector3<WN_INT64> vector = WNVector3<WN_INT64>(1, 2, 3);

    vector.MultiplyAdd(2, 3);

    ASSERT_EQ(vector.GetX(), 5);
    ASSERT_EQ(vector.GetY(), 7);
    ASSERT_EQ(vector.GetZ(), 9);
}

TEST(WNVector3ValidationFLOAT16, MultiplyAddTest) {
    WNVector3<WN_FLOAT16> vector = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                         static_cast<WN_FLOAT16>(3.0f));

    vector.MultiplyAdd(static_cast<WN_FLOAT16>(2.0f), static_cast<WN_FLOAT16>(3.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(5.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(7.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetZ() - static_cast<WN_FLOAT16>(9.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT32, MultiplyAddTest) {
    WNVector3<WN_FLOAT32> vector = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);

    vector.MultiplyAdd(2.0f, 3.0f);

    ASSERT_LT(abs(vector.GetX() - 5.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 7.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 9.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, MultiplyAddTest) {
    WNVector3<WN_FLOAT64> vector = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);

    vector.MultiplyAdd(2.0, 3.0);

    ASSERT_LT(abs(vector.GetX() - 5.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 7.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 9.0), ALLOWED_PERCENTAGE_ERROR);
}
