#include "WNMath/test/inc/Common.h"

TEST(basic, abs) {
    ASSERT_EQ(wn::abs(wn_int8(-1)), wn_int8(1));
    ASSERT_EQ(wn::abs(wn_int16(-1)), wn_int16(1));
    ASSERT_EQ(wn::abs(wn_int32(-1)), wn_int32(1));
    ASSERT_EQ(wn::abs(wn_int64(-1)), wn_int64(1));
    ASSERT_EQ(wn::abs(wn_fixed8(-1)), wn_fixed8(1));
    ASSERT_EQ(wn::abs(wn_fixed16(-1)), wn_fixed16(1));
    ASSERT_EQ(wn::abs(wn_fixed32(-1)), wn_fixed32(1));
    ASSERT_EQ(wn::abs(wn_fixed64(-1)), wn_fixed64(1));
    ASSERT_EQ(wn::abs(wn_float16(-1.0f)), wn_float16(1.0f));
    ASSERT_EQ(wn::abs(wn_float32(-1.0f)), wn_float32(1.0f));
    ASSERT_EQ(wn::abs(wn_float64(-1.0)), wn_float64(1.0));
}

TEST(basic, ceil) {
    ASSERT_EQ(wn::ceil(wn_fixed8(1.346f)), wn_fixed8(2));
    ASSERT_EQ(wn::ceil(wn_fixed16(1.346f)), wn_fixed16(2));
    ASSERT_EQ(wn::ceil(wn_fixed32(1.346f)), wn_fixed32(2));
    ASSERT_EQ(wn::ceil(wn_fixed64(1.346f)), wn_fixed64(2));
}

TEST(basic, floor) {
    ASSERT_EQ(wn::floor(wn_fixed8(1.346f)), wn_fixed8(1));
    ASSERT_EQ(wn::floor(wn_fixed16(1.346f)), wn_fixed16(1));
    ASSERT_EQ(wn::floor(wn_fixed32(1.346f)), wn_fixed32(1));
    ASSERT_EQ(wn::floor(wn_fixed64(1.346f)), wn_fixed64(1));
}

TEST(basic, round) {
    ASSERT_EQ(wn::round(wn_fixed8(1.346f)), wn_fixed8(1));
    ASSERT_EQ(wn::round(wn_fixed16(1.746f)), wn_fixed16(2));
    ASSERT_EQ(wn::round(wn_fixed32(1.346f)), wn_fixed32(1));
    ASSERT_EQ(wn::round(wn_fixed64(1.746f)), wn_fixed64(2));
    ASSERT_EQ(wn::round(wn_float16(1.746f)), wn_float16(2));
    ASSERT_EQ(wn::round(wn_float32(1.346f)), wn_float32(1));
    ASSERT_EQ(wn::round(wn_float64(1.746f)), wn_float64(2));
}

TEST(basic, min) {
    ASSERT_EQ(wn::min(wn_fixed8(1.346f), wn_fixed8(-0.345f)), wn_fixed8(-0.345f));
    ASSERT_EQ(wn::min(wn_fixed16(1.346f), wn_fixed16(-0.345f)), wn_fixed16(-0.345f));
    ASSERT_EQ(wn::min(wn_fixed32(1.346f), wn_fixed32(-0.345f)), wn_fixed32(-0.345f));
    ASSERT_EQ(wn::min(wn_fixed64(1.346f), wn_fixed64(-0.345f)), wn_fixed64(-0.345f));
}