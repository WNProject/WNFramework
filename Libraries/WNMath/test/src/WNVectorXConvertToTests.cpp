#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, ConvertToTest) {
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> int8Vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> int8Vector2 = fxd64Vector.ConvertTo<WN_UINT8>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int8Vector[i], int8Vector2[i]);
    }
}

TEST(WNVectorXValidationUINT16, ConvertToTest) {
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> int16Vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> int16Vector2 = fxd64Vector.ConvertTo<WN_UINT16>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int16Vector[i], int16Vector2[i]);
    }
}

TEST(WNVectorXValidationUINT32, ConvertToTest) {
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> int32Vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> int32Vector2 = fxd64Vector.ConvertTo<WN_UINT32>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int32Vector[i], int32Vector2[i]);
    }
}

TEST(WNVectorXValidationUINT64, ConvertToTest) {
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> int64Vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> int64Vector2 = fxd64Vector.ConvertTo<WN_UINT64>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int64Vector[i], int64Vector2[i]);
    }
}

TEST(WNVectorXValidationINT8, ConvertToTest) {
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> int8Vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> int8Vector2 = fxd64Vector.ConvertTo<WN_INT8>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int8Vector[i], int8Vector2[i]);
    }
}

TEST(WNVectorXValidationINT16, ConvertToTest) {
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> int16Vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> int16Vector2 = fxd64Vector.ConvertTo<WN_INT16>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int16Vector[i], int16Vector2[i]);
    }
}

TEST(WNVectorXValidationINT32, ConvertToTest) {
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> int32Vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> int32Vector2 = fxd64Vector.ConvertTo<WN_INT32>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int32Vector[i], int32Vector2[i]);
    }
}

TEST(WNVectorXValidationINT64, ConvertToTest) {
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> int64Vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> int64Vector2 = fxd64Vector.ConvertTo<WN_INT64>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(int64Vector[i], int64Vector2[i]);
    }
}

TEST(WNVectorXValidationFLOAT16, ConvertToTest) {
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> flt16Vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = flt16Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> flt16Vector2 = fxd64Vector.ConvertTo<WN_FLOAT16>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(flt16Vector[i] - flt16Vector2[i])), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, ConvertToTest) {
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> flt32Vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = flt32Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> flt32Vector2 = fxd64Vector.ConvertTo<WN_FLOAT32>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(flt32Vector[i] - flt32Vector2[i]), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, ConvertToTest) {
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> flt64Vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    const WNVectorX<WN_FIXED64, VECTOR_X_DIMENSION> fxd64Vector = flt64Vector.ConvertTo<WN_FIXED64>();
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> flt64Vector2 = fxd64Vector.ConvertTo<WN_FLOAT64>();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(flt64Vector[i] - flt64Vector2[i]), ALLOWED_PERCENTAGE_ERROR);
    }
}