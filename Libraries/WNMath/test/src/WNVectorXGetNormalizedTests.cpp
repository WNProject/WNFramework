#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationFLOAT16, GetNormalizedTest) {
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector2 = vector1.GetNormalized();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector1[i] - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    }

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2[i] - static_cast<WN_FLOAT16>(0.31616211f))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, GetNormalizedTest) {
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector2 = vector1.GetNormalized();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector1[i] - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    }

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector2[i] - 0.31616211f), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, GetNormalizedTest) {
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector1 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector2 = vector1.GetNormalized();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector1[i] - 1.0), ALLOWED_PERCENTAGE_ERROR);
    }

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector2[i] - 0.31616211), ALLOWED_PERCENTAGE_ERROR);
    }
}