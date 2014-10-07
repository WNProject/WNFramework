#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix2.h"
#endif

using namespace WNMath;

TEST(WNMatrix2ValidationUINT8, CreateZeroTest) {
    WNMatrix2<WN_UINT8> matrix = WNMatrix2<WN_UINT8>(1);

    matrix = WNMatrix2<WN_UINT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT16, CreateZeroTest) {
    WNMatrix2<WN_UINT16> matrix = WNMatrix2<WN_UINT16>(1);

    matrix = WNMatrix2<WN_UINT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT32, CreateZeroTest) {
    WNMatrix2<WN_UINT32> matrix = WNMatrix2<WN_UINT32>(1);

    matrix = WNMatrix2<WN_UINT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationUINT64, CreateZeroTest) {
    WNMatrix2<WN_UINT64> matrix = WNMatrix2<WN_UINT64>(1);

    matrix = WNMatrix2<WN_UINT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationINT8, CreateZeroTest) {
    WNMatrix2<WN_INT8> matrix = WNMatrix2<WN_INT8>(1);

    matrix = WNMatrix2<WN_INT8>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationINT16, CreateZeroTest) {
    WNMatrix2<WN_INT16> matrix = WNMatrix2<WN_INT16>(1);

    matrix = WNMatrix2<WN_INT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationINT32, CreateZeroTest) {
    WNMatrix2<WN_INT32> matrix = WNMatrix2<WN_INT32>(1);

    matrix = WNMatrix2<WN_INT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationINT64, CreateZeroTest) {
    WNMatrix2<WN_INT64> matrix = WNMatrix2<WN_INT64>(1);

    matrix = WNMatrix2<WN_INT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrix2ValidationFLOAT16, CreateZeroTest) {
    WNMatrix2<WN_FLOAT16> matrix = WNMatrix2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    matrix = WNMatrix2<WN_FLOAT16>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i]), 0.0f);
    }
}

TEST(WNMatrix2ValidationFLOAT32, CreateZeroTest) {
    WNMatrix2<WN_FLOAT32> matrix = WNMatrix2<WN_FLOAT32>(1.0f);

    matrix = WNMatrix2<WN_FLOAT32>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0.0f);
    }
}

TEST(WNMatrix2ValidationFLOAT64, CreateZeroTest) {
    WNMatrix2<WN_FLOAT64> matrix = WNMatrix2<WN_FLOAT64>(1.0);

    matrix = WNMatrix2<WN_FLOAT64>::CreateZero();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        ASSERT_EQ(matrix[i], 0.0);
    }
}
