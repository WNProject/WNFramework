#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, LengthTest) {
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);
    const WN_UINT8 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationUINT16, LengthTest) {
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);
    const WN_UINT16 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationUINT32, LengthTest) {
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);
    const WN_UINT32 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationUINT64, LengthTest) {
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);
    const WN_UINT64 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationINT8, LengthTest) {
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);
    const WN_INT8 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationINT16, LengthTest) {
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);
    const WN_INT16 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationINT32, LengthTest) {
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);
    const WN_INT32 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationINT64, LengthTest) {
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);
    const WN_INT64 length = vector.Length();

    ASSERT_EQ(length, 3);
}

TEST(WNVectorXValidationFLOAT16, LengthTest) {
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    const WN_FLOAT16 length = vector.Length();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(length - static_cast<WN_FLOAT16>(3.1621094f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT32, LengthTest) {
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    const WN_FLOAT32 length = vector.Length();

    ASSERT_LT(abs(length - 3.1616211f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVectorXValidationFLOAT64, LengthTest) {
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    const WN_FLOAT64 length = vector.Length();

    ASSERT_LT(abs(length - 3.1622776601683795), ALLOWED_PERCENTAGE_ERROR);
}
