// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/test/inc/Common.h"

TEST(type_traits, dependent_true) {
    ASSERT_TRUE(wn::dependent_true<wn_bool>::value);
    ASSERT_TRUE(wn::dependent_true<wn_int32>::value);
    ASSERT_TRUE(wn::dependent_true<wn_uint32>::value);
    ASSERT_TRUE(wn::dependent_true<wn_float32>::value);
}

TEST(type_traits, dependent_false) {
    ASSERT_FALSE(wn::dependent_false<wn_bool>::value);
    ASSERT_FALSE(wn::dependent_false<wn_int32>::value);
    ASSERT_FALSE(wn::dependent_false<wn_uint32>::value);
    ASSERT_FALSE(wn::dependent_false<wn_float32>::value);
}

TEST(type_traits, boolean_constant) {
    ASSERT_TRUE(wn::boolean_constant<wn_true>::value);
    ASSERT_TRUE(wn::boolean_constant<(10 % 10 == 0)>::value);
    ASSERT_FALSE(wn::boolean_constant<wn_false>::value);
    ASSERT_FALSE(wn::boolean_constant<(10 % 4 == 0)>::value);
}

TEST(type_traits, is_same_decayed) {
    ASSERT_TRUE((wn::is_same_decayed<const wn_bool, wn_bool>::value));
    ASSERT_TRUE((wn::is_same_decayed<wn_int32, volatile wn_int32>::value));
    ASSERT_TRUE((wn::is_same_decayed<const volatile wn_uint32, const wn_uint32>::value));
    ASSERT_TRUE((wn::is_same_decayed<volatile wn_float32, wn_float32>::value));
    ASSERT_FALSE((wn::is_same_decayed<const wn_bool, wn_uint32>::value));
    ASSERT_FALSE((wn::is_same_decayed<wn_int32, volatile wn_float32>::value));
    ASSERT_FALSE((wn::is_same_decayed<const volatile wn_uint32, const wn_bool>::value));
    ASSERT_FALSE((wn::is_same_decayed<volatile wn_float32, wn_int32>::value));
    ASSERT_FALSE((wn::is_same_decayed<wn_uint32, wn_bool>::value));
    ASSERT_FALSE((wn::is_same_decayed<wn_float32, wn_int32>::value));
    ASSERT_FALSE((wn::is_same_decayed<wn_uint32, wn_int32>::value));
}

TEST(type_traits, is_floating_point) {
    ASSERT_TRUE(wn::is_floating_point<wn_float8>::value);
    ASSERT_TRUE(wn::is_floating_point<const wn_float16>::value);
    ASSERT_TRUE(wn::is_floating_point<volatile wn_float32>::value);
    ASSERT_TRUE(wn::is_floating_point<const volatile wn_float64>::value);
    ASSERT_FALSE(wn::is_floating_point<wn_bool>::value);
    ASSERT_FALSE(wn::is_floating_point<const wn_int32>::value);
    ASSERT_FALSE(wn::is_floating_point<volatile wn_uint32>::value);
    ASSERT_FALSE(wn::is_floating_point<const volatile wn_fixed32>::value);
}

TEST(type_traits, is_fixed_point) {
    ASSERT_TRUE(wn::is_fixed_point<wn_fixed8>::value);
    ASSERT_TRUE(wn::is_fixed_point<const wn_fixed16>::value);
    ASSERT_TRUE(wn::is_fixed_point<volatile wn_fixed32>::value);
    ASSERT_TRUE(wn::is_fixed_point<const volatile wn_fixed64>::value);
    ASSERT_TRUE((wn::is_fixed_point<wn_fixed<wn_uint32, 25>>::value));
    ASSERT_FALSE(wn::is_fixed_point<wn_bool>::value);
    ASSERT_FALSE(wn::is_fixed_point<const wn_int32>::value);
    ASSERT_FALSE(wn::is_fixed_point<volatile wn_uint32>::value);
    ASSERT_FALSE(wn::is_fixed_point<const volatile wn_float32>::value);
}

TEST(type_traits, is_real) {
    ASSERT_TRUE(wn::is_real<wn_fixed8>::value);
    ASSERT_TRUE(wn::is_real<const wn_fixed16>::value);
    ASSERT_TRUE(wn::is_real<volatile wn_fixed32>::value);
    ASSERT_TRUE(wn::is_real<const volatile wn_fixed64>::value);
    ASSERT_TRUE((wn::is_real<wn_fixed<wn_uint32, 25>>::value));
    ASSERT_TRUE(wn::is_real<wn_float8>::value);
    ASSERT_TRUE(wn::is_real<const wn_float16>::value);
    ASSERT_TRUE(wn::is_real<volatile wn_float32>::value);
    ASSERT_TRUE(wn::is_real<const volatile wn_float64>::value);
    ASSERT_FALSE(wn::is_real<wn_bool>::value);
    ASSERT_FALSE(wn::is_real<const wn_int32>::value);
    ASSERT_FALSE(wn::is_real<volatile wn_uint32>::value);
}