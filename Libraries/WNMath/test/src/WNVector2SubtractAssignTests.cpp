#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, SubtractAssignTest) {
    WNVector2<WN_UINT8> vector = WNVector2<WN_UINT8>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationUINT16, SubtractAssignTest) {
    WNVector2<WN_UINT16> vector = WNVector2<WN_UINT16>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationUINT32, SubtractAssignTest) {
    WNVector2<WN_UINT32> vector = WNVector2<WN_UINT32>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationUINT64, SubtractAssignTest) {
    WNVector2<WN_UINT64> vector = WNVector2<WN_UINT64>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationINT8, SubtractAssignTest) {
    WNVector2<WN_INT8> vector = WNVector2<WN_INT8>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationINT16, SubtractAssignTest) {
    WNVector2<WN_INT16> vector = WNVector2<WN_INT16>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationINT32, SubtractAssignTest) {
    WNVector2<WN_INT32> vector = WNVector2<WN_INT32>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationINT64, SubtractAssignTest) {
    WNVector2<WN_INT64> vector = WNVector2<WN_INT64>(1, 2);

    vector -= 1;

    ASSERT_EQ(vector.GetX(), 0);
    ASSERT_EQ(vector.GetY(), 1);
}

TEST(WNVector2ValidationFLOAT16, SubtractAssignTest) {
    WNVector2<WN_FLOAT16> vector = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));

    vector -= static_cast<WN_FLOAT16>(1.0f);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(0.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT32, SubtractAssignTest) {
    WNVector2<WN_FLOAT32> vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);

    vector -= 1.0f;

    ASSERT_LT(abs(vector.GetX() - 0.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, SubtractAssignTest) {
    WNVector2<WN_FLOAT64> vector = WNVector2<WN_FLOAT64>(1.0, 2.0);

    vector -= 1.0;

    ASSERT_LT(abs(vector.GetX() - 0.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 1.0), ALLOWED_PERCENTAGE_ERROR);
}
