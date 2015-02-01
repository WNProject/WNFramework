#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, ArraySubscriptTest) {
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT16, ArraySubscriptTest) {
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT32, ArraySubscriptTest) {
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT64, ArraySubscriptTest) {
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT8, ArraySubscriptTest) {
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT16, ArraySubscriptTest) {
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT32, ArraySubscriptTest) {
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT64, ArraySubscriptTest) {
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationFLOAT16, ArraySubscriptTest) {
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[i] - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, ArraySubscriptTest) {
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i] - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, ArraySubscriptTest) {
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i] - 1.0), ALLOWED_PERCENTAGE_ERROR);
    }
}
