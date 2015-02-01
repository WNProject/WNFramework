#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrixX.h"
#endif

using namespace WNMath;

TEST(WNMatrixXValidationUINT8, ZeroTest) {
    WNMatrixX<WN_UINT8, MATRIX_X_ORDER> matrix = WNMatrixX<WN_UINT8, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationUINT16, ZeroTest) {
    WNMatrixX<WN_UINT16, MATRIX_X_ORDER> matrix = WNMatrixX<WN_UINT16, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationUINT32, ZeroTest) {
    WNMatrixX<WN_UINT32, MATRIX_X_ORDER> matrix = WNMatrixX<WN_UINT32, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationUINT64, ZeroTest) {
    WNMatrixX<WN_UINT64, MATRIX_X_ORDER> matrix = WNMatrixX<WN_UINT64, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationINT8, ZeroTest) {
    WNMatrixX<WN_INT8, MATRIX_X_ORDER> matrix = WNMatrixX<WN_INT8, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationINT16, ZeroTest) {
    WNMatrixX<WN_INT16, MATRIX_X_ORDER> matrix = WNMatrixX<WN_INT16, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationINT32, ZeroTest) {
    WNMatrixX<WN_INT32, MATRIX_X_ORDER> matrix = WNMatrixX<WN_INT32, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationINT64, ZeroTest) {
    WNMatrixX<WN_INT64, MATRIX_X_ORDER> matrix = WNMatrixX<WN_INT64, MATRIX_X_ORDER>(1);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0);
    }
}

TEST(WNMatrixXValidationFLOAT16, ZeroTest) {
    WNMatrixX<WN_FLOAT16, MATRIX_X_ORDER> matrix = WNMatrixX<WN_FLOAT16, MATRIX_X_ORDER>(static_cast<WN_FLOAT16>(1.0f));

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i]), 0.0f);
    }
}

TEST(WNMatrixXValidationFLOAT32, ZeroTest) {
    WNMatrixX<WN_FLOAT32, MATRIX_X_ORDER> matrix = WNMatrixX<WN_FLOAT32, MATRIX_X_ORDER>(1.0f);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0.0f);
    }
}

TEST(WNMatrixXValidationFLOAT64, ZeroTest) {
    WNMatrixX<WN_FLOAT64, MATRIX_X_ORDER> matrix = WNMatrixX<WN_FLOAT64, MATRIX_X_ORDER>(1.0);

    matrix.Zero();

    for (WN_UINT32 i = 0; i < MATRIX_X_DIMENSION; ++i) {
        ASSERT_EQ(matrix[i], 0.0);
    }
}
