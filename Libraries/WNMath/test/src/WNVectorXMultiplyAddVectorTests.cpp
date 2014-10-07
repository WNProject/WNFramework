#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, MultiplyAddVectorTest) {
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationUINT16, MultiplyAddVectorTest) {
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationUINT32, MultiplyAddVectorTest) {
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationUINT64, MultiplyAddVectorTest) {
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationINT8, MultiplyAddVectorTest) {
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationINT16, MultiplyAddVectorTest) {
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationINT32, MultiplyAddVectorTest) {
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationINT64, MultiplyAddVectorTest) {
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(2);
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(3);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector1[i], 5);
    }
}

TEST(WNVectorXValidationFLOAT16, MultiplyAddVectorTest) {
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(2.0f));
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(3.0f));

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1[i] - static_cast<WN_FLOAT16>(5.0f))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, MultiplyAddVectorTest) {
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(2.0f);
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(3.0f);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector1[i] - 5.0f), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, MultiplyAddVectorTest) {
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(2.0);
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(3.0);

    vector1.MultiplyAdd(vector2, vector3);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector1[i] - 5.0), ALLOWED_PERCENTAGE_ERROR);
    }
}
