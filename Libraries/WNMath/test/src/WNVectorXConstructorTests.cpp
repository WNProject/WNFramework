#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, ConstructorTest) {
    WN_UINT8 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_UINT8>(i + 1);
    }

    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationUINT16, ConstructorTest) {
    WN_UINT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_UINT16>(i + 1);
    }

    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationUINT32, ConstructorTest) {
    WN_UINT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationUINT64, ConstructorTest) {
    WN_UINT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationINT8, ConstructorTest) {
    WN_INT8 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_INT8>(i + 1);
    }

    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationINT16, ConstructorTest) {
    WN_INT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_INT16>(i + 1);
    }

    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationINT32, ConstructorTest) {
    WN_INT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationINT64, ConstructorTest) {
    WN_INT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(1);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector3[i], (i + 1));
    }
}

TEST(WNVectorXValidationFLOAT16, ConstructorTest) {
    WN_FLOAT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT16>(i + 1);
    }

    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(static_cast<WN_FLOAT16>(1.0f));

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2[i] - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    }

    const WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector3[i] - static_cast<WN_FLOAT16>(i + 1))), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT32, ConstructorTest) {
    WN_FLOAT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT32>(i + 1);
    }

    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(1.0f);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector2[i] - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    }

    const WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector3[i] - static_cast<WN_FLOAT32>(i + 1)), ALLOWED_PERCENTAGE_ERROR);
    }
}

TEST(WNVectorXValidationFLOAT64, ConstructorTest) {
    WN_FLOAT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT64>(i + 1);
    }

    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector1;
    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector2 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(1.0);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector2[i] - 1.0), ALLOWED_PERCENTAGE_ERROR);
    }

    const WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector3 = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(numbers);

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_LT(abs(vector3[i] - static_cast<WN_FLOAT64>(i + 1)), ALLOWED_PERCENTAGE_ERROR);
    }
}