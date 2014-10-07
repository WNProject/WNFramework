#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector3.h"
#endif

using namespace WNMath;

TEST(WNVector3ValidationUINT8, ConstructorTest) {
    const WN_UINT8 numbers[] = {1, 2, 3};
    const WNVector3<WN_UINT8> vector1;
    const WNVector3<WN_UINT8> vector2 = WNVector3<WN_UINT8>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_UINT8> vector3 = WNVector3<WN_UINT8>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_UINT8> vector4 = WNVector3<WN_UINT8>(WNVector2<WN_UINT8>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_UINT8> vector5 = WNVector3<WN_UINT8>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationUINT16, ConstructorTest) {
    const WN_UINT16 numbers[] = {1, 2, 3};
    const WNVector3<WN_UINT16> vector1;
    const WNVector3<WN_UINT16> vector2 = WNVector3<WN_UINT16>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_UINT16> vector3 = WNVector3<WN_UINT16>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_UINT16> vector4 = WNVector3<WN_UINT16>(WNVector2<WN_UINT16>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_UINT16> vector5 = WNVector3<WN_UINT16>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationUINT32, ConstructorTest) {
    const WN_UINT32 numbers[] = {1, 2, 3};
    const WNVector3<WN_UINT32> vector1;
    const WNVector3<WN_UINT32> vector2 = WNVector3<WN_UINT32>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_UINT32> vector3 = WNVector3<WN_UINT32>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_UINT32> vector4 = WNVector3<WN_UINT32>(WNVector2<WN_UINT32>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_UINT32> vector5 = WNVector3<WN_UINT32>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationUINT64, ConstructorTest) {
    const WN_UINT64 numbers[] = {1, 2, 3};
    const WNVector3<WN_UINT64> vector1;
    const WNVector3<WN_UINT64> vector2 = WNVector3<WN_UINT64>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_UINT64> vector3 = WNVector3<WN_UINT64>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_UINT64> vector4 = WNVector3<WN_UINT64>(WNVector2<WN_UINT64>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_UINT64> vector5 = WNVector3<WN_UINT64>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationINT8, ConstructorTest) {
    const WN_INT8 numbers[] = {1, 2, 3};
    const WNVector3<WN_INT8> vector1;
    const WNVector3<WN_INT8> vector2 = WNVector3<WN_INT8>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_INT8> vector3 = WNVector3<WN_INT8>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_INT8> vector4 = WNVector3<WN_INT8>(WNVector2<WN_INT8>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_INT8> vector5 = WNVector3<WN_INT8>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationINT16, ConstructorTest) {
    const WN_INT16 numbers[] = {1, 2, 3};
    const WNVector3<WN_INT16> vector1;
    const WNVector3<WN_INT16> vector2 = WNVector3<WN_INT16>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_INT16> vector3 = WNVector3<WN_INT16>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_INT16> vector4 = WNVector3<WN_INT16>(WNVector2<WN_INT16>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_INT16> vector5 = WNVector3<WN_INT16>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationINT32, ConstructorTest) {
    const WN_INT32 numbers[] = {1, 2, 3};
    const WNVector3<WN_INT32> vector1;
    const WNVector3<WN_INT32> vector2 = WNVector3<WN_INT32>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_INT32> vector3 = WNVector3<WN_INT32>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_INT32> vector4 = WNVector3<WN_INT32>(WNVector2<WN_INT32>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_INT32> vector5 = WNVector3<WN_INT32>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationINT64, ConstructorTest) {
    const WN_INT64 numbers[] = {1, 2, 3};
    const WNVector3<WN_INT64> vector1;
    const WNVector3<WN_INT64> vector2 = WNVector3<WN_INT64>(1);

    ASSERT_EQ(vector2.GetX(), 1);
    ASSERT_EQ(vector2.GetY(), 1);
    ASSERT_EQ(vector2.GetZ(), 1);

    const WNVector3<WN_INT64> vector3 = WNVector3<WN_INT64>(numbers);

    ASSERT_EQ(vector3.GetX(), 1);
    ASSERT_EQ(vector3.GetY(), 2);
    ASSERT_EQ(vector3.GetZ(), 3);

    const WNVector3<WN_INT64> vector4 = WNVector3<WN_INT64>(WNVector2<WN_INT64>(1, 2), 3);

    ASSERT_EQ(vector4.GetX(), 1);
    ASSERT_EQ(vector4.GetY(), 2);
    ASSERT_EQ(vector4.GetZ(), 3);

    const WNVector3<WN_INT64> vector5 = WNVector3<WN_INT64>(1, 2, 3);

    ASSERT_EQ(vector5.GetX(), 1);
    ASSERT_EQ(vector5.GetY(), 2);
    ASSERT_EQ(vector5.GetZ(), 3);
}

TEST(WNVector3ValidationFLOAT16, ConstructorTest) {
    const WN_FLOAT16 numbers[] = {static_cast<WN_FLOAT16>(1.0f), static_cast<WN_FLOAT16>(2.0f),
                                  static_cast<WN_FLOAT16>(3.0f)};
    const WNVector3<WN_FLOAT16> vector1;
    const WNVector3<WN_FLOAT16> vector2 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetY() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector2.GetZ() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT16> vector3 = WNVector3<WN_FLOAT16>(numbers);

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector3.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector3.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector3.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT16> vector4 = WNVector3<WN_FLOAT16>(WNVector2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f),
                                                                                      static_cast<WN_FLOAT16>(2.0f)),
                                                                                      static_cast<WN_FLOAT16>(3.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector4.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector4.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector4.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT16> vector5 = WNVector3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f),
                                                                static_cast<WN_FLOAT16>(2.0f),
                                                                static_cast<WN_FLOAT16>(3.0f));

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector5.GetX() - static_cast<WN_FLOAT16>(1.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector5.GetY() - static_cast<WN_FLOAT16>(2.0f))), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(vector5.GetZ() - static_cast<WN_FLOAT16>(3.0f))), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT32, ConstructorTest) {
    const WN_FLOAT32 numbers[] = {1.0f, 2.0f, 3.0f};
    const WNVector3<WN_FLOAT32> vector1;
    const WNVector3<WN_FLOAT32> vector2 = WNVector3<WN_FLOAT32>(1.0f);

    ASSERT_LT(abs(vector2.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 1.0f), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT32> vector3 = WNVector3<WN_FLOAT32>(numbers);

    ASSERT_LT(abs(vector3.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector3.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector3.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT32> vector4 = WNVector3<WN_FLOAT32>(WNVector2<WN_FLOAT32>(1.0f, 2.0f), 3.0f);

    ASSERT_LT(abs(vector4.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector4.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector4.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT32> vector5 = WNVector3<WN_FLOAT32>(1.0f, 2.0f, 3.0f);

    ASSERT_LT(abs(vector5.GetX() - 1.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector5.GetY() - 2.0f), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector5.GetZ() - 3.0f), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector3ValidationFLOAT64, ConstructorTest) {
    const WN_FLOAT64 numbers[] = {1.0, 2.0, 3.0};
    const WNVector3<WN_FLOAT64> vector1;
    const WNVector3<WN_FLOAT64> vector2 = WNVector3<WN_FLOAT64>(1.0);

    ASSERT_LT(abs(vector2.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetY() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector2.GetZ() - 1.0), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT64> vector3 = WNVector3<WN_FLOAT64>(numbers);

    ASSERT_LT(abs(vector3.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector3.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector3.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT64> vector4 = WNVector3<WN_FLOAT64>(WNVector2<WN_FLOAT64>(1.0, 2.0), 3.0);

    ASSERT_LT(abs(vector4.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector4.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector4.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);

    const WNVector3<WN_FLOAT64> vector5 = WNVector3<WN_FLOAT64>(1.0, 2.0, 3.0);

    ASSERT_LT(abs(vector5.GetX() - 1.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector5.GetY() - 2.0), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(vector5.GetZ() - 3.0), ALLOWED_PERCENTAGE_ERROR);
}