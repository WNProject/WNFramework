#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, DotTest) {
    const WNVector2<WN_UINT8> vector = WNVector2<WN_UINT8>(1, 2);
    const WN_UINT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationUINT16, DotTest) {
    const WNVector2<WN_UINT16> vector = WNVector2<WN_UINT16>(1, 2);
    const WN_UINT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationUINT32, DotTest) {
    const WNVector2<WN_UINT32> vector = WNVector2<WN_UINT32>(1, 2);
    const WN_UINT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationUINT64, DotTest) {
    const WNVector2<WN_UINT64> vector = WNVector2<WN_UINT64>(1, 2);
    const WN_UINT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationINT8, DotTest) {
    const WNVector2<WN_INT8> vector = WNVector2<WN_INT8>(1, 2);
    const WN_INT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationINT16, DotTest) {
    const WNVector2<WN_INT16> vector = WNVector2<WN_INT16>(1, 2);
    const WN_INT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationINT32, DotTest) {
    const WNVector2<WN_INT32> vector = WNVector2<WN_INT32>(1, 2);
    const WN_INT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationINT64, DotTest) {
    const WNVector2<WN_INT64> vector = WNVector2<WN_INT64>(1, 2);
    const WN_INT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 5);
}

TEST(WNVector2ValidationFLOAT32, DotTest) {
    const WNVector2<WN_FLOAT32> vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    const WN_FLOAT32 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 5.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, DotTest) {
    const WNVector2<WN_FLOAT64> vector = WNVector2<WN_FLOAT64>(1.0, 2.0);
    const WN_FLOAT64 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 5.0), ALLOWED_PERCENTAGE_ERROR);
}
