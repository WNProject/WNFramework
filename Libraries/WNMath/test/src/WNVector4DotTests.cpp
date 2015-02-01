#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, DotTest) {
    const WNVector4<WN_UINT8> vector = WNVector4<WN_UINT8>(1, 2, 3, 4);
    const WN_UINT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationUINT16, DotTest) {
    const WNVector4<WN_UINT16> vector = WNVector4<WN_UINT16>(1, 2, 3, 4);
    const WN_UINT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationUINT32, DotTest) {
    const WNVector4<WN_UINT32> vector = WNVector4<WN_UINT32>(1, 2, 3, 4);
    const WN_UINT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationUINT64, DotTest) {
    const WNVector4<WN_UINT64> vector = WNVector4<WN_UINT64>(1, 2, 3, 4);
    const WN_UINT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationINT8, DotTest) {
    const WNVector4<WN_INT8> vector = WNVector4<WN_INT8>(1, 2, 3, 4);
    const WN_INT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationINT16, DotTest) {
    const WNVector4<WN_INT16> vector = WNVector4<WN_INT16>(1, 2, 3, 4);
    const WN_INT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationINT32, DotTest) {
    const WNVector4<WN_INT32> vector = WNVector4<WN_INT32>(1, 2, 3, 4);
    const WN_INT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationINT64, DotTest) {
    const WNVector4<WN_INT64> vector = WNVector4<WN_INT64>(1, 2, 3, 4);
    const WN_INT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 30);
}

TEST(WNVector4ValidationFLOAT16, DotTest) {
    const WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                               static_cast<WN_FLOAT16>(3.0f), static_cast<WN_FLOAT16>(4.0f));
    const WN_FLOAT16 dot = vector.Dot(vector);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(dot - static_cast<WN_FLOAT16>(30.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, DotTest) {
    const WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);
    const WN_FLOAT32 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 30.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, DotTest) {
    const WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);
    const WN_FLOAT64 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 30.0), ALLOWED_PERCENTAGE_ERROR);
}
