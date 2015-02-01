#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, CreateZeroTest) {
    WNVector2<WN_UINT8> vector = WNVector2<WN_UINT8>(1, 2);

    vector = WNVector2<WN_UINT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationUINT16, CreateZeroTest) {
    WNVector2<WN_UINT16> vector = WNVector2<WN_UINT16>(1, 2);

    vector = WNVector2<WN_UINT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationUINT32, CreateZeroTest) {
    WNVector2<WN_UINT32> vector = WNVector2<WN_UINT32>(1, 2);

    vector = WNVector2<WN_UINT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationUINT64, CreateZeroTest) {
    WNVector2<WN_UINT64> vector = WNVector2<WN_UINT64>(1, 2);

    vector = WNVector2<WN_UINT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationINT8, CreateZeroTest) {
    WNVector2<WN_INT8> vector = WNVector2<WN_INT8>(1, 2);

    vector = WNVector2<WN_INT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationINT16, CreateZeroTest) {
    WNVector2<WN_INT16> vector = WNVector2<WN_INT16>(1, 2);

    vector = WNVector2<WN_INT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationINT32, CreateZeroTest) {
    WNVector2<WN_INT32> vector = WNVector2<WN_INT32>(1, 2);

    vector = WNVector2<WN_INT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationINT64, CreateZeroTest) {
    WNVector2<WN_INT64> vector = WNVector2<WN_INT64>(1, 2);

    vector = WNVector2<WN_INT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector2ValidationFLOAT16, CreateZeroTest) {
    WNVector2<WN_FLOAT16> vector = WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f));

    vector = WNVector2<WN_FLOAT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 0.0f);
    }
}

TEST(WNVector2ValidationFLOAT32, CreateZeroTest) {
    WNVector2<WN_FLOAT32> vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);

    vector = WNVector2<WN_FLOAT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0.0f);
    }
}

TEST(WNVector2ValidationFLOAT64, CreateZeroTest) {
    WNVector2<WN_FLOAT64> vector = WNVector2<WN_FLOAT64>(1.0, 2.0);

    vector = WNVector2<WN_FLOAT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        ASSERT_EQ(vector[i], 0.0);
    }
}
