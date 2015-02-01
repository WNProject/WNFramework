#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, OneTest) {
    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT16, OneTest) {
    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT32, OneTest) {
    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationUINT64, OneTest) {
    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT8, OneTest) {
    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT16, OneTest) {
    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT32, OneTest) {
    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationINT64, OneTest) {
    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(2);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1);
    }
}

TEST(WNVectorXValidationFLOAT16, OneTest) {
    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(2.0f));

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 1.0f);
    }
}

TEST(WNVectorXValidationFLOAT32, OneTest) {
    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(2.0f);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1.0f);
    }
}

TEST(WNVectorXValidationFLOAT64, OneTest) {
    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(2.0);

    vector.One();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 1.0);
    }
}
