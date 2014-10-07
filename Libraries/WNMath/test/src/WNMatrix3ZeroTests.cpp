#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix3.h"
#endif

using namespace WNMath;

TEST(WNMatrix3ValidationUINT8, ZeroTest) {
    WNMatrix3<WN_UINT8> matrix = WNMatrix3<WN_UINT8>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

/*TEST(WNMatrix3ValidationUINT16, ZeroTest) {
    WNMatrix3<WN_UINT16> matrix = WNMatrix3<WN_UINT16>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationUINT32, ZeroTest) {
    WNMatrix3<WN_UINT32> matrix = WNMatrix3<WN_UINT32>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationUINT64, ZeroTest) {
    WNMatrix3<WN_UINT64> matrix = WNMatrix3<WN_UINT64>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationINT8, ZeroTest) {
    WNMatrix3<WN_INT8> matrix = WNMatrix3<WN_INT8>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationINT16, ZeroTest) {
    WNMatrix3<WN_INT16> matrix = WNMatrix3<WN_INT16>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationINT32, ZeroTest) {
    WNMatrix3<WN_INT32> matrix = WNMatrix3<WN_INT32>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationINT64, ZeroTest) {
    WNMatrix3<WN_INT64> matrix = WNMatrix3<WN_INT64>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix3ValidationFLOAT16, ZeroTest) {
    WNMatrix3<WN_FLOAT16> matrix = WNMatrix3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i]), 0.0f);
    }
}

TEST(WNMatrix3ValidationFLOAT32, ZeroTest) {
    WNMatrix3<WN_FLOAT32> matrix = WNMatrix3<WN_FLOAT32>(1.0f);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0.0f);
    }
}

TEST(WNMatrix3ValidationFLOAT64, ZeroTest) {
    WNMatrix3<WN_FLOAT64> matrix = WNMatrix3<WN_FLOAT64>(1.0);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < 9; ++i) {
        ASSERT_EQ(matrix[i], 0.0);
    }
}*/
