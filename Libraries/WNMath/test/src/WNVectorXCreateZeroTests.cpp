#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVectorX.h"
#endif

using namespace WNMath;

TEST(WNVectorXValidationUINT8, CreateZeroTest) {
    WN_UINT8 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_UINT8>(i + 1);
    }

    WNVectorX<WN_UINT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_UINT8, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT16, CreateZeroTest) {
    WN_UINT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_UINT16>(i + 1);
    }

    WNVectorX<WN_UINT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_UINT16, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT32, CreateZeroTest) {
    WN_UINT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    WNVectorX<WN_UINT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_UINT32, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationUINT64, CreateZeroTest) {
    WN_UINT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    WNVectorX<WN_UINT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_UINT64, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT8, CreateZeroTest) {
    WN_INT8 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_INT8>(i + 1);
    }

    WNVectorX<WN_INT8, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_INT8, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT16, CreateZeroTest) {
    WN_INT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_INT16>(i + 1);
    }

    WNVectorX<WN_INT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_INT16, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT32, CreateZeroTest) {
    WN_INT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    WNVectorX<WN_INT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_INT32, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationINT64, CreateZeroTest) {
    WN_INT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = (i + 1);
    }

    WNVectorX<WN_INT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_INT64, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0);
    }
}

TEST(WNVectorXValidationFLOAT16, CreateZeroTest) {
    WN_FLOAT16 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT16>(i + 1);
    }

    WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_FLOAT16, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(static_cast<WN_FLOAT32>(vector[i]), 0.0f);
    }
}

TEST(WNVectorXValidationFLOAT32, CreateZeroTest) {
    WN_FLOAT32 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT32>(i + 1);
    }

    WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_FLOAT32, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0.0f);
    }
}

TEST(WNVectorXValidationFLOAT64, CreateZeroTest) {
    WN_FLOAT64 numbers[VECTOR_X_DIMENSION];

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        numbers[i] = static_cast<WN_FLOAT64>(i + 1);
    }

    WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION> vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>(numbers);

    vector = WNVectorX<WN_FLOAT64, VECTOR_X_DIMENSION>::CreateZero();

    for (WN_UINT32 i = 0; i < VECTOR_X_DIMENSION; ++i) {
        ASSERT_EQ(vector[i], 0.0);
    }
}
