#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, ArraySubscriptTest) {
    WNVector4<WN_UINT8> vector = WNVector4<WN_UINT8>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationUINT16, ArraySubscriptTest) {
    WNVector4<WN_UINT16> vector = WNVector4<WN_UINT16>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationUINT32, ArraySubscriptTest) {
    WNVector4<WN_UINT32> vector = WNVector4<WN_UINT32>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationUINT64, ArraySubscriptTest) {
    WNVector4<WN_UINT64> vector = WNVector4<WN_UINT64>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationINT8, ArraySubscriptTest) {
    WNVector4<WN_INT8> vector = WNVector4<WN_INT8>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationINT16, ArraySubscriptTest) {
    WNVector4<WN_INT16> vector = WNVector4<WN_INT16>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationINT32, ArraySubscriptTest) {
    WNVector4<WN_INT32> vector = WNVector4<WN_INT32>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationINT64, ArraySubscriptTest) {
    WNVector4<WN_INT64> vector = WNVector4<WN_INT64>(1, 2, 3, 4);

    ASSERT_EQ(vector[0], 1);
    ASSERT_EQ(vector[1], 2);
    ASSERT_EQ(vector[2], 3);
    ASSERT_EQ(vector[3], 4);
}

TEST(WNVector4ValidationFLOAT16, ArraySubscriptTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                         static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[0] - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[1] - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[2] - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[3] - static_cast<WN_FLOAT16>(4.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, ArraySubscriptTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_LT(abs(vector[0] - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[1] - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[2] - 3.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[3] - 4.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, ArraySubscriptTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    ASSERT_LT(abs(vector[0] - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[1] - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[2] - 3.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector[3] - 4.0), ALLOWED_PERCENTAGE_ERROR);
}
