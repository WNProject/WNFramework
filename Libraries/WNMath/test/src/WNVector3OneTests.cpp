#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, OneTest) {
    WNVector3<WN_UINT8> vector = WNVector3<WN_UINT8>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationUINT16, OneTest) {
    WNVector3<WN_UINT16> vector = WNVector3<WN_UINT16>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationUINT32, OneTest) {
    WNVector3<WN_UINT32> vector = WNVector3<WN_UINT32>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationUINT64, OneTest) {
    WNVector3<WN_UINT64> vector = WNVector3<WN_UINT64>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationINT8, OneTest) {
    WNVector3<WN_INT8> vector = WNVector3<WN_INT8>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationINT16, OneTest) {
    WNVector3<WN_INT16> vector = WNVector3<WN_INT16>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationINT32, OneTest) {
    WNVector3<WN_INT32> vector = WNVector3<WN_INT32>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationINT64, OneTest) {
    WNVector3<WN_INT64> vector = WNVector3<WN_INT64>(1, 2, 3);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVector3ValidationFLOAT16, OneTest) {
    WNVector3<WN_FLOAT16> vector = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                                         static_cast<WN_FLOAT16>(3.0f));

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 1.0f);
    }
}

TEST(WNVector3ValidationFLOAT32, OneTest) {
    WNVector3<WN_FLOAT32> vector = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1.0f);
    }
}

TEST(WNVector3ValidationFLOAT64, OneTest) {
    WNVector3<WN_FLOAT64> vector = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);

    vector.One();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        ASSERT_EQ(vector[i], 1.0);
    }
}
