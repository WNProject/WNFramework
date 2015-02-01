#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix2.h"
#endif

using namespace WNMath;

TEST(WNMatrix2ValidationUINT8, ZeroTest) {
    WNMatrix2<WN_UINT8> vector = WNMatrix2<WN_UINT8>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT16, ZeroTest) {
    WNMatrix2<WN_UINT16> vector = WNMatrix2<WN_UINT16>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT32, ZeroTest) {
    WNMatrix2<WN_UINT32> vector = WNMatrix2<WN_UINT32>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT64, ZeroTest) {
    WNMatrix2<WN_UINT64> vector = WNMatrix2<WN_UINT64>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationINT8, ZeroTest) {
    WNMatrix2<WN_INT8> vector = WNMatrix2<WN_INT8>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationINT16, ZeroTest) {
    WNMatrix2<WN_INT16> vector = WNMatrix2<WN_INT16>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationINT32, ZeroTest) {
    WNMatrix2<WN_INT32> vector = WNMatrix2<WN_INT32>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationINT64, ZeroTest) {
    WNMatrix2<WN_INT64> vector = WNMatrix2<WN_INT64>(1);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNMatrix2ValidationFLOAT16, ZeroTest) {
    WNMatrix2<WN_FLOAT16> vector = WNMatrix2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 0.0f);
    }
}

TEST(WNMatrix2ValidationFLOAT32, ZeroTest) {
    WNMatrix2<WN_FLOAT32> vector = WNMatrix2<WN_FLOAT32>(1.0f);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0.0f);
    }
}

TEST(WNMatrix2ValidationFLOAT64, ZeroTest) {
    WNMatrix2<WN_FLOAT64> vector = WNMatrix2<WN_FLOAT64>(1.0);

    vector.Zero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 0.0);
    }
}
