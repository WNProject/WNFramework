#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNVector4.h"
#endif

using namespace WNMath;

TEST(WNVector4ValidationUINT8, ConvertToTest) {
    const WNVector4<WN_UINT8> int8Vector = WNVector4<WN_UINT8>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_UINT8> int8Vector2 = fxd64Vector.ConvertTo<WN_UINT8>();

    ASSERT_EQ(int8Vector.GetX(), int8Vector2.GetX());
    ASSERT_EQ(int8Vector.GetY(), int8Vector2.GetY());
    ASSERT_EQ(int8Vector.GetZ(), int8Vector2.GetZ());
    ASSERT_EQ(int8Vector.GetW(), int8Vector2.GetW());
}

TEST(WNVector4ValidationUINT16, ConvertToTest) {
    const WNVector4<WN_UINT16> int16Vector = WNVector4<WN_UINT16>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_UINT16> int16Vector2 = fxd64Vector.ConvertTo<WN_UINT16>();

    ASSERT_EQ(int16Vector.GetX(), int16Vector2.GetX());
    ASSERT_EQ(int16Vector.GetY(), int16Vector2.GetY());
    ASSERT_EQ(int16Vector.GetZ(), int16Vector2.GetZ());
    ASSERT_EQ(int16Vector.GetW(), int16Vector2.GetW());
}

TEST(WNVector4ValidationUINT32, ConvertToTest) {
    const WNVector4<WN_UINT32> int32Vector = WNVector4<WN_UINT32>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_UINT32> int32Vector2 = fxd64Vector.ConvertTo<WN_UINT32>();

    ASSERT_EQ(int32Vector.GetX(), int32Vector2.GetX());
    ASSERT_EQ(int32Vector.GetY(), int32Vector2.GetY());
    ASSERT_EQ(int32Vector.GetZ(), int32Vector2.GetZ());
    ASSERT_EQ(int32Vector.GetW(), int32Vector2.GetW());
}

TEST(WNVector4ValidationUINT64, ConvertToTest) {
    const WNVector4<WN_UINT64> int64Vector = WNVector4<WN_UINT64>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_UINT64> int64Vector2 = fxd64Vector.ConvertTo<WN_UINT64>();

    ASSERT_EQ(int64Vector.GetX(), int64Vector2.GetX());
    ASSERT_EQ(int64Vector.GetY(), int64Vector2.GetY());
    ASSERT_EQ(int64Vector.GetZ(), int64Vector2.GetZ());
    ASSERT_EQ(int64Vector.GetW(), int64Vector2.GetW());
}

TEST(WNVector4ValidationINT8, ConvertToTest) {
    const WNVector4<WN_INT8> int8Vector = WNVector4<WN_INT8>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int8Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_INT8> int8Vector2 = fxd64Vector.ConvertTo<WN_INT8>();

    ASSERT_EQ(int8Vector.GetX(), int8Vector2.GetX());
    ASSERT_EQ(int8Vector.GetY(), int8Vector2.GetY());
    ASSERT_EQ(int8Vector.GetZ(), int8Vector2.GetZ());
    ASSERT_EQ(int8Vector.GetW(), int8Vector2.GetW());
}

TEST(WNVector4ValidationINT16, ConvertToTest) {
    const WNVector4<WN_INT16> int16Vector = WNVector4<WN_INT16>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int16Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_INT16> int16Vector2 = fxd64Vector.ConvertTo<WN_INT16>();

    ASSERT_EQ(int16Vector.GetX(), int16Vector2.GetX());
    ASSERT_EQ(int16Vector.GetY(), int16Vector2.GetY());
    ASSERT_EQ(int16Vector.GetZ(), int16Vector2.GetZ());
    ASSERT_EQ(int16Vector.GetW(), int16Vector2.GetW());
}

TEST(WNVector4ValidationINT32, ConvertToTest) {
    const WNVector4<WN_INT32> int32Vector = WNVector4<WN_INT32>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int32Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_INT32> int32Vector2 = fxd64Vector.ConvertTo<WN_INT32>();

    ASSERT_EQ(int32Vector.GetX(), int32Vector2.GetX());
    ASSERT_EQ(int32Vector.GetY(), int32Vector2.GetY());
    ASSERT_EQ(int32Vector.GetZ(), int32Vector2.GetZ());
    ASSERT_EQ(int32Vector.GetW(), int32Vector2.GetW());
}

TEST(WNVector4ValidationINT64, ConvertToTest) {
    const WNVector4<WN_INT64> int64Vector = WNVector4<WN_INT64>(1, 2, 3, 4);
    const WNVector4<WN_FIXED64> fxd64Vector = int64Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_INT64> int64Vector2 = fxd64Vector.ConvertTo<WN_INT64>();

    ASSERT_EQ(int64Vector.GetX(), int64Vector2.GetX());
    ASSERT_EQ(int64Vector.GetY(), int64Vector2.GetY());
    ASSERT_EQ(int64Vector.GetZ(), int64Vector2.GetZ());
    ASSERT_EQ(int64Vector.GetW(), int64Vector2.GetW());
}

TEST(WNVector4ValidationFLOAT16, ConvertToTest) {
    const WNVector4<WN_FLOAT16> flt16Vector = WNVector4<WN_FLOAT16>(1.0f, 2.0f, 3.0f, 4.0f);
    const WNVector4<WN_FIXED64> fxd64Vector = flt16Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_FLOAT16> flt16Vector2 = fxd64Vector.ConvertTo<WN_FLOAT16>();

    ASSERT_LT(abs(static_cast<WN_FLOAT32>(flt16Vector.GetX() - flt16Vector2.GetX())), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(flt16Vector.GetY() - flt16Vector2.GetY())), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(flt16Vector.GetZ() - flt16Vector2.GetZ())), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(static_cast<WN_FLOAT32>(flt16Vector.GetW() - flt16Vector2.GetW())), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT32, ConvertToTest) {
    const WNVector4<WN_FLOAT32> flt32Vector = WNVector4<WN_FLOAT32>(1.0f, 2.0f, 3.0f, 4.0f);
    const WNVector4<WN_FIXED64> fxd64Vector = flt32Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_FLOAT32> flt32Vector2 = fxd64Vector.ConvertTo<WN_FLOAT32>();

    ASSERT_LT(abs(flt32Vector.GetX() - flt32Vector2.GetX()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt32Vector.GetY() - flt32Vector2.GetY()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt32Vector.GetZ() - flt32Vector2.GetZ()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt32Vector.GetW() - flt32Vector2.GetW()), ALLOWED_PERCENTAGE_ERROR);
}

TEST(WNVector4ValidationFLOAT64, ConvertToTest) {
    const WNVector4<WN_FLOAT64> flt64Vector = WNVector4<WN_FLOAT64>(1.0, 2.0, 3.0, 4.0);
    const WNVector4<WN_FIXED64> fxd64Vector = flt64Vector.ConvertTo<WN_FIXED64>();
    const WNVector4<WN_FLOAT64> flt64Vector2 = fxd64Vector.ConvertTo<WN_FLOAT64>();

    ASSERT_LT(abs(flt64Vector.GetX() - flt64Vector2.GetX()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt64Vector.GetY() - flt64Vector2.GetY()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt64Vector.GetZ() - flt64Vector2.GetZ()), ALLOWED_PERCENTAGE_ERROR);
    ASSERT_LT(abs(flt64Vector.GetW() - flt64Vector2.GetW()), ALLOWED_PERCENTAGE_ERROR);
}