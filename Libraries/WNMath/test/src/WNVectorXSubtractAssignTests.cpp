#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, SubtractAssignTest) {
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT16, SubtractAssignTest) {
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT32, SubtractAssignTest) {
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT64, SubtractAssignTest) {
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT8, SubtractAssignTest) {
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT16, SubtractAssignTest) {
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT32, SubtractAssignTest) {
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT64, SubtractAssignTest) {
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);

    vector -= 1;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationFLOAT16, SubtractAssignTest) {
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));

    vector -= static_cast<WN_FLOAT16>(1.0f);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector[i])), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, SubtractAssignTest) {
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);

    vector -= 1.0f;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i]), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, SubtractAssignTest) {
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);

    vector -= 1.0;

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector[i]), ALLOWED_PERCENTAGE_ERROR);
    }
}
