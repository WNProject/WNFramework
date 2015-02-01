#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, DotTest) {
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);
    const WN_UINT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationUINT16, DotTest) {
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);
    const WN_UINT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationUINT32, DotTest) {
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);
    const WN_UINT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationUINT64, DotTest) {
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);
    const WN_UINT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationINT8, DotTest) {
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);
    const WN_INT8 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationINT16, DotTest) {
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);
    const WN_INT16 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationINT32, DotTest) {
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);
    const WN_INT32 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationINT64, DotTest) {
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);
    const WN_INT64 dot = vector.Dot(vector);

    ASSERT_EQ(dot, 10);
}

TEST(WNVectorXValidationFLOAT16, DotTest) {
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    const WN_FLOAT16 dot = vector.Dot(vector);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(dot - static_cast<WN_FLOAT16>(10.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT32, DotTest) {
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    const WN_FLOAT32 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 10.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT64, DotTest) {
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    const WN_FLOAT64 dot = vector.Dot(vector);

    ASSERT_LT(abs(dot - 10.0), ALLOWED_PERCENTAGE_ERROR);
}
