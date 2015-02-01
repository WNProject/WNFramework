#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, OneTest) {
    WNVector4<WN_UINT8> vector = WNVector4<WN_UINT8>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationUINT16, OneTest) {
    WNVector4<WN_UINT16> vector = WNVector4<WN_UINT16>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationUINT32, OneTest) {
    WNVector4<WN_UINT32> vector = WNVector4<WN_UINT32>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationUINT64, OneTest) {
    WNVector4<WN_UINT64> vector = WNVector4<WN_UINT64>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationINT8, OneTest) {
    WNVector4<WN_INT8> vector = WNVector4<WN_INT8>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationINT16, OneTest) {
    WNVector4<WN_INT16> vector = WNVector4<WN_INT16>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationINT32, OneTest) {
    WNVector4<WN_INT32> vector = WNVector4<WN_INT32>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationINT64, OneTest) {
    WNVector4<WN_INT64> vector = WNVector4<WN_INT64>(1, 2, 3, 4);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector4ValidationFLOAT16, OneTest) {
    WNVector4<WN_FLOAT16> vector = WNVector4<WN_FLOAT16>(1.0f, 2.0f, 3.0f, 4.0f);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 1.0f);
    }
}

TEST(WNVector4ValidationFLOAT32, OneTest) {
    WNVector4<WN_FLOAT32> vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1.0f);
    }
}

TEST(WNVector4ValidationFLOAT64, OneTest) {
    WNVector4<WN_FLOAT64> vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);

    vector.One();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(vector[i], 1.0);
    }
}
