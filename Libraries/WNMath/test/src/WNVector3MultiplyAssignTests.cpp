#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, MultiplyAssignTest) {
    WNVector3<WN_UINT8> vector = WNVector3<WN_UINT8>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationUINT16, MultiplyAssignTest) {
    WNVector3<WN_UINT16> vector = WNVector3<WN_UINT16>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationUINT32, MultiplyAssignTest) {
    WNVector3<WN_UINT32> vector = WNVector3<WN_UINT32>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationUINT64, MultiplyAssignTest) {
    WNVector3<WN_UINT64> vector = WNVector3<WN_UINT64>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationINT8, MultiplyAssignTest) {
    WNVector3<WN_INT8> vector = WNVector3<WN_INT8>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationINT16, MultiplyAssignTest) {
    WNVector3<WN_INT16> vector = WNVector3<WN_INT16>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationINT32, MultiplyAssignTest) {
    WNVector3<WN_INT32> vector = WNVector3<WN_INT32>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationINT64, MultiplyAssignTest) {
    WNVector3<WN_INT64> vector = WNVector3<WN_INT64>(1, 2, 3);

    vector *= 1;

    ASSERT_EQ(vector.GetX(), 1);
    ASSERT_EQ(vector.GetY(), 2);
    ASSERT_EQ(vector.GetZ(), 3);
}

TEST(WNVector3ValidationFLOAT16, MultiplyAssignTest) {
    WNVector3<WN_FLOAT16> vector = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                         static_cast<WN_FLOAT16>(3.0f));

    vector *= static_cast<WN_FLOAT16>(1.0f);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT32, MultiplyAssignTest) {
    WNVector3<WN_FLOAT32> vector = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);

    vector *= 1.0f;

    ASSERT_LT(abs(vector.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, MultiplyAssignTest) {
    WNVector3<WN_FLOAT64> vector = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);

    vector *= 1.0;

    ASSERT_LT(abs(vector.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);
}
