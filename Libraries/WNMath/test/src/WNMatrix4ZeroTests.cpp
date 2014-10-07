#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix4.h"
#endif

using namespace WNMath;

TEST(WNMatrix4ValidationUINT8, ZeroTest) {
    WNMatrix4<WN_UINT8> vector = WNMatrix4<WN_UINT8>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationUINT16, ZeroTest) {
    WNMatrix4<WN_UINT16> vector = WNMatrix4<WN_UINT16>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationUINT32, ZeroTest) {
    WNMatrix4<WN_UINT32> vector = WNMatrix4<WN_UINT32>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationUINT64, ZeroTest) {
    WNMatrix4<WN_UINT64> vector = WNMatrix4<WN_UINT64>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationINT8, ZeroTest) {
    WNMatrix4<WN_INT8> vector = WNMatrix4<WN_INT8>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationINT16, ZeroTest) {
    WNMatrix4<WN_INT16> vector = WNMatrix4<WN_INT16>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationINT32, ZeroTest) {
    WNMatrix4<WN_INT32> vector = WNMatrix4<WN_INT32>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationINT64, ZeroTest) {
    WNMatrix4<WN_INT64> vector = WNMatrix4<WN_INT64>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix4ValidationFLOAT16, ZeroTest) {
    WNMatrix4<WN_FLOAT16> vector = WNMatrix4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 0.0f);
    }
}

TEST(WNMatrix4ValidationFLOAT32, ZeroTest) {
    WNMatrix4<WN_FLOAT32> vector = WNMatrix4<WN_FLOAT32>(1.0f);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0.0f);
    }
}

TEST(WNMatrix4ValidationFLOAT64, ZeroTest) {
    WNMatrix4<WN_FLOAT64> vector = WNMatrix4<WN_FLOAT64>(1.0);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 16; ++i) {
        ASSERT_EQ(vector[i], 0.0);
    }
}
