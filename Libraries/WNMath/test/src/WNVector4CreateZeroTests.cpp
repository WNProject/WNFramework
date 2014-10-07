#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, CreateZeroTest) {
    WNVector4<WN_UINT8> vector = WNVector4<WN_UINT8>(1, 2, 3, 4);

    vector = WNVector4<WN_UINT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationUINT16, CreateZeroTest) {
    WNVector4<WN_UINT16> vector = WNVector4<WN_UINT16>(1, 2, 3, 4);

    vector = WNVector4<WN_UINT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationUINT32, CreateZeroTest) {
    WNVector4<WN_UINT32> vector = WNVector4<WN_UINT32>(1, 2, 3, 4);

    vector = WNVector4<WN_UINT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationUINT64, CreateZeroTest) {
    WNVector4<WN_UINT64> vector = WNVector4<WN_UINT64>(1, 2, 3, 4);

    vector = WNVector4<WN_UINT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationINT8, CreateZeroTest) {
    WNVector4<WN_INT8> vector = WNVector4<WN_INT8>(1, 2, 3, 4);

    vector = WNVector4<WN_INT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationINT16, CreateZeroTest) {
    WNVector4<WN_INT16> vector = WNVector4<WN_INT16>(1, 2, 3, 4);

    vector = WNVector4<WN_INT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationINT32, CreateZeroTest) {
    WNVector4<WN_INT32> vector = WNVector4<WN_INT32>(1, 2, 3, 4);

    vector = WNVector4<WN_INT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationINT64, CreateZeroTest) {
    WNVector4<WN_INT64> vector = WNVector4<WN_INT64>(1, 2, 3, 4);

    vector = WNVector4<WN_INT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVector4ValidationFLOAT16, CreateZeroTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(1.0f, 2.0f, 3.0f, 4.0f);

    vector = WNVector4<WN_FLOAT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 0.0f);
    }
}

TEST(WNVector4ValidationFLOAT32, CreateZeroTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    vector = WNVector4<WN_FLOAT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0.0f);
    }
}

TEST(WNVector4ValidationFLOAT64, CreateZeroTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    vector = WNVector4<WN_FLOAT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0.0);
    }
}
