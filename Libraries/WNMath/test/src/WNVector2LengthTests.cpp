#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, LengthTest) {
    const WNVector2<WN_UINT8> vector = WNVector2<WN_UINT8>(1, 2);
    const WN_UINT8 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationUINT16, LengthTest) {
    const WNVector2<WN_UINT16> vector = WNVector2<WN_UINT16>(1, 2);
    const WN_UINT16 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationUINT32, LengthTest) {
    const WNVector2<WN_UINT32> vector = WNVector2<WN_UINT32>(1, 2);
    const WN_UINT32 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationUINT64, LengthTest) {
    const WNVector2<WN_UINT64> vector = WNVector2<WN_UINT64>(1, 2);
    const WN_UINT64 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationINT8, LengthTest) {
    const WNVector2<WN_INT8> vector = WNVector2<WN_INT8>(1, 2);
    const WN_INT8 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationINT16, LengthTest) {
    const WNVector2<WN_INT16> vector = WNVector2<WN_INT16>(1, 2);
    const WN_INT16 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationINT32, LengthTest) {
    const WNVector2<WN_INT32> vector = WNVector2<WN_INT32>(1, 2);
    const WN_INT32 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationINT64, LengthTest) {
    const WNVector2<WN_INT64> vector = WNVector2<WN_INT64>(1, 2);
    const WN_INT64 length = vector.Length();

    ASSERT_EQ(length, 2);
}

TEST(WNVector2ValidationFLOAT32, LengthTest) {
    const WNVector2<WN_FLOAT32> vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    const WN_FLOAT32 length = vector.Length();

    ASSERT_LT(abs(length - 2.2360680f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, LengthTest) {
    const WNVector2<WN_FLOAT64> vector = WNVector2<WN_FLOAT64>(1.0, 2.0);
    const WN_FLOAT64 length = vector.Length();

    ASSERT_LT(abs(length - 2.2360679774997898), ALLOWED_PERCENTAGE_ERROR);
}
