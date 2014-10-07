#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, LengthSquaredTest) {
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);
    const WN_UINT8 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationUINT16, LengthSquaredTest) {
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);
    const WN_UINT16 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationUINT32, LengthSquaredTest) {
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);
    const WN_UINT32 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationUINT64, LengthSquaredTest) {
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);
    const WN_UINT64 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationINT8, LengthSquaredTest) {
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);
    const WN_INT8 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationINT16, LengthSquaredTest) {
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);
    const WN_INT16 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationINT32, LengthSquaredTest) {
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);
    const WN_INT32 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationINT64, LengthSquaredTest) {
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);
    const WN_INT64 lengthSquared = vector.LengthSquared();

    ASSERT_EQ(lengthSquared, 10);
}

TEST(WNVectorXValidationFLOAT16, LengthSquaredTest) {
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    const WN_FLOAT16 lengthSquared = vector.LengthSquared();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(lengthSquared - static_cast<WN_FLOAT16>(10.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT32, LengthSquaredTest) {
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    const WN_FLOAT32 lengthSquared = vector.LengthSquared();

    ASSERT_LT(abs(lengthSquared - 10.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT64, LengthSquaredTest) {
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    const WN_FLOAT64 lengthSquared = vector.LengthSquared();

    ASSERT_LT(abs(lengthSquared - 10.0), ALLOWED_PERCENTAGE_ERROR);
}
