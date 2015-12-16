// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNCore/inc/WNExtendedTypes.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"

TEST(type_traits, integral_constant) {
  EXPECT_TRUE((wn::core::integral_constant<wn_bool, wn_true>::value));
  EXPECT_EQ((wn::core::integral_constant<wn_uint32, (10 % 10)>::value), 0);
}

TEST(type_traits, index_constant) {
  EXPECT_EQ((wn::core::index_constant<0>::value), 0);
  EXPECT_EQ((wn::core::index_constant<(10 - 5)>::value), 5);
}

TEST(type_traits, bool_constant) {
  EXPECT_TRUE(wn::core::bool_constant<wn_true>::value);
  EXPECT_TRUE(wn::core::bool_constant<(10 % 10 == 0)>::value);
  EXPECT_FALSE(wn::core::bool_constant<wn_false>::value);
  EXPECT_FALSE(wn::core::bool_constant<(10 % 4 == 0)>::value);
}

TEST(type_traits, is_null_pointer) {
  EXPECT_TRUE((wn::core::is_null_pointer<decltype(nullptr)>::value));
  EXPECT_FALSE((wn::core::is_null_pointer<bool>::value));
}

TEST(type_traits, is_same) {
  EXPECT_TRUE((wn::core::is_same<wn_bool, wn_bool>::value));
  EXPECT_TRUE((wn::core::is_same<wn_nullptr_t, wn_nullptr_t>::value));
  EXPECT_TRUE((wn::core::is_same<wn_float32, wn_float32>::value));
  EXPECT_TRUE((wn::core::is_same<wn_int64, wn_int64>::value));
  EXPECT_FALSE((wn::core::is_same<const wn_bool, wn_bool>::value));
  EXPECT_FALSE((wn::core::is_same<wn_int32, volatile wn_int32>::value));
  EXPECT_FALSE(
      (wn::core::is_same<const volatile wn_uint32, const wn_uint32>::value));
  EXPECT_FALSE((wn::core::is_same<volatile wn_float32, wn_float32>::value));
  EXPECT_FALSE((wn::core::is_same<const wn_bool, wn_uint32>::value));
  EXPECT_FALSE((wn::core::is_same<wn_int32, volatile wn_float32>::value));
  EXPECT_FALSE(
      (wn::core::is_same<const volatile wn_uint32, const wn_bool>::value));
  EXPECT_FALSE((wn::core::is_same<volatile wn_float32, wn_int32>::value));
  EXPECT_FALSE((wn::core::is_same<wn_uint32, wn_bool>::value));
  EXPECT_FALSE((wn::core::is_same<wn_float32, wn_int32>::value));
  EXPECT_FALSE((wn::core::is_same<wn_uint32, wn_int32>::value));
}

TEST(type_traits, are_same) {
  EXPECT_TRUE((wn::core::are_same<wn_bool, wn_bool, wn_bool>::value));
}

TEST(type_traits, is_same_decayed) {
  EXPECT_TRUE((wn::core::is_same_decayed<wn_bool, wn_bool>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<const wn_bool, wn_bool>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<wn_int32, volatile wn_int32>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<const volatile wn_uint32,
      const wn_uint32>::value));
  EXPECT_TRUE(
      (wn::core::is_same_decayed<volatile wn_float32, wn_float32>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<const wn_bool, wn_uint32>::value));
  EXPECT_FALSE(
      (wn::core::is_same_decayed<wn_int32, volatile wn_float32>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<const volatile wn_uint32,
      const wn_bool>::value));
  EXPECT_FALSE(
      (wn::core::is_same_decayed<volatile wn_float32, wn_int32>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<wn_uint32, wn_bool>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<wn_float32, wn_int32>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<wn_uint32, wn_int32>::value));
}

TEST(type_traits, is_arithmetic) {
  EXPECT_TRUE(wn::core::is_arithmetic<wn_float8>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const wn_float16>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<volatile wn_float32>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const volatile wn_float64>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<wn_fixed8>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const wn_fixed16>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<volatile wn_fixed32>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const volatile wn_fixed64>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<void>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<const void*>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<volatile void*>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<const volatile void*>::value);
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