#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, LengthTest) {
    const WNVector3<WN_UINT8> vector = WNVector3<WN_UINT8>(1, 2, 3);
    const WN_UINT8 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationUINT16, LengthTest) {
    const WNVector3<WN_UINT16> vector = WNVector3<WN_UINT16>(1, 2, 3);
    const WN_UINT16 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationUINT32, LengthTest) {
    const WNVector3<WN_UINT32> vector = WNVector3<WN_UINT32>(1, 2, 3);
    const WN_UINT32 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationUINT64, LengthTest) {
    const WNVector3<WN_UINT64> vector = WNVector3<WN_UINT64>(1, 2, 3);
    const WN_UINT64 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationINT8, LengthTest) {
    const WNVector3<WN_INT8> vector = WNVector3<WN_INT8>(1, 2, 3);
    const WN_INT8 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationINT16, LengthTest) {
    const WNVector3<WN_INT16> vector = WNVector3<WN_INT16>(1, 2, 3);
    const WN_INT16 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationINT32, LengthTest) {
    const WNVector3<WN_INT32> vector = WNVector3<WN_INT32>(1, 2, 3);
    const WN_INT32 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationINT64, LengthTest) {
    const WNVector3<WN_INT64> vector = WNVector3<WN_INT64>(1, 2, 3);
    const WN_INT64 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVector3ValidationFLOAT32, LengthTest) {
    const WNVector3<WN_FLOAT32> vector = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);
    const WN_FLOAT32 length = vector.Length();

    ASSERT_LT(abs(length - 3.7409668f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, LengthTest) {
    const WNVector3<WN_FLOAT64> vector = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);
    const WN_FLOAT64 length = vector.Length();

    ASSERT_LT(abs(length - 3.7416573867739413), ALLOWED_PERCENTAGE_ERROR);
}
