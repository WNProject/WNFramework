// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/test/inc/Common.h"

TEST(type_traits, integral_constant) {
    EXPECT_TRUE((wn::core::integral_constant<wn_bool, wn_true>::value));
    EXPECT_EQ((wn::core::integral_constant<wn_uint32, (10 % 10)>::value), 0);
}

TEST(type_traits, boolean_constant) {
    EXPECT_TRUE(wn::core::boolean_constant<wn_true>::value);
    EXPECT_TRUE(wn::core::boolean_constant<(10 % 10 == 0)>::value);
    EXPECT_FALSE(wn::core::boolean_constant<wn_false>::value);
    EXPECT_FALSE(wn::core::boolean_constant<(10 % 4 == 0)>::value);
}

TEST(type_traits, is_same) {
    EXPECT_TRUE((wn::core::is_same<wn_bool, wn_bool>::value));
    EXPECT_TRUE((wn::core::is_same<wn_nullptr_t, wn_nullptr_t>::value));
    EXPECT_TRUE((wn::core::is_same<wn_float32, wn_float32>::value));
    EXPECT_TRUE((wn::core::is_same<wn_int64, wn_int64>::value));
    EXPECT_FALSE((wn::core::is_same<const wn_bool, wn_bool>::value));
    EXPECT_FALSE((wn::core::is_same<wn_int32, volatile wn_int32>::value));
    EXPECT_FALSE((wn::core::is_same<const volatile wn_uint32, const wn_uint32>::value));
    EXPECT_FALSE((wn::core::is_same<volatile wn_float32, wn_float32>::value));
    EXPECT_FALSE((wn::core::is_same<const wn_bool, wn_uint32>::value));
    EXPECT_FALSE((wn::core::is_same<wn_int32, volatile wn_float32>::value));
    EXPECT_FALSE((wn::core::is_same<const volatile wn_uint32, const wn_bool>::value));
    EXPECT_FALSE((wn::core::is_same<volatile wn_float32, wn_int32>::value));
    EXPECT_FALSE((wn::core::is_same<wn_uint32, wn_bool>::value));
    EXPECT_FALSE((wn::core::is_same<wn_float32, wn_int32>::value));
    EXPECT_FALSE((wn::core::is_same<wn_uint32, wn_int32>::value));
}

TEST(type_traits, is_same_decayed) {
    EXPECT_TRUE((wn::core::is_same_decayed<wn_bool, wn_bool>::value));
    EXPECT_TRUE((wn::core::is_same_decayed<const wn_bool, wn_bool>::value));
    EXPECT_TRUE((wn::core::is_same_decayed<wn_int32, volatile wn_int32>::value));
    EXPECT_TRUE((wn::core::is_same_decayed<const volatile wn_uint32, const wn_uint32>::value));
    EXPECT_TRUE((wn::core::is_same_decayed<volatile wn_float32, wn_float32>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<const wn_bool, wn_uint32>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<wn_int32, volatile wn_float32>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<const volatile wn_uint32, const wn_bool>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<volatile wn_float32, wn_int32>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<wn_uint32, wn_bool>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<wn_float32, wn_int32>::value));
    EXPECT_FALSE((wn::core::is_same_decayed<wn_uint32, wn_int32>::value));
}

TEST(type_traits, is_floating_point) {
    EXPECT_TRUE(wn::core::is_floating_point<wn_float8>::value);
    EXPECT_TRUE(wn::core::is_floating_point<const wn_float16>::value);
    EXPECT_TRUE(wn::core::is_floating_point<volatile wn_float32>::value);
    EXPECT_TRUE(wn::core::is_floating_point<const volatile wn_float64>::value);
    EXPECT_FALSE(wn::core::is_floating_point<wn_bool>::value);
    EXPECT_FALSE(wn::core::is_floating_point<const wn_int32>::value);
    EXPECT_FALSE(wn::core::is_floating_point<volatile wn_uint32>::value);
    EXPECT_FALSE(wn::core::is_floating_point<const volatile wn_fixed32>::value);
}

TEST(type_traits, is_fixed_point) {
    EXPECT_TRUE(wn::core::is_fixed_point<wn_fixed8>::value);
    EXPECT_TRUE(wn::core::is_fixed_point<const wn_fixed16>::value);
    EXPECT_TRUE(wn::core::is_fixed_point<volatile wn_fixed32>::value);
    EXPECT_TRUE(wn::core::is_fixed_point<const volatile wn_fixed64>::value);
    EXPECT_TRUE((wn::core::is_fixed_point<wn_fixed<wn_uint32, 25>>::value));
    EXPECT_FALSE(wn::core::is_fixed_point<wn_bool>::value);
    EXPECT_FALSE(wn::core::is_fixed_point<const wn_int32>::value);
    EXPECT_FALSE(wn::core::is_fixed_point<volatile wn_uint32>::value);
    EXPECT_FALSE(wn::core::is_fixed_point<const volatile wn_float32>::value);
}

TEST(type_traits, is_real) {
    EXPECT_TRUE(wn::core::is_real<wn_fixed8>::value);
    EXPECT_TRUE(wn::core::is_real<const wn_fixed16>::value);
    EXPECT_TRUE(wn::core::is_real<volatile wn_fixed32>::value);
    EXPECT_TRUE(wn::core::is_real<const volatile wn_fixed64>::value);
    EXPECT_TRUE((wn::core::is_real<wn_fixed<wn_uint32, 25>>::value));
    EXPECT_TRUE(wn::core::is_real<wn_float8>::value);
    EXPECT_TRUE(wn::core::is_real<const wn_float16>::value);
    EXPECT_TRUE(wn::core::is_real<volatile wn_float32>::value);
    EXPECT_TRUE(wn::core::is_real<const volatile wn_float64>::value);
    EXPECT_FALSE(wn::core::is_real<wn_bool>::value);
    EXPECT_FALSE(wn::core::is_real<const wn_int32>::value);
    EXPECT_FALSE(wn::core::is_real<volatile wn_uint32>::value);
}