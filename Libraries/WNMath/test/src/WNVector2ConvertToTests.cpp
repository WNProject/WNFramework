#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector2.h"
#endif

using namespace WNMath;

TEST(WNVector2ValidationUINT8, ConvertToTest) {
    const WNVector2<WN_UINT8> int8Vector = WNVector2<WN_UINT8>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_UINT8> int8Vector2 = fxd64Vector.ConvertTo<WN_UINT8>();

    ASSERT_EQ(int8Vector.GetX(), int8Vector2.GetX());
    ASSERT_EQ(int8Vector.GetY(), int8Vector2.GetY());
}

TEST(WNVector2ValidationUINT16, ConvertToTest) {
    const WNVector2<WN_UINT16> int16Vector = WNVector2<WN_UINT16>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_UINT16> int16Vector2 = fxd64Vector.ConvertTo<WN_UINT16>();

    ASSERT_EQ(int16Vector.GetX(), int16Vector2.GetX());
    ASSERT_EQ(int16Vector.GetY(), int16Vector2.GetY());
}

TEST(WNVector2ValidationUINT32, ConvertToTest) {
    const WNVector2<WN_UINT32> int32Vector = WNVector2<WN_UINT32>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_UINT32> int32Vector2 = fxd64Vector.ConvertTo<WN_UINT32>();

    ASSERT_EQ(int32Vector.GetX(), int32Vector2.GetX());
    ASSERT_EQ(int32Vector.GetY(), int32Vector2.GetY());
}

TEST(WNVector2ValidationUINT64, ConvertToTest) {
    const WNVector2<WN_UINT64> int64Vector = WNVector2<WN_UINT64>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_UINT64> int64Vector2 = fxd64Vector.ConvertTo<WN_UINT64>();

    ASSERT_EQ(int64Vector.GetX(), int64Vector2.GetX());
    ASSERT_EQ(int64Vector.GetY(), int64Vector2.GetY());
}

TEST(WNVector2ValidationINT8, ConvertToTest) {
    const WNVector2<WN_INT8> int8Vector = WNVector2<WN_INT8>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_INT8> int8Vector2 = fxd64Vector.ConvertTo<WN_INT8>();

    ASSERT_EQ(int8Vector.GetX(), int8Vector2.GetX());
    ASSERT_EQ(int8Vector.GetY(), int8Vector2.GetY());
}

TEST(WNVector2ValidationINT16, ConvertToTest) {
    const WNVector2<WN_INT16> int16Vector = WNVector2<WN_INT16>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_INT16> int16Vector2 = fxd64Vector.ConvertTo<WN_INT16>();

    ASSERT_EQ(int16Vector.GetX(), int16Vector2.GetX());
    ASSERT_EQ(int16Vector.GetY(), int16Vector2.GetY());
}

TEST(WNVector2ValidationINT32, ConvertToTest) {
    const WNVector2<WN_INT32> int32Vector = WNVector2<WN_INT32>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_INT32> int32Vector2 = fxd64Vector.ConvertTo<WN_INT32>();

    ASSERT_EQ(int32Vector.GetX(), int32Vector2.GetX());
    ASSERT_EQ(int32Vector.GetY(), int32Vector2.GetY());
}

TEST(WNVector2ValidationINT64, ConvertToTest) {
    const WNVector2<WN_INT64> int64Vector = WNVector2<WN_INT64>(1, 2);
    const WNVector2<WN_FIXED64> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_INT64> int64Vector2 = fxd64Vector.ConvertTo<WN_INT64>();

    ASSERT_EQ(int64Vector.GetX(), int64Vector2.GetX());
    ASSERT_EQ(int64Vector.GetY(), int64Vector2.GetY());
}

TEST(WNVector2ValidationFLOAT32, ConvertToTest) {
    const WNVector2<WN_FLOAT32> flt32Vector = WNVector2<WN_FLOAT32>(1.0f, 2.0f);
    const WNVector2<WN_FIXED64> fxd64Vector = flt32Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_FLOAT32> flt32Vector2 = fxd64Vector.ConvertTo<WN_FLOAT32>();

    ASSERT_LT(flt32Vector.GetX() - flt32Vector2.GetX(), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(flt32Vector.GetY() - flt32Vector2.GetY(), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector2ValidationFLOAT64, ConvertToTest) {
    const WNVector2<WN_FLOAT64> flt64Vector = WNVector2<WN_FLOAT64>(1.0, 2.0);
    const WNVector2<WN_FIXED64> fxd64Vector = flt64Vector.ConvertTo<WN_FIXED64>();
    const WNVector2<WN_FLOAT64> flt64Vector2 = fxd64Vector.ConvertTo<WN_FLOAT64>();

    ASSERT_LT(flt64Vector.GetX() - flt64Vector2.GetX(), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(flt64Vector.GetY() - flt64Vector2.GetY(), ALLOWED_PERCENTAGE_ERROR);
}