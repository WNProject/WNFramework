#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, MultiplyAddTest) {
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationUINT16, MultiplyAddTest) {
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationUINT32, MultiplyAddTest) {
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationUINT64, MultiplyAddTest) {
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationINT8, MultiplyAddTest) {
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationINT16, MultiplyAddTest) {
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationINT32, MultiplyAddTest) {
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationINT64, MultiplyAddTest) {
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);

    vector.MultiplyAdd(2, 3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 5);
    }
}

TEST(WNVectorXValidationFLOAT16, MultiplyAddTest) {
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));

    vector.MultiplyAdd(static_cast<WN_FLOAT16>(2.0f), static_cast<WN_FLOAT16>(3.0f));

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[i] - static_cast<WN_FLOAT16>(5.0f))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, MultiplyAddTest) {
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);

    vector.MultiplyAdd(2.0f, 3.0f);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i] - 5.0f), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, MultiplyAddTest) {
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);

    vector.MultiplyAdd(2.0, 3.0);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i] - 5.0), ALLOWED_PERCENTAGE_ERROR);
    }
}
